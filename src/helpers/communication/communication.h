#ifndef _COMMUNICATION_H_
#define _COMMUNICATION_H_

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "messenger.h"

typedef struct Header {
    uint8_t type;              // PCKT_*
    uint32_t length;           // Tālākā satura garums (baitos)
} __attribute__((packed)) msg_header_t;

class Communicator : Messenger{
private:
    /**
     * Currently connected client count
     */
    uint32_t clientCount;
    /**
     * Buffer to create strings for messenger class
     */
    char msgBuffer[512];
public:
    Communicator();
    ~Communicator();
    void setClientCounter(uint32_t *ptr);
    void incrClientCount();
    void decrClientCount();
    uint8_t getClientCount();

    /**
     * Info:
     * function connects to socket on address and port given and returns opened
     * sockets file discriptor into sockfd
     * Input:
     * const char *ip - ipv4 address on which the server is located
     * int port    - port on the server
     * Output:
     * int *scokfd  - fd of opened socket will be stored here
     */
    uint8_t ConnectToServer(const char *ip, const char *port, int *sockfd);

    /**
     * Creates a socket and binds it to listen for any incoming messages from any ip
     * on speccified port
     *
     */
    uint8_t CreateListenSocket(const char *port, int *listenfd);

    /**
     * Wait 10 sec for a connection from client, if succesfull, then return fd of
     * opened socket.
     */
    int ServerAcceptClient(int * listenfd);

    /**
     * Send message to given fd
     * Input:
     * int sendf            - file discriptor to which send the given data
     * void *buf            - pointer to the data buffer
     * size_t len           - length of data that has to be sent
     * uint8_t type         - type of the message that will be sent
     */
    uint8_t SendMessage(int sendfd, void * buf, size_t len, uint8_t msg_type);

    /**
     * Attempts to recieve a message from gived file discriptor;
     * Returns pointer to the received message payload, this has to be freed later.
     * Input:
     * int readfd           - file discriptor from which to read data;
     * Output:
     * uint8_t *msg_type    - type of the received message;
     */
    void *RecieveMessage(int readfd, uint8_t *msg_type, uint8_t *timeout);

};

#endif /* _COMMUNICATION_H_ */
