#ifndef _COMMUNICATION_H_
#define _COMMUNICATION_H_

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "transformer.h"
extern "C"{
#include <pthread.h>
}
#include "messenger.h"

typedef struct Header {
    uint8_t type;
    uint64_t length;
} __attribute__((packed)) msg_header_t;

typedef struct connection_s{
    bool used;
    uint8_t timeout;
    int fd;
} connection_t;


class Communicator : public Messenger{
private:
    static const uint32_t connectionCount = 2;
    static bool instanceFlag;
    bool isServer;
    /**
     * Currently connected client count
     */
    uint32_t clientCount;
    /**
     * Buffer to create strings for messenger class
     */
    pthread_t accept_thread;
    pthread_t derive_thread;

    int listenfd;
    connection_t *connections;
    msg_header_t local_msg_hdr;
    static Communicator *self;
    Transformer *transformer;
    Communicator(bool setServer, const char *setName);
public:
    void PassToTransformer(const char *reply);
    static Communicator *getInstance(bool setServer, const char *setName);
    static Communicator *getInstance();
    ~Communicator();
    void setTransformer(Transformer *new_transformer);
    void setClientCounter(uint32_t *ptr);
    void incrClientCount();
    void decrClientCount();
    int getFreeClientId();
    uint8_t getClientCount();

    /**
     * Info:
     * function connects to socket on address and port given and returns opened
     * sockets file descriptor into sockfd
     * Input:
     * const char *ip - ipv4 address on which the server is located
     * int port    - port on the server
     * Output:
     * int *scokfd  - fd of opened socket will be stored here
     */
    bool ConnectToServer(const char *ip, const char *port);

    /**
     * Creates a socket and binds it to listen for any incoming messages from any ip
     * on specified port
     *
     */
    uint8_t CreateListenSocket(const char *port);

    /**
     * Wait 10 sec for a connection from client, if successful, then return fd of
     * opened socket.
     */
    int ServerAcceptClient();

    /**
     * Send message to given fd
     * Input:
     * int id               - id of client to which send the given data, ignored if it's the client
     * void *buf            - pointer to the data buffer
     * size_t len           - length of data that has to be sent
     * uint8_t type         - type of the message that will be sent
     */
    bool SendMessage(connection_t *conn, void * buf, size_t len, uint8_t msg_type);

    /**
     * Attempts to receive a message from given file descriptor;
     * Returns pointer to the received message payload, this has to be freed later.
     * Input:
     * int readfd           - file descriptor from which to read data;
     * Output:
     * uint8_t *msg_type    - type of the received message;
     */
    void *RecieveMessage(connection_t *conn);

    bool waitForConnection();

    bool initConnectionRegister();
    int registerConnection(int fd);
    int getConnectionFd(int id);
    connection_t *getServerConnPtr();
    connection_t *getConnectionPtr(int id);

    uint8_t *getConnectionTimeout(int id);

    void communicate();
};

#endif /* _COMMUNICATION_H_ */
