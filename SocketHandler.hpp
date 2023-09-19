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
#include "Configuration.hpp"

class SocketHandler {
	public:
		SocketHandler(Config &config);
		~SocketHandler();

		void	screate();
		void	soptions();
		void	sbind();
		void	slisten();
		int		getFd();
	private:
		int fd;
		int port;
		//N connection requests will be queued before further requests are refused
		int nqueue;
		Config &config;
};

#endif