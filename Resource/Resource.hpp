#ifndef RESOURCE_HPP
#define RESOURCE_HPP

#include "../Request/Request.hpp"
#include <map>
#include <iostream>
#include "../Response/Response.hpp"
#include <sys/socket.h>
#include <fstream>
#include <string>

class Resource {
	public:
		Resource(Request &httpReq, int clientFd);
	private:
		std::map<std::string, std::string> resourceToFileMapping;
};

#endif