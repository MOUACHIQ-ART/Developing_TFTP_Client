#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>

#define MAX_BUFFER_SIZE 516

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


    }
    // Step 4a: Build and Send an RRQ (Read Request)
    
    char request[MAX_BUFFER_SIZE];
    memset(request, 0, MAX_BUFFER_SIZE);

    request[0] = 0x00; // Opcode MSB
    request[1] = 0x01; // Opcode RRQ (Read Request)
    strcpy(&request[2], file); 
    strcpy(&request[strlen(file) + 3], "octet"); 
    request[strlen(file) + 3 + strlen("octet") + 1] = 0x00; 
    ssize_t sent_bytes = sendto(sockfd, request, strlen(file) + strlen("octet") + 4, 0, res->ai_addr, res->ai_addrlen);
    if (sent_bytes == -1) {
        perror(" RRQ send error");
        freeaddrinfo(res);
        close(sockfd);
        return 4;
    }
    printf("RRQ sent successfully for file: %s\n", file);
    // Step 4b and 4c: Receive Data and Send ACK
  
    FILE *received_file = fopen(file, "wb");
    if (!received_file) {
        perror("File creation error");
        freeaddrinfo(res);
        close(sockfd);
        return 5;
    }
    struct sockaddr_storage server_addr;
    socklen_t addr_len = sizeof(server_addr);
    char buffer[MAX_BUFFER_SIZE];
    int block_num = 1;

    while (1) {
        ssize_t received_bytes = recvfrom(sockfd, buffer, MAX_BUFFER_SIZE, 0, (struct sockaddr *)&server_addr, &addr_len);
        if (received_bytes == -1) {
            perror("Data receive error");
            fclose(received_file);
            close(sockfd);
            freeaddrinfo(res);
            return 6;
        }
     // Check the packet type
        uint16_t opcode = ntohs(*(uint16_t *)buffer);
        uint16_t received_block = ntohs(*(uint16_t *)(buffer + 2));

        if (opcode == 3) { // DAT 
            if (received_block == block_num) {
                fwrite(buffer + 4, 1, received_bytes - 4, received_file);
                printf("Block #%d received and written to file.\n", block_num);
     // Send ACK
                char ack[4];
                ack[0] = 0x00;
                ack[1] = 0x04; // Opcode ACK
                ack[2] = (block_num >> 8) & 0xFF;
                ack[3] = block_num & 0xFF;
                sendto(sockfd, ack, 4, 0, (struct sockaddr *)&server_addr, addr_len);
                printf("ACK sent for block #%d.\n", block_num);

    // Last packet
                if (received_bytes < MAX_BUFFER_SIZE) {
                    printf(" End of file reached.\n");
                    break;
                }
                block_num++;
            }
        }else if (opcode == 5) { 
                fprintf(stderr, "Server error: %s\n", buffer + 4);
                break;
        }
    }
     fclose(received_file);
    freeaddrinfo(res);
    close(sockfd);
    printf("File transfer complete, socket closed.\n");
    freeaddrinfo(res);
    printf("Host: %s, File: %s\n", host, file);
    return 0;

}
