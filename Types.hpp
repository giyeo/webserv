#ifndef TYPES_HPP
#define TYPES_HPP

typedef struct s_response {
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
	std::string filename;

	s_response() :
		http_version("1.1"),
		status_code("200"),
		status_text("OK"),
		content_type(), 
		content_length(),
		date(__DATE__),
		server(), 
		cache_control(),
		set_cookie(),
		location(),
		connection("close"),
		response_body(),
		filename() {}
}	response_object;

typedef struct cgiInfo {
	int clientFd;
	bool isRead;
}	t_cgiInfo;

typedef struct s_config {
	t_cgiInfo cgiInfo;
	response_object res;
	std::map<int, Request> connectionHeaders;
	std::map<int, t_cgiInfo> cgiConnections;
	std::map<int, int> whoswho;
	std::vector<SocketHandler> serverSockets;
	std::vector<Server> servers;

	//Redefined at Each request
	Request httpReq;
	int epollFd;
	int serverFd;
	int clientFd;
	int serverSocketIndex;
}	t_config;

#endif