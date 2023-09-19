#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cerrno>
#include <cstdio>

#define PORT 8080
int socket_create() {
	int server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket == -1) {
		perror("Socket creation failed");
		exit(EXIT_FAILURE);
	}
	return server_socket;
}

void socket_options(int server_socket) {
	int opt = 1;
	if (setsockopt(server_socket, SOL_SOCKET,
				SO_REUSEADDR | SO_REUSEPORT, &opt,
				sizeof(opt))) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
}

void socket_bind(int server_socket) {
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY; // Listen on all available network interfaces
	server_addr.sin_port = htons(PORT);      // Port number (change as needed)

	if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
		perror("Binding failed");
		close(server_socket);
		exit(EXIT_FAILURE);
	}
}

void socket_listen(int server_socket) {
	if (listen(server_socket, 5) == -1) {
		perror("Listening failed");
		close(server_socket);
		exit(EXIT_FAILURE);
	}
	std::cout << "Server listening on port 8080..." << std::endl;
}

void handle_request(int server_socket) {
	while (true) {
	// Accept incoming connections
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_socket == -1) {
            perror("Accepting connection failed");
            continue; // Handle the error and continue accepting connections
        }

        // Communication with the client
        char buffer[2024];
        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received == -1) {
            perror("Receiving data failed");
        } else {
            // Handle received data
            // ...

            // Send a response
			std::cout << buffer << "\n";
			// Construct an HTTP response.
			std::string httpResponse = "HTTP/1.1 200 OK\r\n";
			httpResponse += "Content-Type: text/html\r\n"; // Set the content type
			httpResponse += "Connection: close\r\n"; // Close the connection after sending the response
			httpResponse += "\r\n"; // Empty line separates headers from the body
			httpResponse += "<html><body><h1>Hello, World!</h1></body></html>";
            int bytesSent = send(client_socket, httpResponse.c_str(), httpResponse.size(), 0);
			if (bytesSent < 0) {
        		std::cerr << "Error sending response to client" << std::endl;
    		}
        }
		
        // Close the client socket
        close(client_socket);
	}
}

int main() {
	// Create a socket
	int server_socket = socket_create();
	// Set Options to the socket
	socket_options(server_socket);
	// Bind the socket to a specific address and port
	socket_bind(server_socket);
	// Listen for incoming connections
	socket_listen(server_socket);
	// Handle accept incoming requests
	handle_request(server_socket);
    // Close the server socket
    close(server_socket);
    return 0;
}
