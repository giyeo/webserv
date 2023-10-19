#include "Server.hpp"

Server::Server() {}
Server::~Server() {}

void Server::parseServerName(std::string value){
	//ex: example.com
	//ex: example.com www.example.com
}

void Server::parseListen(std::string value){
	//ex: 80 (int only)
}

void Server::parseRoot(std::string value){
	//ex: /var/www/html
}

void Server::parseIndex(std::string value){
	//ex: index.html
}

void Server::parseErrorPage(std::string value){
	//ex: 404 /404.html
}

void Server::parseClientMaxBodySize(std::string value){
	//ex: 8042 (int only)
}

void Server::parseLocation(std::string path, std::string value){
	//ex: path: /dir/
	//ex: value: try_files $uri $uri/ =404  try_files $uri $uri/ =404          
}
