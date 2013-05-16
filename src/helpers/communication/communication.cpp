#include "communication.h"
extern void * memset(void * ptr, int value, size_t num);
static void *clientHandler(void *fd);

bool Communicator::instanceFlag = false;
Communicator* Communicator::self = NULL;

Communicator::Communicator(bool setServer = false, const char *setName = "UNNAMED") :
        Messenger(MSG_DEBUG3, setName) {
    info("Communicator started");
    transformer = NULL;
    isServer = false;
    if (!initConnectionRegister()) {
        error("Unable to initialize ID list");
    }
    accept_thread = 0;
    derive_thread = 0;
    clientCount = 0;
    memset(msgBuffer, 0, 512);
}

Communicator *Communicator::getInstance(bool setServer, const char *setName) {
    if (!instanceFlag) {
        self = new Communicator(setServer, setName);
        instanceFlag = true;
        return self;
    }
    return self;
}

Communicator *Communicator::getInstance() {
    return self;
}

Communicator::~Communicator() {
    free(connections);
    close(listenfd);
    info("Communicator removed");
}

/* helpers */
void Communicator::setTransformer(Transformer *new_transformer) {
    transformer = new_transformer;
    memset(msgBuffer, 0, 512);
    sprintf(msgBuffer, "Transformer set to %p", transformer);
    debug2(msgBuffer);
}

void Communicator::PassToTransformer(const char *reply) {
    if (!transformer){
        error("No transformer was set");
    }
    if (!transformer->transform(reply)) {
        debug2("Message transformed, passing to analyzer ( NOT YET DONE !!!!!!!!!!! )");
    }

}

void Communicator::incrClientCount() {
    clientCount++;
    memset(msgBuffer, 0, 512);
    sprintf(msgBuffer, "client count increased to %d", clientCount);
    debug2(msgBuffer);
}

void Communicator::decrClientCount() {
    clientCount--;
    memset(msgBuffer, 0, 512);
    sprintf(msgBuffer, "client count decreased to %d", clientCount);
    debug2(msgBuffer);
}

uint8_t Communicator::getClientCount() {
    return clientCount;
}

uint8_t *Communicator::getConnectionTimeout(int id) {
    return &connections[id].timeout;
}

bool Communicator::initConnectionRegister() {
    connections = (connection_t *) malloc(sizeof(connection_t) * connectionCount);
    memset(connections, 0, sizeof(connection_t) * connectionCount);
    if (connections == NULL)
        return false;
    return true;
}

int Communicator::registerConnection(int fd) {
    int id = getFreeClientId();
    connections[id].fd = fd;
    connections[id].used = true;
    connections[id].timeout = 0;
    memset(msgBuffer, 0, 512);
    sprintf(msgBuffer, "Connection registered  %d/%d/%p", id, fd, &connections[id]);
    debug(msgBuffer);
    return id;
}

int Communicator::getFreeClientId() {
    int i;
    for (i = 0; connections[i].used && i < connectionCount; i++)
        ;
    incrClientCount();
    return i;
}

int Communicator::getConnectionFd(int id) {
    if (id < connectionCount)
        return connections[id].fd;
    return -1;
}

connection_t *Communicator::getConnectionPtr(int id) {
    memset(msgBuffer, 0, 512);
    sprintf(msgBuffer, "connection ptr %p for id %d", &connections[id], id);
    debug2(msgBuffer);
    return &connections[id];
}
connection_t *Communicator::getServerConnPtr() {
    return &connections[0];
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
    int id = 0;
    int connfd = accept(listenfd, (struct sockaddr*) NULL, NULL);
    if (connfd) {
        id = registerConnection(connfd);
        memset(msgBuffer, 0, 512);
        sprintf(msgBuffer, "Accepted client number: %d, connfd: %d", getClientCount() - 1,
                getConnectionFd(getClientCount() - 1));
        debug(msgBuffer);
        return id;
    }
    perror("Unable to accept a connection");
    return 0;
}

bool Communicator::waitForConnection() {
    int id = ServerAcceptClient();

    if (id >= 0) {
        memset(msgBuffer, 0, 512);
        sprintf(msgBuffer, "<SERVER> New connection (%d|%p)", getConnectionFd(id),
                getConnectionPtr(id));
        notice(msgBuffer);

        pthread_create(&derive_thread, 0, &clientHandler, (void*) &id);
        pthread_detach(derive_thread);
        notice("<SERVER> Connection accepted");
    } else {
        error("Unable to accept client");
        return false;
    }
    return true;
}
/* This handles a separate client */
static void *clientHandler(void *idptr) {
    Communicator *self = Communicator::getInstance();
    int id = *(int *) idptr;
    connection_t *conn = self->getConnectionPtr(id);

    char msgBuffer[512];
    memset(msgBuffer, 0, 512);
    sprintf(msgBuffer, "<SERVER> New thread started id: %d, fd: %d;", id, conn->fd);
    self->debug(msgBuffer);

    char message[] = "pong";
    void *reply;

    while (conn->used) {
        reply = self->RecieveMessage(conn);
        self->PassToTransformer((const char *)reply);
        free(reply);
        if (conn->timeout >= 5) {
            memset(msgBuffer, 0, 512);
            sprintf(msgBuffer, "<SERVER> Dropping session %d due to timeout", id);
            self->SendMessage(conn, NULL, 0, 0xf);
            self->notice(msgBuffer);
            self->decrClientCount();
            conn->used = false;
            break;
        }
    }
    memset(msgBuffer, 0, 512);
    sprintf(msgBuffer, "<SERVER> Connection %d session ended", 0);
    self->debug(msgBuffer);
    conn->used = false;
    close(conn->fd);
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

    connection_t * conn = getServerConnPtr();

    if ((conn->fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        error("Could not create socket:");
        return false;
    }

    if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0) {
        error("inet_pton error occurred");
        return false;
    }

    if (connect(conn->fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("Connect Failed");
        return false;
    }
    incrClientCount();
    notice("Connected to server");
    return true;
}

/* Communication */
bool Communicator::SendMessage(connection_t *conn, void * buf, size_t len, uint8_t msg_type) {
    int sendfd = conn->fd;
    debug2("Attempting to send a message");
    struct Header msg_hdr;
    msg_hdr.type = msg_type;
    msg_hdr.length = htonl(len);
    if (conn) {
        if (sizeof(msg_hdr) != write(sendfd, &msg_hdr, sizeof(msg_hdr))) {
            perror("Unable to send msg header");
            return false;
        }
        if (len != write(sendfd, buf, len)) {
            perror("Unable to send msg");
            return false;
        }
        memset(msgBuffer, 0, 512);
        sprintf(msgBuffer, "Message sent fd %d, type: %u; len:%u", sendfd, msg_type,
                (uint32_t) len);
        debug2(msgBuffer);
        return true;
    }
    error("Invalid connection ptr");
    return false;
}

void *Communicator::RecieveMessage(connection_t *conn) {
    int readfd = conn->fd;

    memset(msgBuffer, 0, 512);
    sprintf(msgBuffer, "Attempting to receive a message from fd:%d", readfd);
    debug2(msgBuffer);

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
        conn->timeout++;
        return NULL;
    } else if (res == -1) {
        error("Error in select:");
        return NULL;
    }
    conn->timeout = 0;
    while (received != sizeof(msg_hdr)) {
        received += read(readfd, &msg_hdr + received, sizeof(msg_hdr) - received);

        memset(msgBuffer, 0, 512);
        sprintf(msgBuffer, "%llu/%u", received, sizeof(msg_hdr));
        debug2(msgBuffer);
        /* TODO timeouts !
         if ( received != sizeof(msg_hdr) ){
         error("Unable to read the message");
         return NULL;
         }
         */
    }
    msg_hdr.length = ntohl(msg_hdr.length);
    local_msg_hdr.type = msg_hdr.type;

    memset(msgBuffer, 0, 512);
    sprintf(msgBuffer, "Received message header type: %d, len: %llu fd:%d", msg_hdr.type,
            msg_hdr.length, readfd);
    debug2(msgBuffer);

    if (msg_hdr.type == 0xf) {
        decrClientCount();
        return NULL;
    }

    received = 0;
    void * buf = malloc(msg_hdr.length);
    while (received != msg_hdr.length) {
        received += read(readfd, (void *) ((uint64_t) buf + received), msg_hdr.length - received);

        memset(msgBuffer, 0, 512);
        sprintf(msgBuffer, "%llu/%llu", received, msg_hdr.length);
        debug2(msgBuffer);
    }
    /* TODO timeouts !
     if ( received != msg_hdr.length ){
     error("Unable to read the message");
     free(buf);
     return NULL;
     } */
    return buf;
}

void Communicator::communicate() {
    connection_t *conn = self->getServerConnPtr();
    char *msg = NULL;
    while (getClientCount()) {
        msg = (char *) self->RecieveMessage(conn);
        free(msg);
    }
    debug2("communication ended");
}
