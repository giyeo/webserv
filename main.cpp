#include "Master.hpp"

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

void serverEvent(t_config &config) {
	std::cout << "■■■■■■■■■■■■■■■■■ " << __TIMESTAMP__ << " ■■■■■■■■■■■■■■■■■\n";
	log(__FILE__, __LINE__, concat(2,"Server event Arrived, fd: ",intToString(config.serverFd).c_str()), LOGBLUE);
	epoll_event event;

	int client_socket = createClientSocket(config.serverFd);

	event.events = EPOLLIN | EPOLLET;
	event.data.fd = client_socket;
	if (epoll_ctl(config.epollFd, EPOLL_CTL_ADD, client_socket, &event) == -1)
		log(__FILE__,__LINE__,"epoll_ctl failed", ERROR);
	config.whoswho[client_socket] = config.serverFd;
	log(__FILE__,__LINE__,concat(2, "Client Connected, fd:", intToString(client_socket).c_str()), LOG);
	std::cout << "■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n\n\n\n";
}

void clientEvent(t_config &config) {
	std::cout << "■■■■■■■■■■■■■■■■■ " << __TIMESTAMP__ << " ■■■■■■■■■■■■■■■■■\n";	log(__FILE__, __LINE__, concat(2,"Client event Arrived, fd: ",intToString(config.serverFd).c_str()) , LOGBLUE);
	ssize_t bytesRead;
	SocketHandler serverSocket = config.serverSockets[config.serverSocketIndex];
	char buffer[8196];

	while ((bytesRead = recv(config.clientFd, buffer, sizeof(buffer), 0)) > 0) {
		buffer[bytesRead] = '\0';
		std::map<int, Request>::iterator it = config.connectionHeaders.find(config.clientFd);
		if(it == config.connectionHeaders.end()) {
			Request httpReq((const char *)&buffer, atoi(serverSocket.server.clientMaxBodySize.c_str()));
			config.connectionHeaders[config.clientFd] = httpReq;
			config.httpReq = httpReq;
		}
		config.connectionHeaders[config.clientFd].requestBodyBuffer.append(buffer);
		config.connectionHeaders[config.clientFd].totalBytesRead += bytesRead;
	}

	if (bytesRead == 0) {
		log(__FILE__,__LINE__,"Connection closed by client", LOG);
		epoll_ctl(config.epollFd, EPOLL_CTL_DEL, config.clientFd, NULL);
		config.connectionHeaders.erase(config.clientFd);
		close(config.clientFd);
	}

	if (config.connectionHeaders[config.clientFd].totalBytesRead > 0) {
		log(__FILE__, __LINE__, concat(3, "Received data: ", intToString(config.connectionHeaders[config.clientFd].totalBytesRead).c_str(), " bytes"), WARNING);

		unsigned long sendedRequestedBodySize = config.connectionHeaders[config.clientFd].totalBytesRead - config.connectionHeaders[config.clientFd].getHeadersLength();
		if(config.connectionHeaders[config.clientFd].getContentLength() == sendedRequestedBodySize) {
			log(__FILE__, __LINE__, "Data fully Received", LOG);
			if(config.connectionHeaders[config.clientFd].parseRequestBody(config.clientFd, serverSocket.server.serverName[0]))
				Resource res(config);
			epoll_ctl(config.epollFd, EPOLL_CTL_DEL, config.clientFd, NULL);
			config.connectionHeaders.erase(config.clientFd);
			close(config.clientFd);
		}
	}
	std::cout << "■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n\n\n\n";
}

int getServerByFd(int fd, std::vector<SocketHandler> serverSockets) {
	for(size_t i = 0; i < serverSockets.size(); i++) {
		if (fd == serverSockets[i].getFd())
			return i;
	}
	log(__FILE__, __LINE__, "Vacilamo pesado pq era pra achar", ERROR);
	return 0;
}

bool eventFdIsServerSocket(int fd, std::vector<SocketHandler> serversSockets) {
	for(size_t i = 0; i < serversSockets.size(); i++)
		if(fd == serversSockets[i].getFd())
			return true;
	return false;
}

void createEventPoll(t_config &config) {
	config.epollFd = create_epoll(config.serverSockets);
	epoll_event events[MAX_EVENTS];
	log(__FILE__,__LINE__,"Epoll Created\n\n\n", LOG);
	while (true) {
		//Listening to incoming epoll events
		int numEvents = epoll_wait(config.epollFd, events, MAX_EVENTS, -1);
		if (numEvents == -1) {
			close(config.epollFd);
			log(__FILE__,__LINE__,"Epoll Failed", ERROR);
		}

		for (int i = 0; i < numEvents; ++i) {
			config.clientFd = events[i].data.fd;
			if (eventFdIsServerSocket(events[i].data.fd, config.serverSockets)) {
				serverEvent(config);
			}
			else {
				config.serverFd = config.whoswho[config.clientFd];
				config.serverSocketIndex = getServerByFd(config.clientFd, config.serverSockets);
				clientEvent(config);
			}
		}
	}
	close(config.epollFd);
}

void verifyServers(std::vector<Server> &servers) {
	for(size_t i = 0; i < servers.size(); i++) {
		if(servers[i].listen.empty())
			log(__FILE__, __LINE__, "Server must have listen directive", ERROR);
		if(servers[i].root.empty())
			log(__FILE__, __LINE__, "Server must have root directive", ERROR);
	}
}

int main(int argc, char **argv) {
	if(argc != 2) {
		std::cout << "Must have one argument only, example: ./server webserv.conf\n";
		exit(EXIT_FAILURE);
	}
	t_config config;
	
	log(__FILE__,__LINE__,"Server Started", LOG);
	config.servers = configurationParser(argv[1]);
	log(__FILE__,__LINE__,"Configuration Parsed", LOG);
	verifyServers(config.servers);
	for(size_t i = 0; i < config.servers.size(); i++) {
		SocketHandler server_socket(config.servers[i]);
		config.serverSockets.push_back(server_socket);
	}
	log(__FILE__,__LINE__,"Sockets Created", LOG);
	// Handle accept incoming requests
	createEventPoll(config); //we are going to send the server_config as well, later..
	return (0);
}
