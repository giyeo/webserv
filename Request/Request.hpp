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
#include <vector>

class Request {
	public:
		Request();
		Request(const char *recv);
		~Request();
		void parseRequestLineAndHeaders(const char *recv);
		void parseRequestBody(void);

		std::string getMethod() const;
		std::string getPath() const;
		unsigned long getContentLength() const;
		unsigned long getHeadersLength() const;
		std::vector<char> getRequestBody() const;
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

		void concatenateRequestBodyBuffer(const char *buffer);
		std::string requestBodyBuffer;
		ssize_t totalBytesRead;
	private:
		std::string method;
		std::string path;
		std::vector<char> requestBody;
		std::string contentType;
		unsigned long headersLength;
		unsigned long contentLength;
		std::map<std::string, std::string> pathVariables;
		std::map<std::string, std::string> headers;
};

#endif