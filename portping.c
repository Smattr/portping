/*
	User: Matthew Fernandez
	Date: 11:29 AM 26/11/2009
	
	Creates a base TCP connection to a specified server and port to verify connectivity.
*/

/* You'll need to pass the switch -lws2_32 to gcc for the Winsock components to compile */
#include <winsock2.h>
#include <ws2tcpip.h>

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

int main(int argc, char **argv)
{
	int sockfd, portno;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	WORD wVersionRequested;
	WSADATA wsaData;
	struct timeval tick;
	struct timeval tock;
	int result;
	long elapsed;
	
	if (argc < 3)
	{
		fprintf(stderr,"usage %s hostname port", argv[0]);
		return 0;
	}

	portno = atoi(argv[2]);
	if (portno == 0)
	{
		fprintf(stderr, "Invalid port number\n");
		return 0;
	}
	
	/* Initialise Winsock */
	wVersionRequested = MAKEWORD(1, 0);
	if (WSAStartup(wVersionRequested, &wsaData) != 0)
	{
        fprintf(stderr, "Failed to load Winsock\n");
        return 0;
    }

	do
	{
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd < 0)
		{
			perror("Error opening socket");
			WSACleanup();
			return 0;
		}
		
		server = gethostbyname(argv[1]);
		if (server == NULL)
		{
			perror("Error: no such host");
			WSACleanup();
			return 0;
		}
		
		gettimeofday(&tick, 0);
		memset(&serv_addr, 0, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		memmove(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
		serv_addr.sin_port = htons(portno);
		result = connect(sockfd, &serv_addr, sizeof(serv_addr));
		gettimeofday(&tock, 0);
		elapsed = (tock.tv_sec - tick.tv_sec) * 1000 + (tock.tv_usec - tick.tv_usec) / 1000;
		
		if (result < 0) 
			fprintf(stderr, "Connection failed (time=%dms)\n", elapsed);
		else
			printf("Response from %s:%s (time=%dms)\n", argv[1], argv[2], elapsed);
		closesocket(sockfd);
	}
	while (argc >= 4 && strcmp(argv[3], "-t") == 0);
	
	/* Unload Winsock resources */
	WSACleanup();
	return 0;
}

