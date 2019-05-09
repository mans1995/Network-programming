#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
/* struct sockaddr_in */
#include <netinet/in.h>
#include <arpa/inet.h>
/* memset */
#include <string.h>

#define MAX_CLIENTS 50

int initSocketServer(int port, int maxClients);

int initSocketClient(char serverIp[16], int port);

#endif
