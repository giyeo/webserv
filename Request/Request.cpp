#include "Request.hpp"

Request::Request() : totalBytesRead(0),  headersLength(0), contentLength(0) {

}

Request::Request(const char *recv) : totalBytesRead(0), headersLength(0), contentLength(0) {
	parseRequestLineAndHeaders(recv);
}

bool Request::parseRequestBody() {
	const char *recv = requestBodyBuffer.c_str();
	contentLength = headersLength["Content-Length"];
	if(contentLength > maxBodySize) {
		log(__FILE__, __LINE__, "maxBodySizeError", WARNING);
		return false;
	}

	contentType = headers["Content-Type"];
	if (this->method == "POST"
		&& headers["Content-Type"] == "text/plain"
		&& this->contentLength != 0)
		replicateHttpRequestContent(recv);
	return true;
}

unsigned long calculateHeadersLength(const char *recv) {
	const char* contentStart = strstr(recv, "\r\n\r\n");
	if (!contentStart) {
		std::cerr << "Invalid HTTP request: no blank line separating headers and body." << std::endl;
		exit(1);
	}
	contentStart += 4;
	return contentStart - recv;
}

void Request::parseRequestLineAndHeaders(const char *recv) {
	std::istringstream iss(recv);
	std::string line;
	//PARSE REQUEST LINE
	std::getline(iss, line);
	if(this->method.empty())
		parseRequestLine(splitLine(line, " "));
	//PARSE HEADERS
	while (std::getline(iss, line)) {
		if(parseHeaders(tokenizer(line, ':')) == 1)
			break;
	}
	this->headersLength = calculateHeadersLength(recv);
	this->contentLength = atoi(headers["Content-Length"].c_str());
}

void Request::parseRequestLine(std::vector<std::string> token) {
	std::vector<std::string> methods;
	methods.push_back("GET");
	methods.push_back("POST");
	methods.push_back("DELETE");
	if(token[0].empty() || token[1].empty()) {
		std::cerr << "COULD NOT PARSE REQUEST LINE\n";
		exit(1); //COULD NOT PARSE REQUEST LINE
	}
	if (std::find(methods.begin(), methods.end(), token[0]) != methods.end()) {
		this->method = token[0];
		this->path = token[1];
	} else {
		std::cerr << "HTTP RESPONSE: NOT IMPLEMENTED\n";
		exit(1); //HTTP RESPONSE: NOT IMPLEMENTED
	}
	pathVariables = parseQueryParameters(path);
	path = parsePath(path);
}

bool Request::parseHeaders(std::vector<std::string> token) {

	if(token.empty() || token.size() == 0 || token[0].empty())
		return true;
	std::string key = token[0];

	size_t lastDigit = token[1].length() - 1;
	if(token[1][lastDigit] == '\r')
		token[1] = token[1].substr(0, lastDigit);
	if (token.size() == 3) {
		lastDigit = token[2].length() - 1;
		if(token[2][lastDigit] == '\r')
			token[2] = token[2].substr(0, lastDigit);
		this->headers["Port"] = token[2];
	}
	std::string value = token[1];
	this->headers[key] = value.substr(1, value.npos);
	return false;
}

std::vector<std::string> Request::splitLine(std::string line, std::string fline) const {

	char *token = (char *)line.c_str();
	std::vector<std::string> v;
	if(line.length() == 1)
		return v;
	char *p = strtok(token, fline.c_str());
	while (p != NULL) {
		v.push_back(p);
		p = strtok(NULL, fline.c_str());
	}
	return v;
}

void Request::replicateHttpRequestContent(const char* recv) {
	// Find the start of the content by searching for the blank line that separates headers and body

	const char* contentStart = recv + headersLength;
	std::size_t length = std::char_traits<char>::length(contentStart);
	std::cout << "content:" << contentLength << ", calculed:" << length << "\n\n";
	std::vector<char> charVector(contentStart, contentStart + length);
	this->requestBody = charVector;
	// Open the output file in binary mode
	std::ofstream outputFileStream("outputFile.txt", std::ios::binary);
	if (!outputFileStream) {
		std::cerr << "Error opening output file." << std::endl;
		return;
	}
	// Write the content to the output file
	outputFileStream.write(contentStart, contentLength);
	// Close the output file
	outputFileStream.close();
}

void Request::printRequest() const {
	std::cout <<  "---------------------\n";
	std::cout << "Method:"<< method << "\n" << "Path:" << path << "\n";
	printMap(headers);
	printMap(pathVariables);
	std::cout <<  "---------------------\n";
}

void Request::printMap(std::map<std::string, std::string> map) const {
	for (std::map<std::string, std::string>::const_iterator it = map.begin(); it != map.end(); ++it) {
		std::cout << it->first << " " << it->second << "." << std::endl;
	}
}

std::string Request::getMethod() const {
	return this->method;
}

std::string Request::getPath() const {
	return this->path;
}

std::map<std::string, std::string> Request::getHeaders() const {
	return this->headers;
}

std::map<std::string, std::string> Request::getPathVariables() const {
	return this->pathVariables;
}

unsigned long Request::getContentLength() const {
	return this->contentLength;
}

unsigned long Request::getHeadersLength() const {
	return this->headersLength;
}

std::vector<char> Request::getRequestBody() const {
	return this->requestBody;
}

std::string Request::getHeaderValue(std::string headerName) const {
	std::map<std::string, std::string>::const_iterator it = headers.find(headerName);
	if (it != headers.end())
		return it->second;
	std::cout << "Header not found";
	return "";
}

std::map<std::string, std::string> Request::parseQueryParameters(std::string fullPath) const {
    std::map<std::string, std::string> res;
    size_t pos = fullPath.find("?") + 1;
    while (pos != fullPath.npos) {
        size_t next = fullPath.find('&', pos); // Find the next '&'
        std::string param;

        if (next != fullPath.npos) {
            param = fullPath.substr(pos, next - pos); // Extract the parameter
        } else {
            param = fullPath.substr(pos); // Extract the last parameter
        }

        size_t equalPos = param.find(':');

        if (equalPos != param.npos) {
            std::string key = param.substr(0, equalPos);
            std::string value = param.substr(equalPos + 1);
            res[key] = value;
        }

        if (next == fullPath.npos) {
            break; // No more parameters to process
        }

        pos = next + 1; // Move to the character after '&'
    }

    return res;
}

std::string Request::parsePath(std::string fullPath) const {
	if(fullPath.find("?") == fullPath.npos)
		return fullPath;
	return fullPath.substr(0,fullPath.find("?"));
}

Request::~Request() {
}