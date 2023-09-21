#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cerrno>
#include <cstdio>
#include "Communication/SocketHandler.hpp"
#include "Configuration/Configuration.hpp"
#include "Response/Response.hpp"
#include "Request/Request.hpp"
#include <sys/epoll.h>

#define MAX_EVENTS 10

int create_epoll(int server_socket) {
	int epollFd;
	if ((epollFd = epoll_create1(0)) == -1) {
		perror("epoll_create1 failed");
		exit(EXIT_FAILURE);
	}

	epoll_event event;
	event.events = EPOLLIN;
	event.data.fd = server_socket;

	if (epoll_ctl(epollFd, EPOLL_CTL_ADD, server_socket, &event) == -1) {
		perror("epoll_ctl failed");
		close(epollFd);
		close(server_socket);
		exit(EXIT_FAILURE);
	}
	return epollFd;
}

void handle_request(int server_socket) {
	int epollFd;
	epoll_event events[MAX_EVENTS];
	epollFd = create_epoll(server_socket);

	while (true) {
		//Listening to incoming epoll events
		int numEvents = epoll_wait(epollFd, events, MAX_EVENTS, -1);
		if (numEvents == -1) {
			perror("epoll_wait failed");
			break;
		}

		for (int i = 0; i < numEvents; ++i) {
			// Accept incoming connections
            if (events[i].data.fd == server_socket) {
				epoll_event event;
				struct sockaddr_in client_addr;
				socklen_t client_addr_len = sizeof(client_addr);
				int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
				if (client_socket == -1) {
					perror("Accepting connection failed");
					continue; // Handle the error and continue accepting connections
				}

				event.events = EPOLLIN;
                event.data.fd = client_socket;
                if (epoll_ctl(epollFd, EPOLL_CTL_ADD, client_socket, &event) == -1) {
                    perror("epoll_ctl failed");
                    close(client_socket);
                }
			}
			// Communication with the client
			else {
				char buffer[1024];
				int bytes_received = recv(events[i].data.fd, buffer, sizeof(buffer), 0);
				if (bytes_received <= 0) {
					perror("Receiving data failed");
					epoll_ctl(epollFd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
					close(events[i].data.fd);
				} else {
					buffer[bytes_received] = '\0';

					Request httpReq((const char *)&buffer);
					std::cout << httpReq.getVerb() << " is the verb\n";
					
					//Process the Parsed Request and then, return the Response
					//
					Response httpRes(
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

					std::cout << httpRes.toString() << "\n";
					
					int bytesSent = send(events[i].data.fd, httpRes.toString().c_str(), httpRes.toString().size(), 0);
					if (bytesSent < 0) {
						std::cerr << "Error sending response to client" << std::endl;
					}
					sleep(2);
				}
			}
		}
	}
	close(epollFd);
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
