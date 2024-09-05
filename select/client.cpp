#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    std::string message;

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation error" << std::endl;
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        return -1;
    }

    // Connect to the server
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection Failed" << std::endl;
        return -1;
    }

    std::cout << "Connected to server. Enter a message: ";
    while (std::getline(std::cin, message)) {
        // Send message to the server
        send(sock, message.c_str(), message.length(), 0);
        std::cout << "Message sent\n";

        // Receive message from the server
        int valread = read(sock, buffer, BUFFER_SIZE);
        if (valread > 0) {
            buffer[valread] = '\0';
            std::cout << "Server replied: " << buffer << std::endl;
        }

        std::cout << "Enter a message: ";
    }

    close(sock);
    return 0;
}
