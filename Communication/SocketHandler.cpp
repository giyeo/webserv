#include "SocketHandler.hpp"

SocketHandler::SocketHandler(Config &config): config(config) {
	this->port = 8080;
	this->nqueue = 8;

	socketCreate();
	socketOptions();
	socketBind();
	socketListen();
}

SocketHandler::~SocketHandler() {
	close(this->fd);
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
				SO_REUSEADDR | SO_REUSEPORT, &opt,
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
	server_addr.sin_addr.s_addr = INADDR_ANY; // Listen on all available network interfaces
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
	std::cout << "Server listening on port" << this->port << "..." << std::endl;
}

int SocketHandler::getFd() {
	return this->fd;
}