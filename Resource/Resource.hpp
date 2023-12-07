#pragma once

#include "../Main.hpp"
#include "../Config.hpp"
#include "../Response/Response.hpp"
#include "GetPath.hpp"

class Resource {
	public:
		Resource();
		Resource(Config &config);
		static std::string readFile(std::string filePath);
	private:
		std::map<std::string, std::string> resourceToFileMapping;
		std::string itos(int num) const;
		// int serveFile(Request &httpReq, int clientFd, SocketHandler &server, std::map<int, Request> &connections);
        int serveFile(Config &config);
		bool ft_find(std::string str, std::string to_find) const;
		void handleCGI(Config &config, std::string finalPath);
		void uploadFile(Config &config);
};
