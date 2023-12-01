#include "Server.hpp"

Server::Server() {}
Server::~Server() {}


void Server::parseServerName(std::string value){
	std::vector<std::string> server_names = tokenizer(value, ' ');
	this->serverName = server_names;
}

void Server::parseListen(std::string value){
	for(size_t i = 0; i < value.size(); i++) {
		if(!isdigit(value[i]) && value[i] != '.' && value[i] != ':') {
			throw std::invalid_argument("Invalid listen value");
		}
	}
	this->listen = value;
}

void Server::parseRoot(std::string value){
	std::vector<std::string> tokens = tokenizer(value, ' ');
	if(tokens.size() != 1)
		throw std::invalid_argument("Invalid number of arguments in a \"root\" directive");
	this->root = value;
}

void Server::parseIndex(std::string value){
	this->index = value;
}

// In case of invalid error page, nginx behavior is redirect to a inexistent page.
// I prefer throw an exception instead.
void Server::parseErrorPage(std::string value){
	std::vector<std::string> tokens = tokenizer(value, ' ');

	std::string error_page = tokens.back();
	if(access(error_page.c_str(), R_OK) != 0)
		throw std::invalid_argument("Invalid error_page value");
	this->errorPage = error_page;
}

void Server::parseClientMaxBodySize(std::string value){
	for (size_t i = 0; i < value.size(); i++) {
		if(isdigit(value[i]) == 0) {
			throw std::invalid_argument("Invalid client_max_body_size value");
		}
	}
	this->clientMaxBodySize = value;
}

void Server::parseLocation(std::string path, std::string value) {
	Location location;
	std::istringstream ss(value);
	std::string line;
	std::string directive;
	std::string argument;

	location.path = path;
	
	while (getline(ss, line)) {
		std::vector<std::string> line_tokens = tokenizer(line, ' ');

		directive = line_tokens[0];
		while(line_tokens.size() > 1) {
			argument += line_tokens[1];
			if(line_tokens.size() > 2)
				argument += " ";
			line_tokens.erase(line_tokens.begin() + 1);
		}
		location.dispatcher(directive, argument);
		directive.clear();
		argument.clear();
	}

	this->locations.push_back(location);
}

typedef void (Server::*MemberFunction)(std::string);

void Server::dispatcher(std::string key, std::string value) {
	std::vector<std::string> names;
	names.push_back("server_name");
	names.push_back("listen");
	names.push_back("root");
	names.push_back("index");
	names.push_back("error_page");
	names.push_back("client_max_body_size");

	std::vector<MemberFunction> functionPointers;
    functionPointers.push_back(&Server::parseServerName);
    functionPointers.push_back(&Server::parseListen);
    functionPointers.push_back(&Server::parseRoot);
    functionPointers.push_back(&Server::parseIndex);
    functionPointers.push_back(&Server::parseErrorPage);
    functionPointers.push_back(&Server::parseClientMaxBodySize);
	
	if(names.size() != functionPointers.size()) {
		std::cout << "diferent sizes in dispatcher\n";
		return ;
	}
	
	for(size_t i = 0; i < names.size(); i++) {
		try {
			if(names[i].compare(key) == 0) {
				(this->*functionPointers[i])(value);
			}
		} catch (std::invalid_argument& e) {
			std::cerr << e.what() << std::endl;
		} catch (std::exception& e) {
			std::cerr << e.what() << std::endl;
		}
	}
}