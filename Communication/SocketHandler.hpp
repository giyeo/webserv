#ifndef SOCKETHANDLER_HPP
#define SOCKETHANDLER_HPP

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cerrno>
#include <cstdio>
#include "../Parser/Parser.hpp"
#include <fcntl.h>

class SocketHandler {
	public:
		SocketHandler(Server &server);
		~SocketHandler();
		
		void	socketCreate();
		void	socketOptions();
		void	socketBind();
		void	socketListen();
		int		getFd();
		Server &server;
	private:
		int fd;
		int port;
		//N connection requests will be queued before further requests are refused
		int nqueue;
};

#endif