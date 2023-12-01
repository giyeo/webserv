#include "main.hpp"

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
				
				struct timeval timeout;
                timeout.tv_sec = 1; // 30 seconds timeout
                timeout.tv_usec = 0;

                if (setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
                    perror("Error setting socket timeout");
                    // Handle the error
                }

				fcntl(client_socket, F_SETFL, O_NONBLOCK);
				
                event.events = EPOLLIN | EPOLLET;
                event.data.fd = client_socket;
                if (epoll_ctl(epollFd, EPOLL_CTL_ADD, client_socket, &event) == -1) {
                    perror("epoll_ctl failed");
                    close(client_socket);
                }
			
			}
			// Communication with the client
			else {
				char buffer[65536];
				int bytes_received = recv(events[i].data.fd, buffer, sizeof(buffer), 0);
				if (bytes_received <= 0) {
					std::cout << bytes_received << "\n";
					if(errno == EAGAIN || errno == EWOULDBLOCK)
						std::cout << "HANGING\n";
					epoll_ctl(epollFd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
					close(events[i].data.fd);

				} else {
					buffer[bytes_received] = '\0';
					Request httpReq((const char *)&buffer);
					Resource resource(httpReq, events[i].data.fd);
					 //for test only
					//  sleep(2);
				}
			}
		}
	}
	close(epollFd);
}

void execute_mocked_requests() {
	std::string buffer = Resource::readFile("./_TestFiles/postWithFile");
	Request httpReq(buffer.c_str());
	// Resource resource(httpReq, 1);
}

int main(int argc, char **argv) {
	if(argc != 2) {
		std::cout << "Must have one argument only, example: ./server webserv.conf\n";
		exit(EXIT_FAILURE);
	}
	
	Configuration server_config(argv[1]);
	SocketHandler server_socket(server_config.getConfig());
	// Handle accept incoming requests
	//handle_request(server_socket.getFd()); //we are going to send the server_config as well, later..
	execute_mocked_requests();
	return (0);
}
