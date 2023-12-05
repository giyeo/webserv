#pragma once

#include "../Master.hpp"

class SocketHandler {
	public:
		SocketHandler();
		SocketHandler(Server &server);
		~SocketHandler();
		
		void	socketCreate();
		void	socketOptions();
		void	socketBind();
		void	socketListen();
		int		getFd();
		Server  server;
	private:
		int fd;
		int port;
		//N connection requests will be queued before further requests are refused
		int nqueue;
};
