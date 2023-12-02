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

class Resource {
	public:
		Resource(Request &httpReq, int clientFd, SocketHandler &server);
		static std::string readFile(std::string filePath);
	private:
		std::map<std::string, std::string> resourceToFileMapping;
		std::string itos(int num) const;
		void serveFile(std::string filePath, int clientFd) const;
        bool ft_find(std::string str, std::string to_find) const;
        void handleCGI(std::string filePath, Request &httpReq, int clientFd);
		void uploadFile(Request &httpReq, int clientFd);
};

#endif