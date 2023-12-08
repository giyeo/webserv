#pragma once

#include "../Main.hpp"
#include "Utils.hpp"

#define GET 0
#define POST 1
#define DELETE 2

class Location {
	public:
		Location();
		
		std::string path;
		std::string root;
		std::string index;
		std::string errorPage;
		std::string proxyPass;
		std::string autoindex;
		std::string methods;
		std::string returnCode;
		std::string returnPath;

		void parseRoot(std::string value);
		void parseIndex(std::string value);
		void parseErrorPage(std::string value);
		void parseProxyPass(std::string value);
		void parseAutoIndex(std::string value);
		void parseMethods(std::string value);
		void parseReturn(std::string value);
		void dispatcher(std::string key, std::string value);
};
