#include "stdio.h"
#include <winsock2.h>
#include "Ws2tcpip.h"

#pragma comment(lib,"ws2_32.lib") //Winsock Library

int startlisten(char* port) {
	struct sockaddr_storage their_addr;
	struct addrinfo hints, *res;
	int sockfd, new_fd;
	socklen_t addr_size;

	printf("\nTrying to listen on %s", port);

	// first, load up address structs with getaddrinfo():
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;  // use IPv4 or IPv6, whichever
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

	int addri = getaddrinfo(NULL, port, &hints, &res);
	if (addri != 0) {
		fprintf(stderr, "\ngetaddrinfo: %s\n", gai_strerror(addri));
		return 1;
	}


	// make a socket:
	sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

	// bind it to the port we passed in to getaddrinfo():
	bind(sockfd, res->ai_addr, res->ai_addrlen);

	int backlog = 3; // number of connections that can be queued
	listen(sockfd, backlog);

	printf("\nNow listening on port %s", port);

	addr_size = sizeof their_addr;
	if((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size)) < 0) {
		printf("\nconnection failed");
		return 1;
	}
	
	puts("Connection accepted");

	int buffer_size = 2000;
	char buf[2000];
	int recv_size;

	if ((recv_size = recv(new_fd, buf, buffer_size, 0)) != SOCKET_ERROR) {
		buf[recv_size] = '\0';
		printf("\nrecieved: %s", buf);
	} else {
		printf("\nrecv failed with error code : %d", WSAGetLastError());
		return 1;
	}

	char* msg = "HTTP/1.1 200 OK\r\n" \
		"Date: Saturday, 21 May 2016 23:23:23 GMT\r\n" \
		"Server: Bloodcake 1.0\r\n" \
		"Content-Type: text/html\r\n" \
		"Content-Length: 48\r\n" \
		"Connection: Closed\r\n\r\n" \
		"<html><body><h1>Hello, World!</h1></body></html>";

	if (send(new_fd, msg, strlen(msg) + 1, 0) < 0) {
		printf("\nSend failed");
		return 1;
	}

	printf("\n\nsent: \n\n%s ", msg);

	closesocket(new_fd);

	//getchar();

	return 0;
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("\n bad argument, use like: servr 1111");
		return 1;
	}

	WSADATA wsa; // Will hold info on socket

	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		printf("Failed. Error Code : %d", WSAGetLastError());
		return 1;
	}
	printf("Initialised.");

	startlisten(argv[1]);

	WSACleanup();
	return 0;
}

