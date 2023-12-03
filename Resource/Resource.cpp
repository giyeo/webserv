#include "Resource.hpp"

#define NOTFOUND 0
#define FOUND 1
#define REDIRECT_INDEX 2

typedef struct s_finalPath {
	std::string finalPath;
	std::string locationErrorPage;
	std::string serverErrorPage;
	std::string defaultErrorPage;

	s_finalPath() :
		finalPath(),
		locationErrorPage(),
		serverErrorPage(),
		defaultErrorPage("defaultErrorPage.html")
	{}
} t_finalPath;

bool isValidURI(std::vector<std::string> pathTokens, std::vector<std::string> uriTokens) {
	for (size_t i = 0; i < pathTokens.size(); i++) {
		if(pathTokens[i] != uriTokens[i])
			return false;
	}
	return true;
}

t_finalPath returnFinalPath(std::string finalPath, std::string locationErrorPage, std::string serverErrorPage) {
	t_finalPath res;
	res.finalPath = finalPath;
	res.locationErrorPage = locationErrorPage;
	res.serverErrorPage = serverErrorPage;
	return res;
}

t_finalPath getFinalPath(Server &server, std::string uri) {
	std::string finalPath;
	std::string serverRoot = server.root;
	std::string locationRoot;
	std::cout << "getFinalPath --- PATH REQUESTED:" << uri << std::endl;
	if (uri == "/")
		return returnFinalPath(serverRoot + '/' + server.index, "", server.root + '/' + server.errorPage);

	std::vector<std::string> uriTokens = tokenizer(uri, '/');

	// printVector(uriTokens);
	// TODO: check if is needed to put server.locations[i].path == '/' + uriTokens[0] in the ifs
	for (size_t i = 0; i < server.locations.size(); i++) {
		std::vector<std::string> pathTokens = tokenizer(server.locations[i].path, '/');
		if (isValidURI(pathTokens, uriTokens)) {
			std::cout << "getFinalPath --- Location found" << std::endl;
			locationRoot = server.locations[i].root;
			if (locationRoot.empty()) {
				if (uriTokens.size() == pathTokens.size() && server.locations[i].index.empty()) {
					
					return returnFinalPath(serverRoot + server.locations[i].path + '/' + server.index, server.locations[i].errorPage, server.root + '/' + server.errorPage);
				} 
				else if (uriTokens.size() == pathTokens.size() && server.locations[i].index != "") {
					std::cout << "returning: " << serverRoot +  server.locations[i].path + '/' + server.locations[i].index << "\n";
					return returnFinalPath(serverRoot + server.locations[i].path +  '/' + server.locations[i].index, server.locations[i].errorPage, server.root + '/' + server.errorPage);
				}
				return returnFinalPath(serverRoot + uri, server.locations[i].errorPage, server.root + '/' + server.errorPage);
			} else {
				if (uriTokens.size() == pathTokens.size() && server.locations[i].index == "")
					return returnFinalPath(locationRoot + server.locations[i].path +'/' + server.index, server.locations[i].errorPage, server.root + '/' + server.errorPage);
				else if (uriTokens.size() == pathTokens.size() && server.locations[i].index != "")
					return returnFinalPath(locationRoot + server.locations[i].path + '/' + server.locations[i].index, server.locations[i].errorPage, server.root + '/' + server.errorPage);
				return returnFinalPath(locationRoot + uri, server.locations[i].errorPage, server.root + '/' + server.errorPage);
			}
		}
	}
	return returnFinalPath(server.root + uri, "", server.root + '/' + server.errorPage);
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
	fileMimeMap[".mp3"] = "audio/mpeg";
	fileMimeMap[".mp4"] = "video/mp4";
	fileMimeMap[".js"] = "text/javascript";

	std::vector<std::string> tokens = tokenizer(finalPath, '.');
	std::map<std::string, std::string>::iterator it = fileMimeMap.find("." + tokens[1]);
	if (it != fileMimeMap.end()) {
		std::cout << "MIME type for " << tokens[1] << ": " << it->second << std::endl;
		return fileMimeMap[tokens[1]];
	} else {
		std::cerr << "Error: File extension " << tokens[1] << "not found in fileMimeMap." << std::endl;
		return "text/plain";
	}
}

std::string getFileName(std::string finalPath) {
	return tokenizer(finalPath, '/').back();
}

void Resource::serveFile(Request &httpReq, int clientFd, SocketHandler &server) {
	response_object resp;
	std::string serverRoot = server.server.root;
	std::string uri = httpReq.getPath();
	std::string fileContent;
	t_finalPath finalPath = getFinalPath(server.server, uri);
	std::string serverName = server.server.serverName[0];
	std::cout << "serveFile --- [" << finalPath.finalPath << "]\n";

	if (ft_find(finalPath.finalPath, ".py")) {
		handleCGI(finalPath.finalPath, httpReq, clientFd);
		return ;
	}

	fileContent = readFile(finalPath.finalPath);
	if(fileContent == "") {
		std::string pathErrorPage; 
		if (finalPath.locationErrorPage != "")
			pathErrorPage = finalPath.locationErrorPage;
		else if (finalPath.serverErrorPage != "")
			pathErrorPage = finalPath.serverErrorPage;
		else 
			pathErrorPage = finalPath.defaultErrorPage;
		Response::notFoundResponse(clientFd, serverName,
			readFile(pathErrorPage));
		return ;

	}

	resp.content_type = getContentType(finalPath.finalPath);
	resp.content_length = itos(fileContent.length());
	resp.server = serverName;
	resp.response_body = fileContent;
	if(resp.content_type == "video/mp4" || resp.content_type == "audio/mp3")
		resp.filename = getFileName(finalPath.finalPath);

	Response httpRes(resp);
	httpRes.sendResponse(clientFd);
	close(clientFd);
}

void Resource::handleCGI(std::string finalPath, Request &httpReq, int clientFd) {
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

	resp.http_version = "1.1";
	resp.status_code = "200";
	resp.status_text = "OK";
	resp.content_type = "text/html";
	resp.content_length = itos(res.length());
	resp.date = "2023-09-20T00:31:02.612Z";
	resp.server = "webserv";
	resp.connection = "close";
	resp.response_body = res;
	Response httpRes(resp);
	httpRes.sendResponse(clientFd);
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

Resource::Resource(Request &httpReq, int clientFd, SocketHandler &server) {

	std::string resourcePath = httpReq.getPath();
	std::string method = httpReq.getMethod();

	if (method == "GET")
		serveFile(httpReq, clientFd, server);
	else if (method == "POST")
		uploadFile(httpReq, clientFd);
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
        std::cerr << "Failed to open file: " << fullPath << std::endl;
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
	std::cout << "Success at Reading: " << fullPath << "\n"; 
	return res;
}
