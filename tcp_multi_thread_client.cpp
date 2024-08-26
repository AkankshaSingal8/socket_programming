#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>

void client_task() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    const char *hello = "Hello from client";
    char buffer[1024] = {0};

    std::thread::id this_id = std::this_thread::get_id();

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Thread " << this_id << ": Socket creation error" << std::endl;
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8001);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cerr << "Thread " << this_id << ": Invalid address/Address not supported" << std::endl;
        return;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Thread " << this_id << ": Connection Failed" << std::endl;
        return;
    }

    send(sock, hello, strlen(hello), 0);
    std::cout << "Thread " << this_id << ": Hello message sent" << std::endl;
    read(sock, buffer, 1024);
    std::cout << "Thread " << this_id << ": Message received: " << buffer << std::endl;

    close(sock);
}

int main() {
    std::thread t1(client_task);
    std::thread t2(client_task);

    t1.join();
    t2.join();

    return 0;
}
