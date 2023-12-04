#ifndef RESOURCE_HPP
#define RESOURCE_HPP

#include "../Request/Request.hpp"
#include <map>
#include <iostream>
#include "../Response/Response.hpp"
#include <sys/socket.h>
#include <fstream>
#include <string>
#include "../Communication/SocketHandler.hpp"
#include "../Parser/Utils.hpp"

class Resource {
	public:
		Resource(Request &httpReq, int clientFd, SocketHandler &server, std::map<int, Request> &connections);
		static std::string readFile(std::string filePath);

	private:
		std::map<std::string, std::string> resourceToFileMapping;
		std::string itos(int num) const;
		int serveFile(Request &httpReq, int clientFd, SocketHandler &server, std::map<int, Request> &connections);
        bool ft_find(std::string str, std::string to_find) const;
        void handleCGI(std::string finalPath, Request &httpReq, int clientFd);
		void uploadFile(Request &httpReq, int clientFd);
};

#endif