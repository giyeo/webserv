#include "Config.hpp"

Config::Config(): epollFd(-42), serverFd(-42), clientFd(-42) {}

void Config::print() {
	std::cout << "EpollFd: " << this->epollFd << "\n";
	std::cout << "ServerFd: " << this->serverFd << "\n";
	std::cout << "ClientFd: " << this->clientFd << "\n";
}