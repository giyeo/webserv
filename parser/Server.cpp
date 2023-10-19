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
