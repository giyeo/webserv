#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cerrno>
#include <cstdio>

class SocketHandler {
	public:
		SocketHandler();
		SocketHandler(int port, int nqueue);
		~SocketHandler();

		void	screate();
		void	soptions();
		void	sbind();
		void	slisten();
		int		getFd();
	private:
		int fd;
		int port = 8080;
		//N connection requests will be queued before further requests are refused
		int nqueue = 8;
};

#endif