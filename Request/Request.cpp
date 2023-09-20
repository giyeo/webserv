#include "Request.hpp"

Request::Request(const char *recv) {
	this->revc = recv;
	request_parser();
}

void Request::request_parser() {
	this->verb = "GET";
}

std::string Request::getVerb() const {
	return this->verb;
}