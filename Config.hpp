
#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "Main.hpp"
#include "Communication/SocketHandler.hpp"
#include "Request/Request.hpp"

#define NONE -1
#define CLIENT 0
#define SERVER 1
#define SERVICE 2

typedef struct s_event {
	int type;
	int fd[3];
	std::string buffer;
	ssize_t bytes;
	ssize_t totalSent;
	ssize_t totalLeft;
	Request req;
	FILE *fp;
	s_event() : type(NONE), bytes(0), totalSent(0), totalLeft(0), fp(NULL) {}
}	t_event;

class Config {
	public:
		Config();
		void print();
		
		std::map<int, t_event> events;
		std::vector<SocketHandler> serverSockets;
		std::vector<Server> servers;

		//Redefined at Each request
		SocketHandler server;
		Request httpReq;
		int epollFd;
		int serverFd;
		int clientFd;
		
};

#endif