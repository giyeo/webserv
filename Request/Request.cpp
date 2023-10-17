#include "Request.hpp"

Request::Request(const char *recv) {
	// std::cout << "---------------\n"<< recv << "\n---------------\n";
	requestParser(recv);
}

void Request::requestParser(const char *recv) {
	std::istringstream iss(recv);
	std::string line;
	//PARSE REQUEST LINE
	std::getline(iss, line);
	if(this->method.empty())
		parseRequestLine(splitLine(line, " "));
	//PARSE HEADERS
	while (std::getline(iss, line)) {
		if(parseHeaders(splitLine(line, ":")) == 1)
			break;
	}
	//PARSE REQUEST BODY (TEXT ONLY FOR NOW)
	if (headers.find("Content-Type") != headers.end()) {
        this->contentType = headers["Content-Type"];
	}
	std::cout << "|" << contentType.length() <<"| \n";
	if(this->method == "POST") {
		while (std::getline(iss, line)) {
			// std::cout << line << "\n";
		}
	}
}

void Request::parseRequestLine(std::vector<std::string> token) {
	std::vector<std::string> methods;
	methods.push_back("GET");
	methods.push_back("POST");
	methods.push_back("DELETE");
	if(token[0].empty() || token[1].empty())
		exit(1); //COULD NOT PARSE REQUEST LINE
	if (std::find(methods.begin(), methods.end(), token[0]) != methods.end()) {
		this->method = token[0];
		this->path = token[1];
	} else {
		exit(1); //HTTP RESPONSE: NOT IMPLEMENTED
	}
	pathVariables = parseQueryParameters(path);
	path = parsePath(path);
}

bool Request::parseHeaders(std::vector<std::string> token) {

	if(token.empty() || token.size() == 0 || token[0].empty())
		return true;
	std::string key = token[0];
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

std::string Request::getFormDataBoundary() const {
	return this->formDataBoundary;
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
