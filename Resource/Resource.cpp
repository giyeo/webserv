#include "Resource.hpp"

std::string getContentType(std::string finalPath) {
	std::map<std::string, std::string> fileMimeMap;
	fileMimeMap[".txt"] = "text/plain";
	fileMimeMap[".html"] = "text/html";
	fileMimeMap[".css"] = "text/css";
	fileMimeMap[".json"] = "application/json";
	fileMimeMap[".xml"] = "application/xml";
	fileMimeMap[".jpeg"] = "image/jpeg";
	fileMimeMap[".png"] = "image/png";
	fileMimeMap[".gif"] = "image/gif";
	fileMimeMap[".ico"] = "image/x-icon";
	fileMimeMap[".mp3"] = "audio/mpeg";
	fileMimeMap[".mp4"] = "video/mp4";
	fileMimeMap[".js"] = "text/javascript";

	std::vector<std::string> tokens = tokenizer(finalPath, '.');
	std::map<std::string, std::string>::iterator it = fileMimeMap.find("." + tokens[1]);
	if (it != fileMimeMap.end()) {
		log(__FILE__, __LINE__, concat(4, "MIME type for ", tokens[1].c_str(), ": ", it->second.c_str()), WARNING);
		return fileMimeMap[tokens[1]];
	} else {
		log(__FILE__, __LINE__, concat(3, "Error: File extension ", tokens[1].c_str(), " not found in fileMimeMap."), WARNING);
		return "text/plain";
	}
}

std::string getFileName(std::string finalPath) {
	return tokenizer(finalPath, '/').back();
}

// int	Resource::serveFile(Request &httpReq, int clientFd, SocketHandler &server, std::map<int, Request> &connections) {
void Resource::serveFile(Config &config) {	
	response_object resp;
	std::string serverRoot = config.server.server.root;
	std::string uri = config.httpReq.getPath();
	std::string fileContent;
	int clientFd = config.clientFd;
	GetPath getPathObj;
	t_finalPath finalPath = getPathObj.getFinalPath(config.server.server, uri);
	std::string serverName = config.server.server.serverName[0];
	log(__FILE__, __LINE__, concat(3, "serveFile --- [", finalPath.finalPath.c_str(), "]"), LOG);

	if(config.server.server.locations[finalPath.locationIndex].methods.find("GET") == std::string::npos) {
		errorPage(config, "405", "Method not allowed");
		return ;
	}

	fileContent = readFile(finalPath.finalPath);
	if(fileContent == "") {
		std::string pathErrorPage = finalPath.errorPage;
		log(__FILE__, __LINE__, concat(2, "Redirecting to: ", pathErrorPage.c_str()), WARNING);
		config.events.erase(clientFd);
		notFoundResponse(config, pathErrorPage);
		return ;
	}

	if (ft_find(finalPath.finalPath, ".py") && finalPath.locationIndex != -1) {
		if(config.server.server.locations[finalPath.locationIndex].proxyPass == finalPath.filename)
			handleCGI(config, finalPath.finalPath);
		return ;
	}

	resp.content_type = getContentType(finalPath.finalPath);
	resp.content_length = itos(fileContent.length());
	resp.server = serverName;
	resp.response_body = fileContent;
	if(resp.content_type == "video/mp4" || resp.content_type == "audio/mp3")
		resp.filename = getFileName(finalPath.finalPath);

	std::string buffer = resToString(resp);

	config.events[clientFd].buffer = buffer;
	config.events[clientFd].bytes = buffer.size();

	struct epoll_event ev;
	ev.events = EPOLLOUT;
	ev.data.fd = clientFd;
	epoll_ctl(config.epollFd, EPOLL_CTL_MOD, clientFd, &ev);
}

void Resource::handleCGI(Config &config, std::string finalPath) {
	log(__FILE__, __LINE__, "Entering handleCGI", LOG);
	response_object resp;
	std::string command = "python3 " + finalPath;
	int clientFd = config.clientFd;

	setenv("CLIENT_FD", intToString(clientFd).c_str(), 1);
	setenv("REQUEST_METHOD", config.httpReq.getMethod().c_str(), 1);
	FILE *fp = popen(command.c_str(), "w");
	if (fp == NULL) {
		std::cerr << "Error opening pipe" << std::endl;
		exit(EXIT_FAILURE);
	}

	std::string resbuffer = config.events[clientFd].buffer;
	config.events[clientFd].type = SERVICE;
	config.events[clientFd].buffer = resbuffer;
	config.events[clientFd].bytes = resbuffer.size();
	config.events[clientFd].fp = fp;

	epoll_event event;
	event.events = EPOLLOUT | EPOLLET;
	event.data.fd = config.clientFd;
	if (epoll_ctl(config.epollFd, EPOLL_CTL_MOD, config.clientFd, &event) == -1)
		log(__FILE__,__LINE__,"epoll_ctl failed", ERROR);
}

void Resource::uploadFile(Config &config) {
	response_object resp;
	int clientFd = config.clientFd;
	std::string fileContent = config.events[clientFd].req.requestBodyBuffer;

	resp.http_version = "1.1";
	resp.status_code = "200";
	resp.status_text = "OK";
	resp.content_type = config.httpReq.getHeaderValue("Content-Type");
	resp.content_length = itos(fileContent.length());
	resp.date = "2023-09-20T00:31:02.612Z";
	resp.server = "webserv";
	resp.connection = "close";
	resp.response_body = fileContent;

	std::string buffer = resToString(resp);
	config.events[clientFd].buffer = buffer;
	config.events[clientFd].bytes = buffer.size();

	struct epoll_event ev;
	ev.events = EPOLLOUT;
	ev.data.fd = clientFd;
	epoll_ctl(config.epollFd, EPOLL_CTL_MOD, clientFd, &ev);
}

Resource::Resource() {}

Resource::Resource(Config &config) {
	log(__FILE__,__LINE__,"Dispaching Resource", LOG);
	std::string resourcePath = config.httpReq.getPath();
	std::string method = config.httpReq.getMethod();
	//
	log(__FILE__,__LINE__,method.c_str(), LOG);
	if (method == "GET") {
		serveFile(config);
	}
	else if (method == "POST")//TODO POST HANDLING UPLOAD FILES AND ETC TO CGI
		uploadFile(config);
	// else if (method == "DELETE");
		//TODO alguma coisa;
}

bool Resource::ft_find(std::string str, std::string to_find) const {
    if (str.find(to_find) != std::string::npos)
        return true;
    return false;
}

std::string Resource::itos(int num) const {
	std::stringstream ss;
    ss << num;
    return ss.str();
}

std::string Resource::readFile(std::string fullPath) {
	// Create an input file stream
	std::ifstream inputFile(fullPath.c_str(), std::ifstream::in);

    // Check if the file was successfully opened
    if (!inputFile.is_open()) {
		log(__FILE__, __LINE__, concat(2, "Failed to open file: ", fullPath.c_str()), FAILED);
        return "";
    }

    // Read and print the content of the file line by line
	std::string res;
    std::string line;
    while (std::getline(inputFile, line)) {
		res += line;
		res += "\n";
    }

    // Close the file
    inputFile.close();
	log(__FILE__, __LINE__, concat(2, "Success opening File: ", fullPath.c_str()), LOG);
	return res;
}
