#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>

#define USAGE_MSG "Usage: %s <host> <file>\n"
#define ADDRESS_OBTAINED_MSG "Address obtained for host.\n"
#define SOCKET_CREATED_MSG "Socket created successfully.\n"
#define SOCKET_ERROR_MSG "Error creating socket.\n"


int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s host file\n", argv[0]);
        return 1;
    }
    char *host = argv[1];
    char *file = argv[2];
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    int status = getaddrinfo(host, "1069", &hints, &res);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        return 2;
    }
    
    int sockfd = -1;

    if (res != NULL) {
        sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (sockfd == -1) {
            perror("socket"); 
            sockfd = 0;
        }
    }

    if (sockfd == -1) {
        fprintf(stderr, "Failed to create socket\n");
        return 3;
    }

    freeaddrinfo(res);
    printf("Host: %s, File: %s\n", host, file);
    return 0;
}
