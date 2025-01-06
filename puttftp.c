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
    hints.ai_protocol = IPPROTO_UDP; // UDP Protocol
        // Resolve server address using getaddrinfo
    int status = getaddrinfo(host, "1069", &hints, &res);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        return 2;
    }

    // Display resolved IP address
    if (res != NULL) {
        char ip_str[INET_ADDRSTRLEN];
        struct sockaddr_in *ipv4 = (struct sockaddr_in *)res->ai_addr;
        inet_ntop(AF_INET, &(ipv4->sin_addr), ip_str, INET_ADDRSTRLEN);
        printf("Server IP address: %s\n", ip_str);
    }

    // Create UDP socket
    int sockfd = -1;
    if (res != NULL) {
        sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (sockfd == -1) {
            perror("Socket creation error");
            freeaddrinfo(res);
            return 3;
        }
        printf("UDP socket created. Descriptor: %d\n", sockfd);
    }
     // Step a: Build and send WRQ (Write Request)
    printf("\n Step a: Building and sending WRQ\n");
    char request[MAX_BUFFER_SIZE];
    memset(request, 0, MAX_BUFFER_SIZE);

    request[0] = 0x00; // Opcode MSB
    request[1] = 0x02; // Opcode WRQ (Write Request)
    strcpy(&request[2], file); 
    strcpy(&request[strlen(file) + 3], "octet"); 
    request[strlen(file) + 3 + strlen("octet") + 1] = 0x00; 
    ssize_t sent_bytes = sendto(sockfd, request, strlen(file) + strlen("octet") + 4, 0, res->ai_addr, res->ai_addrlen);
    if (sent_bytes == -1) {
        perror("WRQ send error");
        freeaddrinfo(res);
        close(sockfd);
        return 4;
    }
     // Step b & c: Sending file data as TFTP DAT packets
    printf("\nStep b & c: Sending file data\n");
    FILE *input_file = fopen(file, "rb");
    if (!input_file) {
        perror("File open error");
        freeaddrinfo(res);
        close(sockfd);
        return 5;
    }
     struct sockaddr_storage server_addr;
    socklen_t addr_len = sizeof(server_addr);
    char buffer[MAX_BUFFER_SIZE];
    char ack[4];
    int block_num = 0;
    while (1) {
        // Read up to 512 bytes from the file
        size_t bytes_read = fread(buffer + 4, 1, 512, input_file);
        if (bytes_read < 0) {
            perror("File read error");
            fclose(input_file);
            freeaddrinfo(res);
            close(sockfd);
            return 6;
        }
      // Prepare DAT packet
        block_num++;
        buffer[0] = 0x00; // Opcode MSB
        buffer[1] = 0x03; // Opcode DAT (Data Packet)
        buffer[2] = (block_num >> 8) & 0xFF; // Block number MSB
        buffer[3] = block_num & 0xFF;        // Block number LSB
                // Send DAT packet
        ssize_t sent_data = sendto(sockfd, buffer, bytes_read + 4, 0, (struct sockaddr *)&server_addr, addr_len);
        if (sent_data == -1) {
            perror("Data send error");
            fclose(input_file);
            freeaddrinfo(res);
            close(sockfd);
            return 7;
        }
        printf("Block #%d sent ( bytes).\n", block_num, bytes_read);

        // Receive ACK
        ssize_t received_bytes = recvfrom(sockfd, ack, 4, 0, (struct sockaddr *)&server_addr, &addr_len);
        if (received_bytes == -1) {
            perror("ACK receive error");
            fclose(input_file);
            freeaddrinfo(res);
            close(sockfd);
            return 8;
        }
        // Check ACK
        uint16_t opcode = ntohs(*(uint16_t *)ack);
        uint16_t received_block = ntohs(*(uint16_t *)(ack + 2));

        if (opcode == 4 && received_block == block_num) {
            printf("ACK received for block #%d.\n", block_num);
        } else {
            fprintf(stderr, "Invalid ACK for block #%d.\n", block_num);
            fclose(input_file);
            freeaddrinfo(res);
            close(sockfd);
            return 9;
        }
        // Check ACK
        uint16_t opcode = ntohs(*(uint16_t *)ack);
        uint16_t received_block = ntohs(*(uint16_t *)(ack + 2));

        if (opcode == 4 && received_block == block_num) {
            printf("ACK received for block #%d.\n", block_num);
        } else {
            fprintf(stderr, " Invalid ACK for block #%d.\n", block_num);
            fclose(input_file);
            freeaddrinfo(res);
            close(sockfd);
            return 9;
        }
        // Check ACK
        uint16_t opcode = ntohs(*(uint16_t *)ack);
        uint16_t received_block = ntohs(*(uint16_t *)(ack + 2));

        if (opcode == 4 && received_block == block_num) {
            printf("ACK received for block #%d.\n", block_num);
        } else {
            fprintf(stderr, "Invalid ACK for block #%d.\n", block_num);
            fclose(input_file);
            freeaddrinfo(res);
                  close(sockfd);
            return 9;
        }

        // End of file
        if (bytes_read < 512) {
            printf("End of file reached. All data sent.\n");
            break;
        }
    }
    fclose(input_file);
    freeaddrinfo(res);
    close(sockfd);
    printf("File transfer complete, socket closed.\n");
    return 0;
}
