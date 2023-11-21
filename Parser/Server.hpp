#ifndef SERVER_HPP
#define SERVER_HPP

#include "Utils.hpp"
#include <string>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <sstream>
#include <map>
#include <algorithm>
#include <regex>

typedef struct s_location {
	std::string path;
}		Location;

class Server {
	public:
		Server();
		~Server();

		void parseServerName(std::string value);
		void parseListen(std::string value);
		void parseRoot(std::string value);
		void parseIndex(std::string value);
		void parseErrorPage(std::string value);
		void parseLocation(std::string value);
		void parseClientMaxBodySize(std::string value);
		void parseLocation(std::string path, std::string value); //.pushback to locations vector
		void dispatcher(std::string key, std::string value);
		std::string serverName;
		std::string listen;
		std::string root;
		std::string index;
		std::string errorPage;
		std::string location;
		std::string clientMaxBodySize;
		std::vector<Location> locations;
};

#endif