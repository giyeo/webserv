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


int createClientSocket(int server_socket) {
	struct sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(client_addr);
	int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
	if (client_socket == -1) {
		perror("Accepting connection failed");
		return ; // Handle the error and continue accepting connections
	}
	
	struct timeval timeout;
	timeout.tv_sec = 1; // 30 seconds timeout
	timeout.tv_usec = 0;

	if (setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
		perror("Error setting socket timeout");
		// Handle the error
	}

	fcntl(client_socket, F_SETFL, O_NONBLOCK);
	return client_socket;
}

void serverEvent(int server_socket, int epollFd) {
	epoll_event event;

	int client_socket = createClientSocket(server_socket);
	
	event.events = EPOLLIN | EPOLLET;
	event.data.fd = client_socket;
	if (epoll_ctl(epollFd, EPOLL_CTL_ADD, client_socket, &event) == -1) {
		perror("epoll_ctl failed");
		close(client_socket);
	}
}

void clientEvent(int client_socket, int epollFd) {
	char buffer[8196];
	int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
	if (bytes_received <= 0) {
		std::cout << bytes_received << "\n";
		epoll_ctl(epollFd, EPOLL_CTL_DEL, client_socket, NULL);
		close(client_socket);
	} else {
		buffer[bytes_received] = '\0';
		Request httpReq((const char *)&buffer);
		Resource resource(httpReq, client_socket);
	}
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
            if (events[i].data.fd == server_socket)
				serverEvent(server_socket, epollFd);
			else
				clientEvent(events[i].data.fd, epollFd);
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
