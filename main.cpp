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

	if ((epollFd = epoll_create1(0)) == -1)
		log(__FILE__,__LINE__,"epoll_create1 failed", ERROR);

	for(size_t i = 0; i < serversSocket.size(); i++) {
		int serverFd = serversSocket[i].getFd();
		epoll_event event;
		std::memset(&event, 0, sizeof(event));
		event.events = EPOLLIN;
		event.data.fd = serverFd;

		if (epoll_ctl(epollFd, EPOLL_CTL_ADD, serverFd, &event) == -1) {
			close(epollFd);
			close(serverFd);
			log(__FILE__,__LINE__,"epoll_ctl failed", ERROR);
		}
	}

	return epollFd;
}


int createClientSocket(int server_socket) {
	struct sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(client_addr);
	int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
	if (client_socket == -1) {
		log(__FILE__,__LINE__,"client connection accept failed", WARNING);
		return -1; // Handle the error and continue accepting connections
	}
	
	struct timeval timeout;
	timeout.tv_sec = 1; // TODO not working properly 30 seconds timeout
	timeout.tv_usec = 0;

	if (setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
		log(__FILE__,__LINE__,"Error setting socket timeout", WARNING);
	}

	fcntl(client_socket, F_SETFL, O_NONBLOCK);
	return client_socket;
}

void serverEvent(int server_socket, int epollFd) {
	epoll_event event;

	int client_socket = createClientSocket(server_socket);
	
	event.events = EPOLLIN | EPOLLET;
	event.data.fd = client_socket;
	if (epoll_ctl(epollFd, EPOLL_CTL_ADD, client_socket, &event) == -1)
		log(__FILE__,__LINE__,"epoll_ctl failed", ERROR);
	whoswho[client_socket] = server_socket;
	log(__FILE__,__LINE__,"Server Client Connected", LOG);
}


static std::map<int, Request> connectionHeaders;

void clientEvent(int client_socket, int epollFd, SocketHandler serverSocket) {
	log(__FILE__, __LINE__, concat(2, "Client ", "Request Arrived"), LOGBLUE);
	ssize_t bytesRead;
	char buffer[8196];

	while ((bytesRead = recv(client_socket, buffer, sizeof(buffer), 0)) > 0) {
		buffer[bytesRead] = '\0';
		std::map<int, Request>::iterator it = connectionHeaders.find(client_socket);
		if(it == connectionHeaders.end()) {
			Request httpReq((const char *)&buffer, atoi(serverSocket.server.clientMaxBodySize.c_str()));
			connectionHeaders[client_socket] = httpReq;
		}
		connectionHeaders[client_socket].requestBodyBuffer.append(buffer);
		connectionHeaders[client_socket].totalBytesRead += bytesRead;
	}

	if (bytesRead == 0) {
		log(__FILE__,__LINE__,"Connection closed by client", LOG);
		epoll_ctl(epollFd, EPOLL_CTL_DEL, client_socket, NULL);
		connectionHeaders.erase(client_socket);
		close(client_socket);
	} else if (bytesRead == -1 && errno != EWOULDBLOCK && errno != EAGAIN) {
		log(__FILE__,__LINE__,"Waiting for socket to be Readable", WARNING);
		epoll_ctl(epollFd, EPOLL_CTL_DEL, client_socket, NULL);
		connectionHeaders.erase(client_socket);
		close(client_socket);
	}

	if (connectionHeaders[client_socket].totalBytesRead > 0) {
		log(__FILE__, __LINE__, concat(3, "Received data: ", intToString(connectionHeaders[client_socket].totalBytesRead).c_str(), " bytes"), WARNING);
		// std::cout << connectionHeaders[client_socket].requestBodyBuffer << '\n';
		unsigned long sendedRequestedBodySize = connectionHeaders[client_socket].totalBytesRead - connectionHeaders[client_socket].getHeadersLength();
		if(connectionHeaders[client_socket].getContentLength() == sendedRequestedBodySize) {
			if(connectionHeaders[client_socket].parseRequestBody(client_socket, serverSocket.server.serverName[0]))
				Resource resource(connectionHeaders[client_socket], client_socket, serverSocket);
			epoll_ctl(epollFd, EPOLL_CTL_DEL, client_socket, NULL);
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
	log(__FILE__,__LINE__,"Epoll Created", LOG);
	while (true) {
		//Listening to incoming epoll events
		int numEvents = epoll_wait(epollFd, events, MAX_EVENTS, -1);
		if (numEvents == -1) {
			close(epollFd);
			log(__FILE__,__LINE__,"Epoll Failed", ERROR);
		}

		for (int i = 0; i < numEvents; ++i) {
			if (eventFdIsServerSocket(events[i].data.fd, serversSockets)) {
				log(__FILE__,__LINE__,"Server Event", LOG);
				serverEvent(events[i].data.fd, epollFd);
			}
			else {
				log(__FILE__,__LINE__,"Client Event", LOG);
				clientEvent(events[i].data.fd, epollFd,
					getServerByFd(whoswho[events[i].data.fd], serversSockets));
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
	
	log(__FILE__,__LINE__,"Server Started", LOG);
	std::vector<Server> servers = configurationParser(argv[1]);
	log(__FILE__,__LINE__,"Configuration Parsed", LOG);
	std::vector<SocketHandler> serverSockets;
	for(size_t i = 0; i < servers.size(); i++) {
		SocketHandler server_socket(servers[i]);
		serverSockets.push_back(server_socket);
	}
	log(__FILE__,__LINE__,"Sockets Created", LOG);
	// Handle accept incoming requests
	createEventPoll(serverSockets); //we are going to send the server_config as well, later..
	return (0);
}
