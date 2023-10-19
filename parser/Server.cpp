#include "Server.hpp"

Server::Server() {}
Server::~Server() {}

void Server::parseServerName(std::string value){
	std::cout << value << "\n";
	//ex: example.com
	//ex: example.com www.example.com
}

void Server::parseListen(std::string value){
	std::cout << value << "\n";
	//ex: 80 (int only)
	//ex: 192.166.37.2:80 (int + . and :)
}

void Server::parseRoot(std::string value){
	std::cout << value << "\n";
	//ex: /var/www/html
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
	std::vector<std::string> names = {"server_name", "listen", "root", "index", "error_page", "client_max_body_size"};
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
	
	for(int i = 0; i < names.size(); i++) {
		if(names[i].compare(key) == 0) {
			(this->*functionPointers[i])(value);
		}
	}
}