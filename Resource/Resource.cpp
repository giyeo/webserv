#include "Resource.hpp"

#define NOTFOUND 0
#define FOUND 1
#define REDIRECT_INDEX 2
// / [x]
// /image [ ] 
// /images/ [ ]
// /images/1.jpg [ ]

std::string getFinalPath(Server &server, std::string uri) {
	std::string finalPath;
	std::string serverRoot = server.root;
	std::string locationRoot;
	std::cout << "getFinalPath --- PATH REQUESTED:" << uri << std::endl;
	if (uri == "/")
		return serverRoot + '/' + server.index;

	std::vector<std::string> uriTokens = tokenizer(uri, '/');

	// printVector(uriTokens);
	// TODO: check if is needed to put server.locations[i].path == '/' + uriTokens[0] in the ifs
	for (size_t i = 0; i < server.locations.size(); i++) {
		if (server.locations[i].path == '/' + uriTokens[0]) {
			std::cout << "getFinalPath --- Location found" << std::endl;
			locationRoot = server.locations[i].root;
			if (locationRoot.empty()) {
				if (uriTokens.size() == 1 && server.locations[i].index.empty()) 
					return serverRoot + server.locations[i].path + '/' + server.index;
				else if (uriTokens.size() == 1 && server.locations[i].index != "") {
					std::cout << "returning: " << serverRoot +  server.locations[i].path + '/' + server.locations[i].index << "\n";
					return serverRoot + server.locations[i].path +  '/' + server.locations[i].index;
				}
				return serverRoot + uri;
			} else {
				if (uriTokens.size() == 1 && server.locations[i].index == "")
					return locationRoot + server.locations[i].path +'/' + server.index;
				else if (uriTokens.size() == 1 && server.locations[i].index != "")
					return locationRoot + server.locations[i].path + '/' + server.locations[i].index;
				return locationRoot + uri;
			}
		}
	}
	return "";
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
	fileMimeMap[".svg"] = "image/svg+xml";
	fileMimeMap[".mp3"] = "audio/mpeg";
	fileMimeMap[".wav"] = "audio/wav";
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

void Resource::serveFile(Request &httpReq, int clientFd, SocketHandler &server) const {
	response_object resp;
	std::string serverRoot = server.server.root;
	std::string uri = httpReq.getPath();
	std::string fileContent;
	std::string finalPath = getFinalPath(server.server, uri);
	std::string serverName = server.server.serverName[0];
	std::cout << "serveFile --- [" << finalPath << "]\n";

	fileContent = readFile(finalPath);
	if(fileContent == "") {
		Response::notFoundResponse(clientFd, serverName,
			readFile(serverRoot + "/notFound.html"));
		return ;
	}

	resp.content_type = getContentType(finalPath);
	resp.content_length = itos(fileContent.length());
	resp.server = serverName;
	resp.response_body = fileContent;
	resp.filename = getFileName(finalPath);

	Response httpRes(resp);
	httpRes.sendResponse(clientFd);
	close(clientFd);
}

void Resource::handleCGI(std::string filePath, Request &httpReq, int clientFd) {
	response_object resp;
	std::string fullPath = "/home/giyeo/webserv/webserv/_Files/" + filePath;
	std::string command = "python3 " + fullPath;	
	
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
	// std::cout << res << std::endl;
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

	std::cout << clientFd << server.getFd() << "\n";

	//vai ter que fazer para cada server o resource file mapping
	//root é a pasta raiz.

	std::string resourcePath = httpReq.getPath();
	std::string method = httpReq.getMethod();

	// if (resourceToFileMapping.find(resourcePath) != resourceToFileMapping.end()) {
		// std::string filePath = resourceToFileMapping[resourcePath];

	// if (ft_find(filePath, ".py"))
		// handleCGI(filePath, httpReq, clientFd);
	if (method == "GET")
		serveFile(httpReq, clientFd, server);
	else if (method == "POST")
		uploadFile(httpReq, clientFd);
	// else if (method == "DELETE");
	// 		//TODO alguma coisa;
	// } //else {
		//serveFile("notFound.html", clientFd, server.server.root);
	//}
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
