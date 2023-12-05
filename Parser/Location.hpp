#pragma once

#include "../Master.hpp"

class Location {
	public:
		Location();
		
		std::string path;
		std::string root;
		std::string index;
		std::string errorPage;
		std::string proxyPass;

		void parseRoot(std::string value);
		void parseIndex(std::string value);
		void parseErrorPage(std::string value);
		void parseProxyPass(std::string value);
		void dispatcher(std::string key, std::string value);
};
