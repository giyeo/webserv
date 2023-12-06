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
int	Resource::serveFile(Config &config) {	
	response_object resp;
	std::string serverRoot = config.server.server.root;
	std::string uri = config.httpReq.getPath();
	std::string fileContent;
	int clientFd = config.clientFd;
	GetPath getPathObj;
	t_finalPath finalPath = getPathObj.getFinalPath(config.server.server, uri);
	std::string serverName = config.server.server.serverName[0];
	log(__FILE__, __LINE__, concat(3, "serveFile --- [", finalPath.finalPath.c_str(), "]"), LOG);

	fileContent = readFile(finalPath.finalPath);
	if(fileContent == "") {
		std::string pathErrorPage = finalPath.errorPage;
		log(__FILE__, __LINE__, concat(2, "Redirecting to: ", pathErrorPage.c_str()), WARNING);
		config.events.erase(clientFd);
		Response::notFoundResponse(clientFd, serverName, pathErrorPage);
		close(clientFd);
		return -1;
	}

	if (ft_find(finalPath.finalPath, ".py") && finalPath.locationIndex != -1) {
		if(config.server.server.locations[finalPath.locationIndex].proxyPass == finalPath.filename)
			handleCGI(finalPath.finalPath, config.httpReq, serverName, clientFd);
		return 1;
	}

	resp.content_type = getContentType(finalPath.finalPath);
	resp.content_length = itos(fileContent.length());
	resp.server = serverName;
	resp.response_body = fileContent;
	if(resp.content_type == "video/mp4" || resp.content_type == "audio/mp3")
		resp.filename = getFileName(finalPath.finalPath);

	Response httpRes(resp);
	std::string buffer = httpRes.toString();

	config.events[clientFd].buffer = buffer;
	config.events[clientFd].bytes = buffer.size();

	struct epoll_event ev;
	ev.events = EPOLLOUT | EPOLLET;
	ev.data.fd = clientFd;
	epoll_ctl(config.epollFd, EPOLL_CTL_MOD, clientFd, &ev);
	return 1;
}

void Resource::handleCGI(std::string finalPath, Request &httpReq, std::string serverName, int clientFd) {
	response_object resp;
	std::string command = "python3 " + finalPath;	
	
	(void)httpReq;

	setenv("REQUEST_METHOD", "GET", 1);
	FILE *fp = popen(command.c_str(), "r");
	if (fp == NULL) {
		std::cerr << "Error opening pipe" << std::endl;
		exit(EXIT_FAILURE);
	}
	
	char buffer[1024];
	std::string res;
	while (fgets(buffer, 1024, fp) != NULL) {
		res += buffer;
	}
	pclose(fp);

	resp.content_type = getContentType(res);
	resp.content_length = itos(res.length());
	resp.server = serverName;
	resp.response_body = res;
	if(resp.content_type == "video/mp4" || resp.content_type == "audio/mp3")
		resp.filename = getFileName(res);

	Response httpRes(resp);
	httpRes.sendResponse(clientFd);
	close(clientFd);
}

void Resource::uploadFile(Request &httpReq, int clientFd) {
	response_object resp;
	std::string fileContent = httpReq.requestBodyBuffer;

	resp.http_version = "1.1";
	resp.status_code = "200";
	resp.status_text = "OK";
	resp.content_type = httpReq.getHeaderValue("Content-Type");
	resp.content_length = itos(fileContent.length());
	resp.date = "2023-09-20T00:31:02.612Z";
	resp.server = "webserv";
	resp.connection = "close";
	resp.response_body = fileContent;

	Response httpRes(resp);
	httpRes.sendResponse(clientFd);
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
	else if (method == "POST")
		uploadFile(config.httpReq, config.clientFd);
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
