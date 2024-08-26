#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>


int main(){

    int server_fd;
    struct sockaddr_in server_addr,client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    char buffer[1024] = {0};
    const char* hello = "Hello from server";

    // Creating a socket
    server_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(server_fd < 0){
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);

    // Binding the socket
    if(bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    printf("Server is running on port 8080\n");

    // Receive data from client
    int n = recvfrom(server_fd,buffer,1024,0,(struct sockaddr*)&client_addr,&client_addr_len);
    if(n < 0){
        perror("recvfrom failed");
        exit(EXIT_FAILURE);
    }
    buffer[n] = '\0';
    printf("Client: %s\n",buffer);

    // Send data to client
    if(sendto(server_fd,hello,strlen(hello),0,(struct sockaddr*)&client_addr,client_addr_len)<0){
        perror("sendto failed");
        exit(EXIT_FAILURE);
    }
    printf("Hello message sent\n");

    //close socket
    close(server_fd);

    return 0;


}