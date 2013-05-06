#include "communication.h"
extern void * memset(void * ptr, int value, size_t num);
static void *clientHandler(void *fd);

bool Communicator::instanceFlag = false;
Communicator* Communicator::self = NULL;

Communicator::Communicator() :
        Messenger(MSG_INFO) {
    info("Communicator started");
    if ( !idInit() ){
        error("Unable to initialize ID list");
    }
    accept_thread = 0;
    derive_thread = 0;
    clientCount = 0;
    memset(msgBuffer, 0, 512);
    connectionCount = 1;
    connectionTimeout = (uint8_t *) malloc(sizeof(uint8_t) * connectionCount);
    connectionFDs = (uint32_t*) malloc(sizeof(uint8_t) * connectionCount);
    memset(connectionTimeout, 0, sizeof(uint8_t) * connectionCount);
    memset(connectionFDs, 0, sizeof(uint8_t) * connectionCount);
    CreateListenSocket("1337", &listenfd);
}

Communicator *Communicator::getInstance(){
    if (!instanceFlag){
        self = new Communicator();
        instanceFlag = true;
        return self;
    }
    return self;
}

Communicator::~Communicator() {
    free(connectionTimeout);
    free(connectionFDs);
    close(listenfd);
    info("Communicator removed");
}

void Communicator::incrClientCount() {
    clientCount++;
}

void Communicator::decrClientCount() {
    clientCount--;
}

uint8_t Communicator::getClientCount() {
    return clientCount;
}

uint8_t Communicator::ConnectToServer(const char *ip, const char *port, int *sockfd) {
    int i_port = 0;
    struct sockaddr_in serv_addr;

    memset(&serv_addr, '0', sizeof(serv_addr));
    sscanf(port, "%d", &i_port);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(i_port);

    if ((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        error("Could not create socket:");
        return 0;
    }

    if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0) {
        error("inet_pton error occurred");
        return 0;
    }

    if (connect(*sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("Connect Failed");
        return 0;
    }
    notice("Connected to server");
    return 1;
}

uint8_t Communicator::CreateListenSocket(const char *port, int *listenfd) {
    int i_port = 0;
    struct sockaddr_in serv_addr;

    memset(&serv_addr, '0', sizeof(serv_addr));
    sscanf(port, "%d", &i_port);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY );
    serv_addr.sin_port = htons(i_port);

    *listenfd = socket(AF_INET, SOCK_STREAM, 0);

    if (bind(*listenfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr))) {
        error("Unable to bind to the listen socket: ");
        return 0;
    }
    memset(msgBuffer, 0, 512);
    sprintf(msgBuffer, "Server started on with given port: %s", port);
    notice(msgBuffer);
    return 1;
}

int Communicator::ServerAcceptClient(int * listenfd) {
    listen(*listenfd, 10);
    int connfd = accept(*listenfd, (struct sockaddr*) NULL, NULL);
    if (connfd) {
        incrClientCount();
        memset(msgBuffer, 0, 512);
        sprintf(msgBuffer, "Accepted client number: %d, connfd: %d", getClientCount(), connfd);
        debug(msgBuffer);
        return connfd;
    }
    perror("Unable to accept a connection");
    return 0;
}

uint8_t Communicator::SendMessage(int sendfd, void * buf, size_t len, uint8_t msg_type) {
    struct Header msg_hdr;
    msg_hdr.type = msg_type;
    msg_hdr.length = htonl(len);
    if (sizeof(msg_hdr) != write(sendfd, &msg_hdr, sizeof(msg_hdr))) {
        perror("Unable to send msg header");
        return 0;
    }
    if (len != write(sendfd, buf, len)) {
        perror("Unable to send msg");
        return 0;
    }
    memset(msgBuffer, 0, 512);
    sprintf(msgBuffer, "Message sent type: %u; len:%u", msg_type, (uint32_t) len);
    debug2(msgBuffer);
    return 1;
}

void *Communicator::RecieveMessage(int readfd, uint8_t *msg_type, uint8_t *timeout) {
    msg_header_t msg_hdr;
    uint64_t received = 0;
    struct timeval m_timeout;
    m_timeout.tv_sec = 0;
    m_timeout.tv_usec = 100;

    fd_set set;
    FD_ZERO(&set);
    FD_SET(readfd, &set);

    int res = select(readfd + 1, &set, NULL, NULL, &m_timeout);

    if (res == 0) {
        (*timeout)++;
        return NULL;
    } else if (res == -1) {
        error("Error in select:");
        return NULL;
    }
    (*timeout) = 0;
    while (received != sizeof(msg_hdr)) {
        received += read(readfd, &msg_hdr + received, sizeof(msg_hdr) - received);
    }
    msg_hdr.length = ntohl(msg_hdr.length);
    *msg_type = msg_hdr.type;

    memset(msgBuffer, 0, 512);
    sprintf(msgBuffer, "Received message header type: %d, len: %lu", msg_hdr.type,
            msg_hdr.length);
    debug2(msgBuffer);

    received = 0;
    void * buf = malloc(msg_hdr.length);
    while (received != msg_hdr.length) {
        received += read(readfd, (void *) ((uint64_t) buf + received), msg_hdr.length - received);

        memset(msgBuffer, 0, 512);
        sprintf(msgBuffer, "%lu/%lu\n", received, msg_hdr.length);
        debug2(msgBuffer);
    }
    return buf;
}

void Communicator::waitForConnections() {
    int connfd = 0;
    while (getClientCount() < connectionCount) {
        connfd = ServerAcceptClient(&listenfd);
        connectionFDs[getClientCount() - 1] = connfd;

        if (connfd) {
            notice("<SERVER> New player connected");

            /* PlayerID generation TODO: */
            int newId;
            newId = idGen(connfd);
            memset(msgBuffer, 0, 512);
            sprintf(msgBuffer, "newID is ---- %d", newId);
            debug3(msgBuffer);

            uint8_t client_count = getClientCount();
            pthread_create(&derive_thread, 0, &clientHandler, (void*) &connfd);
            pthread_detach (derive_thread);
        }
    }
    notice("<SERVER> All players connected");
}

bool Communicator::idInit() {
    idList = (int*) malloc((connectionCount) * sizeof(int));
    if ( idList == NULL )
        return false;
    for (int i = 0; i < connectionCount; i++) {
        idList[i] = -1;
    }
    return true;
}

int Communicator::idGen(int id) {
    for ( int i = 0; i < connectionCount; i++) {
        if (idList[i] == -1) {
            idList[i] = id;
            return i;
        }
    }
    return -1;
}

uint8_t *Communicator::getConnectionTimeout(int id){
    return &connectionTimeout[id];
}

static void *clientHandler(void *fd)
{
    Communicator *self = Communicator::getInstance();
    int connfd = *(int *)fd;
    uint8_t msg_type = 0;
    int a = 5639, i = 0;
    void *upd_pkt;
    size_t upd_pkt_len = 0;

    int message = 0;
    char msgBuffer[512];

    self->SendMessage(connfd, &message, sizeof(message), 0);

    while(1) {
        self->RecieveMessage(connfd, &msg_type, self->getConnectionTimeout(0));
        self->SendMessage(connfd, &message, sizeof(message), 0);
        if ( *(self->getConnectionTimeout(0)) >= 5 )
        {
            memset(msgBuffer, 0, 512);
            sprintf(msgBuffer, "<SERVER> Dropping player %d due to timeout\n", connfd);
            self->notice(msgBuffer);
            break;
        }
    }
    memset(msgBuffer, 0, 512);
    sprintf(msgBuffer, "<SERVER> Player %d session ended\n", 0);
    self->debug(msgBuffer);
    close(connfd);
    return NULL;
}
