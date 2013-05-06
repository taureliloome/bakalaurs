#include "communication.h"
extern void * memset ( void * ptr, int value, size_t num );
Communicator::Communicator(){
    clientCount = 0;
    memset(msgBuffer,0,512);
}

void Communicator::incrClientCount()
{
    clientCount++;
}

void Communicator::decrClientCount()
{
    clientCount--;
}

uint8_t Communicator::getClientCount()
{
    return clientCount;
}

uint8_t Communicator::ConnectToServer(const char *ip, const char *port, int *sockfd)
{
    int i_port = 0;
    struct sockaddr_in serv_addr;

    memset(&serv_addr, '0', sizeof(serv_addr));
    sscanf(port, "%d", &i_port);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(i_port);

    if((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        error("Could not create socket: \n");
        return 0;
    }

    if(inet_pton(AF_INET, ip, &serv_addr.sin_addr)<=0)
    {
        error("inet_pton error occurred\n");
        return 0;
    }

    if( connect(*sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        error("Connect Failed \n");
        return 0;
    }
    notice("Connected to server\n");
    return 1;
}

uint8_t Communicator::CreateListenSocket(const char *port, int *listenfd)
{
    int i_port = 0;
    struct sockaddr_in serv_addr;

    memset(&serv_addr, '0', sizeof(serv_addr));
    sscanf(port, "%d", &i_port);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(i_port);

    *listenfd = socket(AF_INET, SOCK_STREAM, 0);

    if ( bind(*listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) )
    {
        error("Unable to bind to the listen socket: ");
        return 0;
    }
    memset(msgBuffer,0,512);
    sprintf(msgBuffer, "Server started on with given port: %s\n", port);
    notice(msgBuffer);
    return 1;
}

int Communicator::ServerAcceptClient(int * listenfd)
{
    listen(*listenfd, 10);
    int connfd = accept(*listenfd, (struct sockaddr*)NULL, NULL);
    if ( connfd )
    {
        incrClientCount();
        memset(msgBuffer,0,512);
        sprintf(msgBuffer,"Accepted client number: %d, connfd: %d \n", getClientCount(),  connfd);
        debug(msgBuffer);
        return connfd;
    }
    perror("Unable to accept a connection");
    return 0;
}

uint8_t Communicator::SendMessage(int sendfd, void * buf, size_t len, uint8_t msg_type)
{
    struct Header msg_hdr;
    msg_hdr.type = msg_type;
    msg_hdr.length = htonl(len);
    if ( sizeof(msg_hdr) != write(sendfd, &msg_hdr, sizeof(msg_hdr)) )
    {
        perror("Unable to send msg header");
        return 0;
    }
    if ( len != write(sendfd, buf, len) )
    {
        perror("Unable to send msg");
        return 0;
    }
    memset(msgBuffer,0,512);
    sprintf(msgBuffer,"Message sent type: %u; len:%u\n",msg_type, (uint32_t) len);
    debug2(msgBuffer);
    return 1;
}

void *Communicator::RecieveMessage(int readfd, uint8_t *msg_type, uint8_t *timeout)
{
    msg_header_t msg_hdr;
    uint32_t received = 0;
    struct timeval m_timeout;
    m_timeout.tv_sec = 0;
    m_timeout.tv_usec = 100;

    fd_set set;
    FD_ZERO(&set);
    FD_SET(readfd, &set);

    int res = select(readfd + 1, &set, NULL, NULL, &m_timeout );

    if ( res == 0 ){
        (*timeout)++;
        return NULL;
    }
    else if ( res == -1 ) {
        error("Error in select:");
        return NULL;
    }
    (*timeout) = 0;
    while ( received != sizeof(msg_hdr) )
    {
        received += read(readfd, &msg_hdr + received, sizeof(msg_hdr) - received );
    }
    msg_hdr.length = ntohl(msg_hdr.length);
    *msg_type = msg_hdr.type;

    memset(msgBuffer,0,512);
    sprintf(msgBuffer,"Received message header type: %d, len: %d\n", msg_hdr.type, msg_hdr.length );
    debug2(msgBuffer);


    received = 0;
    void * buf = malloc(msg_hdr.length);
    while ( received != msg_hdr.length )
    {
        received += read(readfd, buf + received, msg_hdr.length - received );

        memset(msgBuffer,0,512);
        sprintf(msgBuffer,"%u/%u\n", received, msg_hdr.length);
        debug2(msgBuffer);
    }
    return buf;
}
