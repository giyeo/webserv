#pragma once

#include "../Master.hpp"

class Response {
	public:
		Response(response_object &res);
		static void notFoundResponse(int fd, std::string serverName, std::string content);
		static void maxBodySizeResponse(int fd, std::string serverName, size_t size);
		std::string toString();
		void sendResponse(int clientFd);
	private:
		response_object res;
};
