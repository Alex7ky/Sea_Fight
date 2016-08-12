#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>

static int create_sock();
static struct sockaddr_in create_sockaddr_in(const unsigned short int, 
	                                         const char *);
static void input_serv_info(unsigned short int *, 
                            char *);
static void input_serv_info(unsigned short int *, 
                            char *);
static int create_connect_serv();

#endif