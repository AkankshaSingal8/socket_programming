#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define BUFFER_SIZE 1024

struct ClientThreadData {
    int thread_id;
    struct sockaddr_in serv_addr;
};

void* client_thread(void* arg) {
    struct ClientThreadData* data = (struct ClientThreadData*)arg;
    int sock;
    char buffer[BUFFER_SIZE] = {0};
    const char *hello = "Hello from client";
    socklen_t addr_len = sizeof(data->serv_addr);

    // Create socket file descriptor
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("Thread %d: Socket creation error\n", data->thread_id);
        pthread_exit(NULL);
    }

    // Send message to server
    sendto(sock, hello, strlen(hello), 0, (struct sockaddr *)&data->serv_addr, addr_len);
    printf("Thread %d: Hello message sent\n", data->thread_id);

    // Receive message from server
    int n = recvfrom(sock, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&data->serv_addr, &addr_len);
    buffer[n] = '\0';
    printf("Thread %d: Server: %s\n", data->thread_id, buffer);

    // Close socket
    close(sock);

    pthread_exit(NULL);
}

int main() {
    struct sockaddr_in serv_addr;
    pthread_t threads[5]; // Number of client threads
    struct ClientThreadData thread_data[5];

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    // Create multiple client threads
    for (int i = 0; i < 5; i++) {
        thread_data[i].thread_id = i;
        thread_data[i].serv_addr = serv_addr;

        if (pthread_create(&threads[i], NULL, client_thread, &thread_data[i]) != 0) {
            printf("Error creating thread %d\n", i);
        }
    }

    // Wait for all threads to finish
    for (int i = 0; i < 5; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
