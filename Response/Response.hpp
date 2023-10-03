#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

typedef struct {
	//Status Line
	std::string http_version;
	std::string status_code;
	std::string status_text;
	//Headers
	std::string content_type;
	std::string content_length;
	std::string date;
	std::string server;
	std::string cache_control;
	std::string set_cookie;
	std::string location;
	std::string connection;
	//Reponse Body (HTML, JSON, XML, text, binary data, or any other format.)
	std::string response_body;
}	response_object;

class Response {
	public:
		Response(response_object &res);
		std::string toString();
		void sendResponse(int clientFd);
	private:
		response_object res;
};

#endif