/* Creates a datagram server.  The port
   number is passed as an argument.  This
   server runs forever */

#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <iostream>

#define DEFAULT_PORT 9090

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[]) {
    int sock, n;
    unsigned length;
    struct sockaddr_in server, from;
    struct hostent *hp;
    char buffer[256];

    if (argc != 2) {
        printf("Usage: server port\n");
        exit(1);
    }
    sock= socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
        error("socket");

    server.sin_family = AF_INET;
    hp = gethostbyname(argv[1]);
    if (hp == nullptr)
        error("Unknown host");

    bcopy(hp->h_addr, (char *)&server.sin_addr, static_cast<size_t>(hp->h_length));
    server.sin_port = htons(DEFAULT_PORT);
    length=sizeof(struct sockaddr_in);
    printf("Please enter the message: ");
    bzero(buffer, 256);
    fgets(buffer,255, stdin);
    n= static_cast<int>(sendto(sock, buffer, strlen(buffer), 0, (const struct sockaddr *)& server, length));
    if (n < 0)
        error("Sendto");
    n = static_cast<int>(recvfrom(sock, buffer, 256, 0, (struct sockaddr *)& from, &length));
    if (n < 0)
        error("recvfrom");
    write(1, "Got an ack: ", 12);
    write(1, buffer, static_cast<size_t>(n));
    close(sock);
    return 0;
}