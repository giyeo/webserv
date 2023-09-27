#include "Request.hpp"

Request::Request(const char *recv) {
	this->recv = recv;
	request_parser();
	printRequest();
}

void Request::request_parser() {
    std::string line;
    std::istringstream iss(this->recv);
    while (std::getline(iss, line)) {
        store(splitLine(line));
	}
}

void Request::store(std::vector<std::string> token) {

	if(token.empty())
		return ;
	std::vector<std::string> methods;
	methods.push_back("GET");
    methods.push_back("POST");
    methods.push_back("DELETE");

    if (this->verb.empty() && std::find(methods.begin(), methods.end(), token[0]) != methods.end()) {
        this->verb = token[0];
		this->path = token[1];
    } else {
		std::string key = token[0];
		std::string value = token[1];
		this->headers[key] = value;
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
	std::cout << "Method:"<< verb << "\n" << "Path:" << path << "\n";

	for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
        std::cout << it->first << it->second << std::endl;
    }
}

std::string Request::getVerb() const {
	return this->verb;
}