#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <cstring>
#include <vector>
#include <map>
#include <algorithm>

class Request { 
	public:
		Request(const char *recv);
		void requestParser(const char *recv);

		std::string getVerb() const;
		void store(std::vector<std::string> token);
        std::vector<std::string> splitLine(std::string line) const;
		void printRequest() const;
	private:
		std::string verb;
		std::string path;
		std::map<std::string, std::string> headers;
};

#endif