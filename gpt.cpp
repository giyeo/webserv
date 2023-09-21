#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cerrno>
#include <ctime>
#include <vector>

#include <sys/epoll.h>
#include <unistd.h>
#include <arpa/inet.h>

const int MAX_EVENTS = 10;
const int BUFFER_SIZE = 1024;

int main() {
    int serverSocket, epollFd;
    epoll_event events[MAX_EVENTS];
    char buffer[BUFFER_SIZE];
    
    // Create a socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    // Bind the socket
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Binding failed");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }
    
    // Listen for incoming connections
    if (listen(serverSocket, 5) == -1) {
        perror("Listening failed");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }
    
    std::cout << "Server listening on port 12345" << std::endl;

    // Create epoll instance
    epollFd = epoll_create1(0);
    if (epollFd == -1) {
        perror("epoll_create1 failed");
        exit(EXIT_FAILURE);
    }

    epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = serverSocket;

    // Add the server socket to the epoll set
    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, serverSocket, &event) == -1) {
        perror("epoll_ctl failed");
        close(epollFd);
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    while (true) {
        int numEvents = epoll_wait(epollFd, events, MAX_EVENTS, -1);
        if (numEvents == -1) {
            perror("epoll_wait failed");
            break;
        }

        for (int i = 0; i < numEvents; ++i) {
            if (events[i].data.fd == serverSocket) {
                // Accept a new connection
                struct sockaddr_in clientAddr;
                socklen_t clientAddrLen = sizeof(clientAddr);
                int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
                if (clientSocket == -1) {
                    perror("accept failed");
                    continue;
                }

                // Add the new client socket to the epoll set
                event.events = EPOLLIN;
                event.data.fd = clientSocket;
                if (epoll_ctl(epollFd, EPOLL_CTL_ADD, clientSocket, &event) == -1) {
                    perror("epoll_ctl failed");
                    close(clientSocket);
                }
            } else {
                // Handle data from a client
                int bytesRead = read(events[i].data.fd, buffer, BUFFER_SIZE);
                if (bytesRead <= 0) {
                    // Connection closed or error
                    epoll_ctl(epollFd, EPOLL_CTL_DEL, events[i].data.fd, nullptr);
                    close(events[i].data.fd);
                } else {
                    // Process the data received from the client
                    buffer[bytesRead] = '\0';
                    std::cout << "Received: " << buffer << std::endl;

					
                }
            }
        }
    }

    close(epollFd);
    close(serverSocket);

    return 0;
}
