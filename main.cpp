#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cerrno>
#include <cstdio>
#include "SocketHandler.hpp"
#include "Configuration.hpp"

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
		char buffer[1024];
		int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
		if (bytes_received == -1) {
			perror("Receiving data failed");
		} else {
			// Handle received data
			// ...
			std::string string_buffer(buffer);
			// Send a response
			size_t newlinePos = string_buffer.find('\n');

			if (newlinePos != std::string::npos) {
				// Extract the first line from the buffer.
				std::string firstLine = string_buffer.substr(0, newlinePos);

				// Print the first line.
				std::cout << firstLine << std::endl;
			} else {
				// If no newline character is found, the entire buffer is considered as the first line.
				std::cout << "Buffer contains only one line: " << string_buffer << std::endl;
			}

			// std::cout << buffer << "\r\n";

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

int main(int argc, char **argv) {
	if(argc != 2) {
		std::cout << "Must have one argument only, example: ./server webserv.conf\n";
		exit(EXIT_FAILURE);
	}
	
	Configuration server_config(argv[1]);
	SocketHandler server_socket(server_config.getConfig());
	// Handle accept incoming requests
	handle_request(server_socket.getFd());
	return 0;
}
