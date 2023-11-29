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
#include <fstream>

class Request { 
	public:
		Request(const char *recv);
		void requestParser(const char *recv);

		std::string getMethod() const;
		std::string getPath() const;
		std::string getFormDataBoundary() const;
		std::map<std::string, std::string> getPathVariables() const;
		std::map<std::string, std::string> getHeaders() const;

		void printMap(std::map<std::string, std::string> map) const;
		bool parseHeaders(std::vector<std::string> token);
		std::vector<std::string> splitLine(std::string line,std::string fline) const;
		void printRequest() const;
		std::string getHeaderValue(std::string headerName) const;
		std::map<std::string, std::string> parseQueryParameters(std::string fullPath) const;
		std::string parsePath(std::string fullPath) const;

		void replicateHttpRequestContent(const char* recv);
		void parseRequestLine(std::vector<std::string> token);
	private:
		std::string method;
		std::string path;
		std::string formDataBoundary;
		std::string contentType;
		unsigned long contentLength;
		std::map<std::string, std::string> pathVariables;
		std::map<std::string, std::string> headers;

};

#endif