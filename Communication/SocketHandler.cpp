#include "SocketHandler.hpp"

SocketHandler::SocketHandler() {}

SocketHandler::SocketHandler(Server &server): server(server) {
	this->port = atoi(server.listen.c_str());
	this->nqueue = 8;

	socketCreate();
	socketOptions();
	socketBind();
	socketListen();
}

SocketHandler::~SocketHandler() {
	
};

void SocketHandler::socketCreate() {
	this->fd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->fd == -1) {
		perror("Socket creation failed");
		exit(EXIT_FAILURE);
	}
}

void SocketHandler::socketOptions() {
	int opt = 1;
	if (setsockopt(this->fd, SOL_SOCKET,
				SO_REUSEADDR | SO_REUSEPORT, &opt, //TODO remove when intra
				sizeof(opt))) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	fcntl(this->fd, F_SETFL, O_NONBLOCK);
}

void SocketHandler::socketBind() {
	struct sockaddr_in server_addr;

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = (server.address.empty()) ? INADDR_ANY : inet_addr(getAddressByName(server.address).c_str()); // Listen on all available network interfaces
	std::cout << server_addr.sin_addr.s_addr << std::endl;
	server_addr.sin_port = htons(this->port); // Port number (change as needed)
	if (bind(this->fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
		perror("Binding failed");
		close(this->fd);
		exit(EXIT_FAILURE);
	}
};

void SocketHandler::socketListen() {
	if (listen(this->fd, this->nqueue) == -1) {
		perror("Listening failed");
		close(this->fd);
		exit(EXIT_FAILURE);
	}
	std::cout << "Server listening on port " << this->port << "..." << std::endl;
}

int SocketHandler::getFd() {
	return this->fd;
}