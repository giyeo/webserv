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
		std::cout << pathTokens[i] << " " << uriTokens[i] << "\n";
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

	log(__FILE__, __LINE__, concat(2,"Requested URI:",uri.c_str()), LOG);

	if (uri == "/")
		return returnFinalPath(serverRoot + '/' + server.index, "", server.root + '/' + server.errorPage);

	// printVector(uriTokens);
	// TODO: check if is needed to put server.locations[i].path == '/' + uriTokens[0] in the ifs

	std::vector<std::string> uriTokens = tokenizer(uri, '/');
	for (size_t i = 0; i < server.locations.size(); i++) {
		std::vector<std::string> pathTokens = tokenizer(server.locations[i].path, '/');
		if (isValidURI(pathTokens, uriTokens)) {
			std::cout << "Location found, root:" << server.locations[i].root << std::endl;
			locationRoot = server.locations[i].root;
			if (locationRoot == "") {
				std::cout << uriTokens.size() << pathTokens.size() << server.locations[i].index << '\n';
				if (uriTokens.size() == pathTokens.size() && server.locations[i].index.empty())
					return returnFinalPath(serverRoot + server.locations[i].path + '/' + server.index, server.locations[i].errorPage, server.errorPage);
				else if (uriTokens.size() == pathTokens.size() && server.locations[i].index != "") {
					std::cout << "returning: " << serverRoot +  server.locations[i].path + '/' + server.locations[i].index << "\n";
					return returnFinalPath(serverRoot + server.locations[i].path +  '/' + server.locations[i].index, server.locations[i].errorPage, server.errorPage);
				}
				return returnFinalPath(serverRoot + uri, server.locations[i].errorPage, server.errorPage);
			} else {
				if (uriTokens.size() == pathTokens.size() && server.locations[i].index == "")
					return returnFinalPath(locationRoot + server.locations[i].path +'/' + server.index, server.locations[i].errorPage, server.errorPage);
				else if (uriTokens.size() == pathTokens.size() && server.locations[i].index != "")
					return returnFinalPath(locationRoot + server.locations[i].path + '/' + server.locations[i].index, server.locations[i].errorPage, server.errorPage);
				return returnFinalPath(locationRoot + uri, server.locations[i].errorPage, server.errorPage);
			}
		}
	}
	log(__FILE__,__LINE__,concat(5, "default root:",server.root.c_str(),uri.c_str()," Error Page:",server.errorPage.c_str()), LOG);
	return returnFinalPath(server.root + uri, "", "/" + server.errorPage);
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

int	Resource::serveFile(Request &httpReq, int clientFd, SocketHandler &server, std::map<int, Request> &connections) {
	response_object resp;
	std::string serverRoot = server.server.root;
	std::string uri = httpReq.getPath();
	std::string fileContent;
	t_finalPath finalPath = getFinalPath(server.server, uri);
	std::string serverName = server.server.serverName[0];
	log(__FILE__, __LINE__, concat(3, "serveFile --- [", finalPath.finalPath.c_str(), "]"), LOG);

	if (ft_find(finalPath.finalPath, ".py")) {
		handleCGI(finalPath.finalPath, httpReq, clientFd);
		return 1;
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
		log(__FILE__, __LINE__, concat(2, "Redirecting to: ", pathErrorPage.c_str()), WARNING);
		connections.erase(clientFd);
		Response::notFoundResponse(clientFd, serverName, pathErrorPage);
		close(clientFd);
		return -1;
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
	return 1;
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

Resource::Resource(Request &httpReq, int clientFd, SocketHandler &server, std::map<int, Request> &connections) {

	std::string resourcePath = httpReq.getPath();
	std::string method = httpReq.getMethod();

	log(__FILE__,__LINE__,method.c_str(), LOG);
	if (method == "GET") {
		serveFile(httpReq, clientFd, server, connections);
	}
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
