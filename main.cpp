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
#include "Response.hpp"

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
			
			// std::cout << buffer << "\r\n";
			Response httpres(
				/*http_version,status_code,status_text*/
					"1.1","200","OK",
				/*content_type*/	
					"text/html",
				/*content_length*/
					"48",
				/*date*/
					"2023-09-20T00:31:02.612Z",
				/*server*/
					"webserv",
				/*cache_control*/
					"",
				/*set_cookie*/
					"",
				/*location*/
					"",
				/*connection*/
					"close",
				/*response_body*/
					"<html><body><h1>Hello, World!</h1></body></html>"
			);

			std::cout << httpres.toString() << "\n";
			
			int bytesSent = send(client_socket, httpres.toString().c_str(), httpres.toString().size(), 0);
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
	handle_request(server_socket.getFd()); //we are going to send the server_config as well, later..
	return (0);
}
