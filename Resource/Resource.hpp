#pragma once

#include "../Master.hpp"

class Resource {
	public:
		Resource();
		Resource(Config &config);
		static std::string readFile(std::string filePath);
	private:
		std::map<std::string, std::string> resourceToFileMapping;
		std::string itos(int num) const;
		int serveFile(Request &httpReq, int clientFd, SocketHandler &server, std::map<int, Request> &connections);
        bool ft_find(std::string str, std::string to_find) const;
		void handleCGI(std::string finalPath, Request &httpReq, std::string serverName, int clientFd);
		void uploadFile(Request &httpReq, int clientFd);
};
