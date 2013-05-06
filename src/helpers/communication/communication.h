#ifndef _COMMUNICATION_H_
#define _COMMUNICATION_H_

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
extern "C"{
#include <pthread.h>
}
#include "messenger.h"

typedef struct Header {
    uint8_t type;
    uint64_t length;
} __attribute__((packed)) msg_header_t;

class Communicator : public Messenger{
private:
    static bool instanceFlag;
    /**
     * Currently connected client count
     */
    uint32_t clientCount;
    /**
     * Buffer to create strings for messenger class
     */
    char msgBuffer[512];
    pthread_t accept_thread;
    pthread_t derive_thread;

    int listenfd;
    int *idList;
    uint32_t connectionCount;
    uint8_t *connectionTimeout;
    uint32_t *connectionFDs;
    static Communicator *self;
    Communicator();
public:
    static Communicator *getInstance();
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

    void waitForConnections();

    /*
    * 1: Init array with {-1, -1, -1 ,-1 ,-1 ... }
    *    when server is started.
    * 2: Pass FD to gen_id, and it will put the FD in first
    *    available player ID space.
    * 3: Return the value of ID if OK.
    */

    //Array of FDs, Index is ID
    //Input values, need to be changed

    /**
    //Debug print

    void print(){
        for(int i=0; i<PLAYER_COUNT; i++){
            printf ("Number: %d\n", tmp_id);
        }
    **/

    //Intialize array with, {-1,-1,-1,-1,-1...}
    bool idInit();

    int idGen(int id);

    uint8_t *getConnectionTimeout(int id);
};

#endif /* _COMMUNICATION_H_ */
