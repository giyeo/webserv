
#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "Main.hpp"
#include "Communication/SocketHandler.hpp"
#include "Request/Request.hpp"

typedef struct cgiInfo {
	int clientFd;
	bool isRead;
}	t_cgiInfo;

class Config {
	public:
		Config();
		void print();
		
		t_cgiInfo cgiInfo;
		std::map<int, Request> connectionHeaders;
		std::map<int, t_cgiInfo> cgiConnections;
		std::map<int, int> whoswho;
		std::vector<SocketHandler> serverSockets;
		std::vector<Server> servers;

		//Redefined at Each request
		SocketHandler server;
		Request httpReq;
		int epollFd;
		int serverFd;
		int clientFd;
		int serverSocketIndex;
		
};

#endif