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
	if (access(value.c_str(), R_OK) != 0)
		throw std::invalid_argument("Invalid root value");
	this->root = value;
}

void Server::parseIndex(std::string value){
	std::cout << value << "\n";
	//ex: index.html
}

void Server::parseErrorPage(std::string value){
	std::cout << value << "\n";
	//ex: 404 /404.html
}

void Server::parseClientMaxBodySize(std::string value){
	std::cout << value << "\n";
	//ex: 8042 (int only)
}

void Server::parseLocation(std::string path, std::string value){
	std::cout << path << " " << value << "\n";
	//ex: path: /dir/
	//ex: value: try_files $uri $uri/ =404  try_files $uri $uri/ =404          
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