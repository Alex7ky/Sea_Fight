#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "net.h"
#include "graphics.h"

#define MIN_VAL 1024
#define MAX_VAL 32000

int CreateSock();
int CreateConnectServ();
void InitSockaddrIn(struct sockaddr_in *addr, const unsigned short int,
                    const char *);
void InputServInfo(unsigned short int *, char *);

#endif