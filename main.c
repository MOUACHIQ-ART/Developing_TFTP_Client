#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s host file\n", argv[0]);
        return 1;
    }
    char *host = argv[1];
    char *file = argv[2];
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;       // IPv4
    hints.ai_socktype = SOCK_DGRAM;  // UDP
    hints.ai_protocol = IPPROTO_UDP; // Protocole UDP

    //Call to getaddrinfo
    int status = getaddrinfo(host, "1069", &hints, &res);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        return 2;
    }
     // Display the resolved IP address
    if (res != NULL) {
        char ip_str[INET_ADDRSTRLEN];
        struct sockaddr_in *ipv4 = (struct sockaddr_in *)res->ai_addr;
        inet_ntop(AF_INET, &(ipv4->sin_addr), ip_str, INET_ADDRSTRLEN);
        printf(" Server Ip address : %s\n", ip_str);
    }
     // Socket creation 
    int sockfd = -1;
    if (res != NULL) {
        sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (sockfd == -1) {
            perror("Socket creation error");
            freeaddrinfo(res);
            return 3;
        }
        printf("UDP socket created . Descriptor : %d\n", sockfd);

    if (sockfd == -1) {
        fprintf(stderr, "Socket creation fail \n");
        freeaddrinfo(res);
        return 3;
    }


    }

     freeaddrinfo(res);
    printf("Host: %s, File: %s\n", host, file);
    return 0;
}
