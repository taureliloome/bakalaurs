#include "communication.h"
extern void * memset(void * ptr, int value, size_t num);
static void *clientHandler(void *fd);

bool Communicator::instanceFlag = false;
Communicator* Communicator::self = NULL;

Communicator::Communicator() :
        Messenger(MSG_DEBUG3) {
    info("Communicator started");
    if (!initConnectionRegister()) {
        error("Unable to initialize ID list");
    }
    serverFd = 0;
    accept_thread = 0;
    derive_thread = 0;
    clientCount = 0;
    memset(msgBuffer, 0, 512);
    connectionTimeout = (uint8_t *) malloc(sizeof(uint8_t) * connectionCount);
    connectionFDs = (uint32_t*) malloc(sizeof(uint8_t) * connectionCount);
    memset(connectionTimeout, 0, sizeof(uint8_t) * connectionCount);
    memset(connectionFDs, 0, sizeof(uint8_t) * connectionCount);
}

Communicator *Communicator::getInstance() {
    if (!instanceFlag) {
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

/* helpers */
void Communicator::incrClientCount() {
    clientCount++;
}

void Communicator::decrClientCount() {
    clientCount--;
}

uint8_t Communicator::getClientCount() {
    return clientCount;
}

uint8_t *Communicator::getConnectionTimeout(int id) {
    return &connectionTimeout[id];
}

bool Communicator::initConnectionRegister(){
    connectionFDs = (uint32_t*) malloc(sizeof(int)*connectionCount);
    if ( connectionFDs == NULL )
        return false;
    return true;
}

int Communicator::registerConnection(int fd) {
    connectionFDs[getClientCount()] = fd;
    incrClientCount();
    return getClientCount() - 1;
}

int Communicator::getConnectionFd(int id) {
    return connectionFDs[id];
}

int *Communicator::getConnectionFdPtr(int id){
    return (int *) &connectionFDs[id];
}

/* Server Side */
uint8_t Communicator::CreateListenSocket(const char *port) {
    int i_port = 0;
    struct sockaddr_in serv_addr;

    memset(&serv_addr, '0', sizeof(serv_addr));
    sscanf(port, "%d", &i_port);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY );
    serv_addr.sin_port = htons(i_port);

    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    if (bind(listenfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr))) {
        int errorNumber = errno;
        memset(msgBuffer, 0, 512);
        sprintf(msgBuffer, "Unable to bind to the listen socket: %d", errorNumber);
        error(msgBuffer);
        return 0;
    }
    memset(msgBuffer, 0, 512);
    sprintf(msgBuffer, "Server started on with given port: %s", port);
    notice(msgBuffer);
    return 1;
}

int Communicator::ServerAcceptClient() {
    listen(listenfd, 10);
    int connfd = accept(listenfd, (struct sockaddr*) NULL, NULL);
    if (connfd) {
        registerConnection(connfd);
        memset(msgBuffer, 0, 512);
        sprintf(msgBuffer, "Accepted client number: %d, connfd: %d", getClientCount() - 1,
                getConnectionFd(getClientCount() - 1));
        debug(msgBuffer);
        return connfd;
    }
    perror("Unable to accept a connection");
    return 0;
}

bool Communicator::waitForConnection() {
    int id = ServerAcceptClient();

    if (id > 0) {
        memset(msgBuffer, 0, 512);
        sprintf(msgBuffer, "<SERVER> New connection (%d|%p)", getConnectionFd(id),
                getConnectionFdPtr(id));
        notice(msgBuffer);

        pthread_create(&derive_thread, 0, &clientHandler, (void*) getConnectionFdPtr(id));
        pthread_detach(derive_thread);
        notice("<SERVER> Connection accepted");
    } else {
        error("Unable to accept client");
        return false;
    }
    return true;
}

static void *clientHandler(void *fd) {
    Communicator *self = Communicator::getInstance();
    self->debug("<SERVER> New thread started;");
    int connfd = *(int *) fd;
    int a = 5639, i = 0;
    void *upd_pkt;
    size_t upd_pkt_len = 0;

    char msgBuffer[512];

    memset(msgBuffer, 0, 512);
    sprintf(msgBuffer, "<SERVER> thread fd:%d %p", *(int*) fd, fd);
    self->debug(msgBuffer);

    while (1) {
        self->RecieveMessage(connfd);
        self->SendMessage(connfd, &message, sizeof(message), 0);
        if (*(self->getConnectionTimeout(0)) >= 5) {
            memset(msgBuffer, 0, 512);
            sprintf(msgBuffer, "<SERVER> Dropping session %d due to timeout", connfd);
            self->notice(msgBuffer);
            break;
        }
    }
    memset(msgBuffer, 0, 512);
    sprintf(msgBuffer, "<SERVER> Connection %d session ended", 0);
    self->debug(msgBuffer);
    close(connfd);
    return NULL;
}

/* Client Side */
bool Communicator::ConnectToServer(const char *ip, const char *port) {
    int i_port = 0;
    struct sockaddr_in serv_addr;

    memset(&serv_addr, '0', sizeof(serv_addr));
    sscanf(port, "%d", &i_port);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(i_port);

    if ((serverFd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        error("Could not create socket:");
        return false;
    }

    if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0) {
        error("inet_pton error occurred");
        return false;
    }

    if (connect(serverFd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("Connect Failed");
        return false;
    }
    notice("Connected to server");
    return true;
}

/* Communication */
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

void *Communicator::RecieveMessage(int readfd) {
    if (readfd == 0)
        readfd = serverFd;
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
        (*connectionTimeout)++;
        return NULL;
    } else if (res == -1) {
        error("Error in select:");
        return NULL;
    }
    (*connectionTimeout) = 0;
    while (received != sizeof(msg_hdr)) {
        received += read(readfd, &msg_hdr + received, sizeof(msg_hdr) - received);
    }
    msg_hdr.length = ntohl(msg_hdr.length);
    local_msg_hdr.type = msg_hdr.type;

    memset(msgBuffer, 0, 512);
    sprintf(msgBuffer, "Received message header type: %d, len: %lu", msg_hdr.type, msg_hdr.length);
    debug2(msgBuffer);

    received = 0;
    void * buf = malloc(msg_hdr.length);
    while (received != msg_hdr.length) {
        received += read(readfd, (void *) ((uint64_t) buf + received), msg_hdr.length - received);

        memset(msgBuffer, 0, 512);
        sprintf(msgBuffer, "%lu/%lu", received, msg_hdr.length);
        debug2(msgBuffer);
    }
    return buf;
}

void Communicator::communicate() {
    while (connectionCount) {
        RecieveMessage(connectionFDs[getClientCount() - 1]);
    }
}
