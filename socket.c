#include "socket.h"
#include "utils.h"

int initSocketServer(int port, int maxClients) {	
	int sockfd;
	int ret;
	struct sockaddr_in addr;
	// socket creation
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	checkNeg(sockfd, "Error socket server");
	// prepare sockaddr to bind
	memset(&addr,0, sizeof(addr));  // met à 0 tous les bits pointés par &addr sur une taille de sizeof(addr); memset ne renvoie pas -1
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	// listen on all server interfaces
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	ret = bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
	checkNeg(ret, "Error bind server");
	// simultaneous client max -> 2
	ret = listen(sockfd, maxClients);
	checkNeg(ret, "Error listen server");
	return sockfd;		
}

int initSocketClient(char serverIp[16], int port) {
	int sockfd;
  	struct sockaddr_in addr;

  	sockfd = socket(AF_INET,SOCK_STREAM, 0 );
  	memset(&addr , 0 , sizeof(addr));
  	addr.sin_family = AF_INET;
  	addr.sin_port = htons(port);
  	checkNeg(inet_aton(serverIp, &addr.sin_addr),"error aton");
  	connect(sockfd, (struct sockaddr *) &addr, sizeof(addr));
  	return sockfd;
}
