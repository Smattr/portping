/*
    User: Matthew Fernandez
    Date: 11:29 AM 26/11/2009

    Creates a base TCP connection to a specified server and port to verify
    connectivity.
*/

#ifdef _WIN32
    /* You'll need to pass the switch -lws2_32 to gcc for the Winsock components
     * to compile.
     */
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <windows.h>
#else
    #include <netinet/in.h>
    #include <netdb.h>
#endif

#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#define BUFLEN 10

static inline int init(void) {
#ifdef _WIN32
    WORD wVersionRequested;
    WSADATA wsaData;

    /* Initialise Winsock. */
    wVersionRequested = MAKEWORD(1, 0);
    return WSAStartup(wVersionRequested, &wsaData);
#else
    return 0;
#endif
}

static inline void cleanup(void) {
#ifdef _WIN32
    /* Unload Winsock resources. */
    WSACleanup();
#endif
}

int main(int argc, char **argv)
{
    int sockfd, portno;
    struct sockaddr_in serv_addr;
    struct hostent* server;
    struct timeval tick;
    struct timeval tock;
    char buffer[BUFLEN];
    int result;
    long elapsed;
    int protocol;

    if (argc < 3) {
        fprintf(stderr,"Usage: %s hostname port\n", argv[0]);
        return 0;
    }

    portno = atoi(argv[2]);
    if (!portno) {
        fprintf(stderr, "Invalid port number.\n");
        return 0;
    }

    if (init()) {
        perror("Error during initialisation");
        return 1;
    }

    if ((argc > 3) && !strcmp(argv[3], "udp"))
        protocol = SOCK_DGRAM;
    else
        protocol = SOCK_STREAM;

    do {
        sockfd = socket(AF_INET, protocol, 0);

        if (sockfd < 0) {
            perror("Error opening socket");
            cleanup();
            return 1;
        }

        server = gethostbyname(argv[1]);
        if (!server) {
            perror("DNS lookup failed");
            cleanup();
            return 1;
        }

        gettimeofday(&tick, 0);
        memset(&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        memmove(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
        serv_addr.sin_port = htons(portno);
        if (protocol == SOCK_STREAM)
            result = connect(sockfd, &serv_addr, sizeof(serv_addr));
        else {
            result = sendto(sockfd, 0, 0, 0, &serv_addr, sizeof(serv_addr));
            if (!result)
                result = recvfrom(sockfd, buffer, BUFLEN, 0, &serv_addr,
                                   sizeof(serv_addr));
        }
        gettimeofday(&tock, 0);
        elapsed = (tock.tv_sec - tick.tv_sec) * 1000 + (tock.tv_usec - tick.tv_usec) / 1000;

        if (result < 0) 
            fprintf(stderr, "Connection failed (time=%lums)\n", elapsed);
        else
            printf("Response from %s:%s (time=%lums)\n", argv[1], argv[2], elapsed);
#ifdef _WIN32
        closesocket(sockfd);
#else
        close(sockfd);
#endif
    } while ((argc >= 4 && !strcmp(argv[3], "-t")) ||
             (argc >= 5 && !strcmp(argv[4], "-t")));
    /* FIXME: Proper arg handling. */

    cleanup();
    return 0;
}

