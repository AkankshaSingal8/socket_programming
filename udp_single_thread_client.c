#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sock;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    const char *hello = "Hello from client";
    socklen_t addr_len = sizeof(serv_addr);

    // Create socket file descriptor
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("Socket creation error\n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    // Send message to server
    sendto(sock, hello, strlen(hello), 0, (struct sockaddr *)&serv_addr, addr_len);
    printf("Hello message sent\n");

    // Receive message from server
    int n = recvfrom(sock, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&serv_addr, &addr_len);
    buffer[n] = '\0';
    printf("Server: %s\n", buffer);

    // Close socket
    close(sock);

    return 0;
}
