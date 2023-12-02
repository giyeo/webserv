#include "main.hpp"

#include <dirent.h>  // For directory operations
#include <unistd.h>  // For getpid

std::map<int, int> whoswho;

void listFileDescriptors() {
    // Get the process ID
    pid_t pid = getpid();

    // Construct the path to the /proc directory
    char procPath[256];
    std::sprintf(procPath, "/proc/%d/fd", pid);

    // Open the /proc/<PID>/fd directory
    DIR* dir = opendir(procPath);
    if (dir == NULL) {
        perror("opendir failed");
        return;
    }

    // Read the contents of the directory
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] != '.') {
            // Print the file descriptor
            std::cout << "File Descriptor: " << entry->d_name << std::endl;
        }
    }

    // Close the directory
    closedir(dir);
}


int create_epoll(std::vector<SocketHandler> &serversSocket) {
	int epollFd = 0;

	if ((epollFd = epoll_create1(0)) == -1) {
		perror("epoll_create1 failed");
		exit(EXIT_FAILURE);
	}

	for(size_t i = 0; i < serversSocket.size(); i++) {
		int serverFd = serversSocket[i].getFd();
		epoll_event event;
		std::memset(&event, 0, sizeof(event));
		event.events = EPOLLIN;
		event.data.fd = serverFd;

		if (epoll_ctl(epollFd, EPOLL_CTL_ADD, serverFd, &event) == -1) {
			perror("epoll_ctl failed");
			close(epollFd);
			close(serverFd);
			exit(EXIT_FAILURE);
		}
	}

	return epollFd;
}


int createClientSocket(int server_socket) {
	struct sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(client_addr);
	int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
	if (client_socket == -1) {
		perror("Accepting connection failed");
		return -1; // Handle the error and continue accepting connections
	}
	
	struct timeval timeout;
	timeout.tv_sec = 1; // TODO not working properly 30 seconds timeout
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
	whoswho[client_socket] = server_socket;
	std::cout<<"Client Connected: " << client_socket << "\n";
}


static std::map<int, Request> connectionHeaders;

void clientEvent(int client_socket, int epollFd, SocketHandler serverSocket) {
	std::cout << "Client " << client_socket << " Request Arrived\n";
	ssize_t bytesRead;
	char buffer[8196];

	while ((bytesRead = recv(client_socket, buffer, sizeof(buffer), 0)) > 0) {
		buffer[bytesRead] = '\0';
		std::map<int, Request>::iterator it = connectionHeaders.find(client_socket);
		if(it == connectionHeaders.end()) {
			Request httpReq((const char *)&buffer);
			connectionHeaders[client_socket] = httpReq;
		}
		connectionHeaders[client_socket].requestBodyBuffer.append(buffer);
		connectionHeaders[client_socket].totalBytesRead += bytesRead;
	}

	if (bytesRead == 0) {
		std::cout << "Connection closed by client" << std::endl;
		epoll_ctl(epollFd, EPOLL_CTL_DEL, client_socket, NULL);
		close(client_socket);
	} else if (bytesRead == -1 && errno != EWOULDBLOCK && errno != EAGAIN) {
		perror("Error receiving data");
		epoll_ctl(epollFd, EPOLL_CTL_DEL, client_socket, NULL);
		close(client_socket);
	}

	if (connectionHeaders[client_socket].totalBytesRead > 0) {
		std::cout << "Received data: " << connectionHeaders[client_socket].totalBytesRead << " bytes" << std::endl;
		
		if(connectionHeaders[client_socket].getContentLength() == connectionHeaders[client_socket].totalBytesRead - connectionHeaders[client_socket].getHeadersLength()) {
			// std::cout << "contentlength: "<< connectionHeaders[client_socket].getContentLength() << "\n";
			// std::cout << "totalnow: "<< connectionHeaders[client_socket].totalBytesRead << "\n";
			// std::cout << "headerslength: " << connectionHeaders[client_socket].getHeadersLength() << "\n";
			// std::cout << connectionHeaders[client_socket].requestBodyBuffer << '\n';
			connectionHeaders[client_socket].parseRequestBody();
			Resource resource(connectionHeaders[client_socket], client_socket, serverSocket);
			connectionHeaders.erase(client_socket);
			close(client_socket);
		}
	}
}

SocketHandler &getServerByFd(int fd, std::vector<SocketHandler> serverSockets) {
	for(size_t i = 0; i < serverSockets.size(); i++) {
		if (fd == serverSockets[i].getFd())
			return serverSockets[i];
	}
	std::cout << "TODO DEU TUDO ERRADO\n";
	exit(1);
}

bool eventFdIsServerSocket(int fd, std::vector<SocketHandler> serversSockets) {
	for(size_t i = 0; i < serversSockets.size(); i++)
		if(fd == serversSockets[i].getFd())
			return true;
	return false;
}

void createEventPoll(std::vector<SocketHandler> &serversSockets) {
	int epollFd = create_epoll(serversSockets);
	epoll_event events[MAX_EVENTS];
	while (true) {
		//Listening to incoming epoll events
		int numEvents = epoll_wait(epollFd, events, MAX_EVENTS, -1);
		if (numEvents == -1) {
			perror("epoll_wait failed");
			break;
		}

		for (int i = 0; i < numEvents; ++i) {
			if (eventFdIsServerSocket(events[i].data.fd, serversSockets))
				serverEvent(events[i].data.fd, epollFd);
			else
				clientEvent(events[i].data.fd, epollFd,
					getServerByFd(whoswho[events[i].data.fd], serversSockets));
		}
	}
	close(epollFd);
}

int main(int argc, char **argv) {
	if(argc != 2) {
		std::cout << "Must have one argument only, example: ./server webserv.conf\n";
		exit(EXIT_FAILURE);
	}
	
	std::vector<Server> servers = configurationParser(argv[1]);
	std::vector<SocketHandler> serverSockets;
	for(size_t i = 0; i < servers.size(); i++) {
		SocketHandler server_socket(servers[i]);
		serverSockets.push_back(server_socket);
	}

	// Handle accept incoming requests
	createEventPoll(serverSockets); //we are going to send the server_config as well, later..
	return (0);
}
