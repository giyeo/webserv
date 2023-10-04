#include "Request.hpp"

Request::Request(const char *recv) {
	std::cout << "STARTING REQUEST PARSER\n";
	// std::cout << recv << "\n";
	requestParser(recv);
	pathVariables = parsePathVariables(path);
	path = parsePath(path);
	printRequest();
}

void Request::requestParser(const char *recv) {
	std::string line;
	std::istringstream iss(recv);
	while (std::getline(iss, line)) {
		store(splitLine(line), line);
	}
	std::cout << "END REQUEST PARTSER\n";
}

void Request::store(std::vector<std::string> token, std::string line) {

	static bool isRequestBody = false;
	if(token.empty() || token.size() == 0 || token[0].empty()) {
		isRequestBody = true;
		return ;
	}

	std::vector<std::string> methods;
	methods.push_back("GET");
	methods.push_back("POST");
	methods.push_back("DELETE");
	
	if (this->method.empty() && std::find(methods.begin(), methods.end(), token[0]) != methods.end()) {
		this->method = token[0];
		this->path = token[1];
		isRequestBody = false;
	} else if (isRequestBody == false) {
		std::string key = token[0];
		std::string value = token[1];
		this->headers[key] = value;
		std::cout << isRequestBody << key << value << "\n";
	} else {
		std::cout << isRequestBody << line << "\n";
	}
}

std::vector<std::string> Request::splitLine(std::string line) const {
	char *token = (char *)line.c_str();
	std::vector<std::string> v;
	if(line.length() == 1)
		return v;
	char *p = strtok(token, " ");
	while (p != NULL) {
		v.push_back(p);
		p = strtok(NULL, " ");
	}
	return v;
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
		std::cout << it->first << " " << it->second << std::endl;
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

std::string Request::getHeaderValue(std::string headerName) const {
	std::map<std::string, std::string>::const_iterator it = headers.find(headerName);
	if (it != headers.end())
       	return it->second;
	std::cout << "Header not found";
	return "";
}

std::map<std::string, std::string> Request::parsePathVariables(std::string fullPath) const {
    std::map<std::string, std::string> res;
    size_t pos = 2; // Start at position 2 to skip "/?"

    while (pos != fullPath.npos) {
        size_t next = fullPath.find('&', pos); // Find the next '&'
        std::string param;

        if (next != fullPath.npos) {
            param = fullPath.substr(pos, next - pos); // Extract the parameter
        } else {
            param = fullPath.substr(pos); // Extract the last parameter
        }

        size_t equalPos = param.find('=');

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