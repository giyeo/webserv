#pragma once

#include "../Master.hpp"

class Request {
	public:
		Request();
		Request(const char *recv, size_t maxBodySize);
		~Request();
		void parseRequestLineAndHeaders(const char *recv);
		bool parseRequestBody(int clientFd, std::string serverName);

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
		size_t maxBodySize;
};
