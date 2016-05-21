#include "stdio.h"
#include <winsock2.h>
#include "Ws2tcpip.h"

#pragma comment(lib,"ws2_32.lib") //Winsock Library

char* append(char* first, char* second) {
	int size = strlen(first)+1;
	int newSize = size + strlen(second);
	char* nieuw = (char*)malloc(newSize);
	strcpy_s(nieuw, newSize, first);
	nieuw[size] = '\0';
	strcat_s(nieuw, newSize, second);
	return nieuw;
}

int printip(char* hostname) {
	struct addrinfo hints, *res = NULL;
	memset(&hints, 0, sizeof hints);
	hints.ai_flags = 0;
	hints.ai_family = AF_UNSPEC; // IPv4 and IPv6 allowed
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	int addri = getaddrinfo(hostname, NULL, &hints, &res);
	if (addri != 0) {
		fprintf(stderr, "\ngetaddrinfo: %s\n", gai_strerror(addri));
		return 1;
	}

	char ipstr[INET6_ADDRSTRLEN];
	if (res->ai_family == AF_INET) {
		struct sockaddr_in *ipv4 = (struct sockaddr_in *)res->ai_addr;
		void *addr = &(ipv4->sin_addr);
		printf("\n\tIPv4 address %s\n", inet_ntop(res->ai_family, addr, ipstr, sizeof ipstr));
	}
	
	freeaddrinfo(res);

	return 0;
}

int connect_to(char* addr, char* port) {
	struct addrinfo hints, *res = NULL;
	int sockfd;

	memset(&hints, 0, sizeof hints);
	hints.ai_flags = 0;
	hints.ai_family = AF_UNSPEC; // IPv4 and IPv6 allowed
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	int addri = getaddrinfo(addr, port, &hints, &res);
	if (addri != 0) {
		fprintf(stderr, "\ngetaddrinfo: %s\n", gai_strerror(addri));
		return 1;
	}

	// make a socket:
	sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

	int c = connect(sockfd, res->ai_addr, res->ai_addrlen);
	if(c != 0) {
		printf("\nerror connecting: %d", errno);
	}
	freeaddrinfo(res);

	printf("\n Connected to %s : %s", addr, port);

	char* msg = "GET / HTTP/1.1\r\nHost: ";
	msg = append(msg, addr);
	msg = append(msg, ":");
	msg = append(msg, port);
	msg = append(msg, "\r\n\r\n");
	printf("\n\nsending: \n %s \n", msg);
	//char* msg = "GET / HTTP/1.1\r\n\r\n";
	if (send(sockfd, msg, strlen(msg), 0) < 0) {
		printf("\nSend failed");
		return 1;
	}

	printf("\nSend success");
	char server_reply[512];
	int recv_size;

	do {
		if ((recv_size = recv(sockfd, server_reply, 512, 0)) == SOCKET_ERROR) {
			printf("\nrecv failed: %d\n", WSAGetLastError());
			return 1;
		}

		printf("\n\nreply: %s", server_reply);

		/*
		//recv_size = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0)
			printf("Bytes received: %d\n", iResult);
		else if (iResult == 0)
			printf("Connection closed\n");
		else
			printf("recv failed: %d\n", WSAGetLastError());
			*/

	} while (recv_size > 0);

	//Receive a reply from the server
	

	printf("\nReply received\n");

	//Add a NULL terminating character to make it a proper string before printing
	server_reply[recv_size] = '\0';
	printf("\n\nreply: %s", server_reply);

	return 0;
}


int main(int argc, char *argv[]) {
	if (argc == 1) {
		printf("\nnot enough arguments, use like: ");
		printf("\nconsole www.example.com to get ip");
		printf("\nconsole www.example.com 11111 to connect");
		return 1;
	}
	
	WSADATA wsa; // Will hold info on socket
	
	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		printf("Failed. Error Code : %d", WSAGetLastError());
		return 1;
	}
	printf("Initialised.\n");
	
	if (argc == 2)
		printip(argv[1]);
	else if(argc == 3)
		connect_to(argv[1], argv[2]);
	
	WSACleanup();
	return 0;
}
