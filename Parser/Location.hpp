#ifndef LOCATION_HPP
#define LOCATION_HPP

#include "Utils.hpp"
#include <string>
#include <unistd.h>



class Location {
public:
	std::string path;
	std::string root;
	std::string index;
	std::string errorPage;
	std::string returnCode;
	std::string proxyPass;

	void parseRoot(std::string value);
	void parseIndex(std::string value);
	void parseErrorPage(std::string value);
	void parseProxyPass(std::string value);
	void dispatcher(std::string key, std::string value);
};

#endif