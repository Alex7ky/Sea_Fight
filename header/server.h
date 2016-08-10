#ifndef _SERVER_H_
#define _SERVER_H_

#include "net.h"

#include <stdio.h>
/*
        close()
*/
#include <unistd.h>
/*
        socket()
*/
#include <sys/types.h>
#include <sys/socket.h>
/*
        inet_aton()
*/
#include <arpa/inet.h>
#include <netinet/in.h>
/*
        pthreads
*/
#include <pthread.h>

/*
        SERVER_S - information by server
                sock_d - socket descriptor
                client_1 - descriptor of stream client 1
                client_2 - descriptor of stream client 2
                sock_addr - struct address information of server
                print - pthread id, service for print log of server
                network - pthread id, network service
                mutex - block read/write for this structure
*/
#define SERVER_S struct mServerInformation

SERVER_S {
        int sock_d;
        int client_1;
        int client_2;
        sockaddr_in sock_addr;
        pthread_h print;
        pthread_h network;
        pthread_mutex_t mutex;
};
/*
        Server commands
                CM_CLOSE - close server
                CM_HELP - help
                CM_UNDEF - command not found
*/
#define CM_CLOSE 0
#define CM_HELP 1
#define CM_UNDEF -1
/*
        Server functions
*/
SERVER_S InitServer();
pthread_t InitLOGService();
pthread_t InitNetworkService();
void *PrintLogService(void *);
void *NetworkService(void *);
int GetCommand();
int GetIP(char *buff);
int GetPORT(int *port);

#endif
