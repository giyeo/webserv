#include "Resource.hpp"

bool isDirectory(const char *path) {
    struct stat fileStat;

    // Use stat to get information about the file
    if (stat(path, &fileStat) != 0) {
        std::cerr << "Error checking file stat." << std::endl;
        return false;
    }

    // Check if it's a directory
    return S_ISDIR(fileStat.st_mode);
}

std::string getDirectoryString(std::string path) {
	DIR *dir;
    struct dirent *ent;
	std::string res;
    // Open the current directory
    if ((dir = opendir(path.c_str())) != NULL) {
        // Print all files and directories within the current directory
        while ((ent = readdir(dir)) != NULL) {
            res.append(ent->d_name);
			res.append("\n");
        }
        closedir(dir);
    } else {
        // Could not open directory
        std::cerr << "Error opening directory." << std::endl;
        return "";
    }
	return res;
}


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
	std::string serverName = config.serverName;
	log(__FILE__, __LINE__, concat(3, "serveFile --- [", finalPath.finalPath.c_str(), "]"), LOG);

	if(finalPath.locationIndex != -1) {
		Location myLocation = config.server.server.locations[finalPath.locationIndex];

		if(myLocation.autoindex.find("on") != std::string::npos && isDirectory(finalPath.finalPath.c_str())) {
			log(__FILE__, __LINE__, "Is directory getting values and returning", WARNING);
			directoryListing(config, getDirectoryString(finalPath.finalPath));
			return ;
		}

		if(myLocation.returnCode.empty() == 0) {
			log(__FILE__, __LINE__, "RETURNING", WARNING);
			redirectPage(config, myLocation.returnCode, myLocation.returnPath);
			return ;
		}

		if(myLocation.methods.find(config.httpReq.getMethod()) == std::string::npos) {
			errorPage(config, "405", "Method not allowed");
			return ;
		}
	}

	fileContent = readFile(finalPath.finalPath);
	if(fileContent == "") {
		std::string pathErrorPage = finalPath.errorPage;
		log(__FILE__, __LINE__, concat(2, "Redirecting to: ", pathErrorPage.c_str()), WARNING);
		notFoundResponse(config, pathErrorPage);
		return ;
	}

	if (ft_find(finalPath.finalPath, ".py") && finalPath.locationIndex != -1) {
		if(config.server.server.locations[finalPath.locationIndex].proxyPass == finalPath.filename) {
			if (config.httpReq.getMethod() == "POST")
				handleCGI(config, finalPath.finalPath, "POST");
			else if (config.httpReq.getMethod() == "DELETE")
				handleCGI(config, finalPath.finalPath, "DELETE");
			else
				handleCGI(config, finalPath.finalPath, "GET");
			return ;
		}
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

void Resource::handleCGI(Config &config, std::string finalPath, std::string method) {
	log(__FILE__, __LINE__, "Entering handleCGI", LOG);
	log(__FILE__, __LINE__, method.c_str(), LOGBLUE);
	
	response_object resp;
	std::string command = "python3 " + finalPath;
	int clientFd = config.clientFd;
	if(config.httpReq.getHeaders()["uploadName"].empty()) {
		log(__FILE__, __LINE__, "uploadName must be set in headers for CGI use", FAILED);
		epoll_ctl(config.epollFd, EPOLL_CTL_DEL, config.clientFd, NULL);
		config.events.erase(clientFd);
		close(config.clientFd);
	}
	
	size_t lastSlashPos = finalPath.find_last_of('/');
    std::string directory = finalPath.substr(0, lastSlashPos + 1);

	setenv("PATH_INFO", directory.c_str(), 1);
	setenv("UPLOADNAME", config.httpReq.getHeaders()["uploadName"].c_str(), 1);
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
	event.events = EPOLLOUT;
	event.data.fd = config.clientFd;
	if (epoll_ctl(config.epollFd, EPOLL_CTL_MOD, config.clientFd, &event) == -1)
		log(__FILE__,__LINE__,"epoll_ctl failed", FAILED);
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

	log(__FILE__,__LINE__,method.c_str(), LOG);
	if (method == "GET" || method == "POST" || method == "DELETE") {
		serveFile(config);
	}
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
