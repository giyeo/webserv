#include "Request.hpp"

Request::Request(const char *recv) {
	this->recv = recv;
    std::cout << recv << std::endl;
	request_parser();
}

void Request::request_parser() {
    std::string line;
    std::istringstream iss(this->recv);

	
    while (std::getline(iss, line)) {
        char *token = strtok((char *)line.c_str(), " ");
        std::cout << "Verb method:" << this->verb << "\n\n" <<std::endl;
    }
}

std::string Request::getVerb() const {
	return this->verb;
}

// TODO: Implement this function in order to get key values from the request
std::vector<std::string> Request::splitLine(char *token) const {
    std::vector<std::string> v;
    char *p = strtok(token, " ");
    while (p != NULL) {
        v.push_back(p);
        p = strtok(NULL, " ");
    }
    return v;
}