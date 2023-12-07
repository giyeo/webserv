#include "Master.hpp"
#include <algorithm>

void sendResponse(Config &config) {
	t_event &event = config.events[config.clientFd];
	int clientFd = config.clientFd;
	
	std::string responseString = event.buffer;
	const char* buffer = responseString.c_str();

	int bytesSent = 0;
	while (event.totalSent < event.bytes) {
		ssize_t maxChunkSize = std::min((ssize_t)4096, event.bytes - event.totalSent);
		if(event.type == SERVICE)
			bytesSent = fwrite(buffer + event.totalSent, 1, maxChunkSize, config.events[clientFd].fp);
		else
			bytesSent = send(config.clientFd, buffer + event.totalSent, maxChunkSize, 0);
		if (bytesSent < 0) {
			log(__FILE__, __LINE__, "Waiting to write", LOG);
			break;
		}
		event.totalSent += bytesSent;
		log(__FILE__, __LINE__, concat(4,"bytesSent: ", intToString(event.totalSent).c_str(), "/", intToString(event.bytes).c_str()), LOG);
		if (event.totalSent == event.bytes) {
			if(event.type == SERVICE)
				pclose(config.events[clientFd].fp);
			log(__FILE__, __LINE__, "Success on Sending to the client, Closing FD, Erasing event, removing from epoll", LOG);
			epoll_ctl(config.epollFd, EPOLL_CTL_DEL, config.clientFd, NULL);
			config.events.erase(config.clientFd);
			if(event.type != SERVICE)
				close(config.clientFd);
		}
		if (event.totalSent >= 16000) {
			log(__FILE__, __LINE__, "Exiting the function early to re-enter epoll", LOG);
			break;
		}
	}
	std::cout << "■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n\n\n\n";
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
	timeout.tv_sec = 30; // TODO maybe working?
	timeout.tv_usec = 0;

	if (setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
		log(__FILE__,__LINE__,"Error setting socket timeout", WARNING);
	}

	fcntl(client_socket, F_SETFL, O_NONBLOCK);
	return client_socket;
}

void serverEvent(Config &config) {
	std::cout << "■■■■■■■■■■■■■■■■■ " << __TIMESTAMP__ << " ■■■■■■■■■■■■■■■■■\n";
	log(__FILE__, __LINE__, concat(2,"Server event Arrived, fd: ",intToString(config.serverFd).c_str()), LOGBLUE);
	epoll_event event;

	int clientFd = createClientSocket(config.serverFd);

	event.events = EPOLLIN | EPOLLET;
	event.data.fd = clientFd;
	if (epoll_ctl(config.epollFd, EPOLL_CTL_ADD, clientFd, &event) == -1)
		log(__FILE__,__LINE__,"epoll_ctl failed", ERROR);
	config.events[clientFd].fd[SERVER] = config.serverFd;
	log(__FILE__,__LINE__,concat(2, "Client Connected, fd:", intToString(clientFd).c_str()), LOG);
	std::cout << "■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n\n\n\n";
}

void clientForwarding(Config &config) {
	SocketHandler serverSocket = config.server;
	int clientFd = config.clientFd;
	log(__FILE__, __LINE__, concat(3, "Received data: ", intToString(config.events[clientFd].bytes).c_str(), " bytes"), WARNING);
	unsigned long sendedRequestedBodySize = config.events[clientFd].bytes - config.events[clientFd].req.getHeadersLength();
	if(config.events[clientFd].req.getContentLength() == sendedRequestedBodySize) {
		log(__FILE__, __LINE__, "Data fully Received", LOG);
		std::string requestBuffer = config.events[clientFd].buffer.substr(config.events[clientFd].req.getHeadersLength() , config.events[clientFd].buffer.size());
		config.events[clientFd].req.requestBodyBuffer = requestBuffer;
		if(config.events[clientFd].req.parseRequestBody(clientFd, serverSocket.server.serverName[0]))
			Resource res(config);
	}
}

void clientEvent(Config &config) {
	int clientFd = config.clientFd;
	SocketHandler serverSocket = config.server;
	char buffer[8196];
	ssize_t bytesRead;

	while ((bytesRead = recv(clientFd, buffer, sizeof(buffer), 0)) > 0) {
		buffer[bytesRead] = '\0';
		if(config.events[clientFd].req.getMethod().empty()) {
			Request httpReq((const char *)&buffer, atoi(serverSocket.server.clientMaxBodySize.c_str()));
			config.events[clientFd].req = httpReq;
			config.httpReq = httpReq;
		}
		config.events[clientFd].buffer.append(buffer);
		config.events[clientFd].bytes += bytesRead;
	}

	if (bytesRead == 0) {
		log(__FILE__,__LINE__,"Connection closed by client", LOG);
		epoll_ctl(config.epollFd, EPOLL_CTL_DEL, clientFd, NULL);
		config.events.erase(clientFd);
		close(clientFd);
	}

	if (config.events[clientFd].bytes > 0)
		clientForwarding(config);
	
	std::cout << "■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n\n\n\n";
}

SocketHandler getServerByFd(int fd, std::vector<SocketHandler> serverSockets) {
	for(size_t i = 0; i < serverSockets.size(); i++) {
		if (fd == serverSockets[i].getFd())
			return serverSockets[i];
	}
	log(__FILE__, __LINE__, "Socket not Found in Servers Socket list given Socket Id", ERROR);
	exit(1);
}

bool eventFdIsServerSocket(int fd, std::vector<SocketHandler> serversSockets) {
	for(size_t i = 0; i < serversSockets.size(); i++)
		if(fd == serversSockets[i].getFd())
			return true;
	return false;
}

void buildEvent(Config &config, int eventFd) {
	if (eventFdIsServerSocket(eventFd, config.serverSockets)) {
		config.events[eventFd].type = SERVER;
		config.events[eventFd].fd[SERVER] = eventFd;
	} 
	else {
		config.events[eventFd].type = CLIENT;
		config.events[eventFd].fd[CLIENT] = eventFd;
	} 
	if  (config.events[eventFd].fp != NULL) {
		config.events[eventFd].type = SERVICE;
		config.events[eventFd].fd[SERVICE] = eventFd;
	}
}

void createEventPoll(Config &config) {
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
			int eventFd = events[i].data.fd;
			buildEvent(config, eventFd);
			int type = config.events[eventFd].type;
			if (type == SERVER) {
				config.serverFd = eventFd;
				serverEvent(config);
			}
			if (type == CLIENT) {
				std::cout << "■■■■■■■■■■■■■■■■■ " << __TIMESTAMP__ << " ■■■■■■■■■■■■■■■■■\n";	
				log(__FILE__, __LINE__, concat(2,"Client event Arrived, fd: ",intToString(eventFd).c_str()) , LOGBLUE);

				config.clientFd = eventFd;
				if(events[i].events & EPOLLIN) {
					log(__FILE__, __LINE__, "Reading from ClientFd", WARNING);
					config.serverFd = config.events[eventFd].fd[SERVER];
					config.server = getServerByFd(config.serverFd, config.serverSockets);
					clientEvent(config);
				}
				if(events[i].events & EPOLLOUT) {
					log(__FILE__, __LINE__, "Sending on ClientFd", WARNING);
					sendResponse(config);
				}
			}
			if (type == SERVICE) {
				if(events[i].events & EPOLLOUT) {
					log(__FILE__, __LINE__, "Sending on ServiceFd", WARNING);
					sendResponse(config);
				}
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
	Config config;
	
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
