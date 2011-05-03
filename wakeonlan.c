/* 
 * Wake-on-LAN is a protocol that allows you to bootup a computer from a
 * powered off state by sending it a "magic packet". This program allows you
 * to send magic packets to a given host.
 *
 * FIXME: The mac_to_bytes function was hastily written and doesn't work.
 * TODO: Better commenting.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>

/* A magic packet leads with 6 bytes of 0xff. */
#define PREFIX_DATA ((unsigned char)0xff)
#define PREFIX_LEN  6

/* Following these 6 bytes is the destination's MAC address repeated 16
 * times.
 */
#define MAC_REPETITIONS 16

/* Technically you can send a magic packet to any port, but 7 (IANA-defined
 * echo) and 9 (IANA-defined discard) are traditionally used to avoid
 * creating unnecessary noise for other services.
 */
#define DESTINATION_PORT 9

/* A MAC address should be xx:xx:xx:xx:xx:xx where x are hex values. */
#define MAC_CHARACTERS 12
#define MAC_BYTES       6

/* The length of the magic packet payload. */
#define BUFFER_LEN (PREFIX_LEN + MAC_BYTES * MAC_REPETITIONS)

unsigned char char_to_hex(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return 0;
}

void mac_to_bytes(unsigned char* out, char* mac) {
    int i;

    for (i = 0; i < MAC_BYTES; ++i, ++mac) {
        if (*mac != '\0')
            *(out + i) = char_to_hex(*mac++) << 4;
        if (*mac != '\0')
            *(out + i) |= char_to_hex(*mac++);
        if (*mac != '\0' && *mac != ':')
            return;
    }
}

int main(int argc, char** argv) {
    unsigned char mac[MAC_BYTES];
    unsigned char buffer[BUFFER_LEN];
    int i;
    int sockfd;
    struct sockaddr_in dest;
    struct hostent* dest_addr;

    if (argc < 3) {
        fprintf(stderr, "Usage: %s broadcast_address MAC_address\n", *argv);
        return 1;
    }

    /* Convert the textual MAC address to a byte array. */
    mac_to_bytes(mac, argv[2]);
    for (i = 0; i < MAC_BYTES * 2; ++i)
        printf("%x\n", i % 2 ? mac[i] >> 4 : mac[i] & 0xf);

    /* Construct the payload of the magic packet. */
    memset(buffer, PREFIX_DATA, PREFIX_LEN);
    for (i = 0; i < MAC_REPETITIONS; ++i)
        memcpy(buffer + PREFIX_LEN + (i * MAC_BYTES), mac, MAC_BYTES);

    /* Create a UDP socket. */
    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd < 0) {
        fprintf(stderr, "Error opening socket.\n");
        return 1;
    }

    /* Set the socket to broadcast mode. It is possible we don't have
     * permission to do this.
     */
    if (!setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, (void*)1, 0)) {
        fprintf(stderr, "Failed to set socket to broadcast mode.\n");
        return 1;
    }

    /* Lookup the destination. */
    dest_addr = gethostbyname(argv[1]);
    if (!dest_addr) {
        fprintf(stderr, "Could not lookup host %s.\n", argv[1]);
        return 1;
    }

    /* Setup the destination structure. */
    memset(&dest, 0, sizeof(dest));
    dest.sin_family = AF_INET;
    memcpy(&dest.sin_addr.s_addr, dest_addr->h_addr, dest_addr->h_length);
    dest.sin_port = htons(DESTINATION_PORT);

    /* We're ready to send the packet. Note that there will be no response
     * regardless of whether the destination received the packet.
     */
    sendto(sockfd, buffer, BUFFER_LEN, 0, &dest, sizeof(dest));

    close(sockfd);
    return 0;
}











