#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <cstring>
#include <vector>

class Request { 
	public:
		Request(const char *recv);
		void request_parser();

		std::string getVerb() const;
        std::vector<std::string> splitLine(char *token) const;
	private:
		const char *recv;
		std::string verb;
		std::string resource;
		std::string version;
		std::string request_url;
		std::string host;
		std::string user_agent;
		std::string authorization;
		std::string query_params;
		std::string content_type;
		std::string payload;
};

#endif