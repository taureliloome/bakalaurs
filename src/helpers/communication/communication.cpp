#include "communication.h"
extern void * memset(void * ptr, int value, size_t num);
static void *clientHandler(void *fd);

bool Communicator::instanceFlag = false;
Communicator* Communicator::self = NULL;

Communicator::Communicator(bool setServer = false, const char *setName = "UNNAMED") :
        Messenger(MSG_DEBUG3, setName) {
    info("Communicator started");
    transformer = NULL;
    connections = NULL;
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
    debug2("Transformer set to %p", transformer);
}

void Communicator::PassToTransformer(transfer_t *reply, size_t len) {
    if (!transformer) {
        error("No transformer was set");
    }
    if (!transformer->transform(reply, len)) {
        debug2("Message transformed, passing to analyzer ( NOT YET DONE !!!!!!!!!!! )");
    }

}

void Communicator::incrClientCount() {
    clientCount++;
    debug2("client count increased to %d", clientCount);
}

void Communicator::decrClientCount() {
    clientCount--;
    debug2("client count decreased to %d", clientCount);
}

uint8_t Communicator::getClientCount() {
    return clientCount;
}

uint8_t *Communicator::getConnectionTimeout(int id) {
    return &connections[id].timeout;
}

bool Communicator::initConnectionRegister() {
    connections = (connection_t *) malloc(sizeof(connection_t) * connectionCount);
    if (connections == NULL)
        return false;
    memset(connections, 0, sizeof(connection_t) * connectionCount);
    return true;
}

int Communicator::registerConnection(int fd) {
    int id = getFreeClientId();
    connections[id].fd = fd;
    connections[id].used = true;
    connections[id].timeout = 0;
    debug("Connection registered  %d/%d/%p", id, fd, &connections[id]);
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
    if (connections != NULL && id < connectionCount)
        return connections[id].fd;
    return -1;
}

connection_t *Communicator::getConnectionPtr(int id) {
    if (connections != NULL && id < connectionCount) {
        debug2("connection ptr %p for id %d", &connections[id], id);
        return &connections[id];
    }
    return NULL;
}
connection_t *Communicator::getServerConnPtr() {
    if (connections != NULL)
        return &connections[0];
    return NULL;
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
        error("Unable to bind to the listen socket: %d", errorNumber);
        return 0;
    }
    notice("Server started on with given port: %s", port);
    return 1;
}

int Communicator::ServerAcceptClient() {
    listen(listenfd, 10);
    int id = 0;
    int connfd = accept(listenfd, (struct sockaddr*) NULL, NULL);
    if (connfd) {
        id = registerConnection(connfd);
        debug("Accepted client number: %d, connfd: %d", getClientCount() - 1,
                getConnectionFd(getClientCount() - 1));
        return id;
    }
    perror("Unable to accept a connection");
    return 0;
}

bool Communicator::waitForConnection() {
    int id = ServerAcceptClient();

    if (id >= 0) {
        notice("New connection (%d|%p)", getConnectionFd(id), getConnectionPtr(id));

        pthread_create(&derive_thread, 0, &clientHandler, (void*) &id);
        pthread_detach(derive_thread);
        notice("Connection accepted");
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
    sprintf(msgBuffer, "New thread started id: %d, fd: %d;", id, conn->fd);
    self->_debug(msgBuffer);

    void *reply;
    size_t len = 0;
    transfer_t *msg = Primal::getInstance()->fileListToMessage(&len);
    self->SendMessage(conn, msg, len, MSG_FILE_LIST);

    while (conn->used) {
        reply = self->RecieveMessage(conn, &len);
        free(reply);
        if (conn->timeout >= 5) {
            memset(msgBuffer, 0, 512);
            sprintf(msgBuffer, "<HANDLER> Dropping session %d due to timeout", id);
            self->SendMessage(conn, NULL, 0, MSG_END_OF_COMMUNICATION);
            self->_notice(msgBuffer);
            self->decrClientCount();
            conn->used = false;
            break;
        }
    }
    self->transformer->printFiles(true);
    memset(msgBuffer, 0, 512);
    sprintf(msgBuffer, "<HANDLER> Connection %d session ended", 0);
    self->_debug(msgBuffer);
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
    if (conn) {
        int sendfd = conn->fd;
        debug2("Attempting to send a message");
        struct Header msg_hdr;
        msg_hdr.type = msg_type;
        msg_hdr.length = htonl(len);
        if (sizeof(msg_hdr) != write(sendfd, &msg_hdr, sizeof(msg_hdr))) {
            perror("Unable to send msg header");
            return false;
        }
        if (len != write(sendfd, buf, len)) {
            perror("Unable to send msg");
            return false;
        }
        debug2("Message sent fd %d, type: %u; len:%u", sendfd, msg_type, (uint32_t ) len);
        return true;
    }
    error("Invalid connection ptr");
    return false;
}

void *Communicator::RecieveMessage(connection_t *conn, size_t *len) {
    int readfd = conn->fd;

    debug3("Attempting to receive a message from fd:%d", readfd);

    msg_header_t msg_hdr;
    uint64_t received = 0;
    struct timeval m_timeout;
    m_timeout.tv_sec = 1;
    m_timeout.tv_usec = 0;

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
    while (received < sizeof(msg_hdr)) {
        received += read(readfd, &msg_hdr + received, sizeof(msg_hdr) - received);

        debug3("%lu/%lu", received, sizeof(msg_hdr));
        /* TODO timeouts !
         if ( received != sizeof(msg_hdr) ){
         error("Unable to read the message");
         return NULL;
         }
         */
    }
    msg_hdr.length = ntohl(msg_hdr.length);
    local_msg_hdr.type = msg_hdr.type;

    debug2("Received message header type: %d, len: %lu fd:%d", msg_hdr.type, msg_hdr.length,
            readfd);

    void * buf = NULL;
    switch (msg_hdr.type) {
    case MSG_END_OF_COMMUNICATION:
        debug("END OF COMMUNICATION received, closing connection");
        decrClientCount();
        return NULL;
        break;
    case MSG_HEART_BEAT:
        debug3("Heart beat message received");
        conn->timeout = 0;
        return NULL;
    case MSG_FILE_LIST:
        debug3("incoming file list received");
        received = 0;
        buf = malloc(msg_hdr.length + 1);
        while (received < msg_hdr.length) {
            received += read(readfd, (void *) ((uint64_t) buf + received), msg_hdr.length - received);
            debug3("%lu/%lu", received, msg_hdr.length);
        }
        /* TODO timeouts !
         if ( received != msg_hdr.length ){
         error("Unable to read the message");
         free(buf);
         return NULL;
         } */
        if (len)
            *len = msg_hdr.length;
        return buf;
        break;
    case MSG_DATA:
        debug3("incoming data received");
        received = 0;
        buf = malloc(msg_hdr.length + 1);
        while (received < msg_hdr.length) {
            received += read(readfd, (void *) ((uint64_t) buf + received), msg_hdr.length - received);
            debug3("%lu/%lu", received, msg_hdr.length);
        }
        /* TODO timeouts !
         if ( received != msg_hdr.length ){
         error("Unable to read the message");
         free(buf);
         return NULL;
         } */
        PassToTransformer((transfer_t *)buf, msg_hdr.length);
        return buf;
        break;
    default:
        error("UNKNOWN message received");
        return NULL;
        break;
    }
    return NULL;
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
