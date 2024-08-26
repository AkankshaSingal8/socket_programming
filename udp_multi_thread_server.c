#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

struct ClientData {
    int socket;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len;
    char buffer[1024];
};

void* handle_client(void* client_data) {
    struct ClientData* data = (struct ClientData*)client_data;
    const char* hello = "Hello from server";

    // Process client request
    printf("Client: %s\n", data->buffer);

    sleep(5);
    // Send response to client
    sendto(data->socket, hello, strlen(hello), 0, (struct sockaddr*)&data->client_addr, data->client_addr_len);
    printf("Hello message sent\n");

    // Free allocated memory for client data
    free(data);
    pthread_exit(NULL);
}

int main() {
    int server_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    char buffer[1024];

    // Create socket
    server_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_fd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Bind socket to port 8080
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port 8080\n");

    while (1) {
        // Allocate memory for client data
        struct ClientData* data = (struct ClientData*)malloc(sizeof(struct ClientData));
        if (data == NULL) {
            perror("Memory allocation failed");
            continue;
        }
        
        data->socket = server_fd;
        data->client_addr_len = client_addr_len;

        // Receive data from client
        int n = recvfrom(server_fd, data->buffer, 1024, 0, (struct sockaddr*)&data->client_addr, &data->client_addr_len);
        if (n < 0) {
            perror("Receive failed");
            free(data);
            continue;
        }
        data->buffer[n] = '\0';

        // Create a new thread to handle the client
        pthread_t client_thread;
        if (pthread_create(&client_thread, NULL, handle_client, (void*)data) != 0) {
            perror("Could not create thread");
            free(data);
        }

        // Detach thread to clean up resources automatically when it finishes
        pthread_detach(client_thread);
    }

    close(server_fd);
    return 0;
}
