#include "Resource.hpp"

void Resource::serveFile(std::string filePath, int clientFd) const {
	response_object resp;

	std::string fileContent = readFile(filePath);

	resp.http_version = "1.1";
	resp.status_code = "200";
	resp.status_text = "OK";
	resp.content_type = "text/html";
	resp.content_length = itos(fileContent.length());
	resp.date = "2023-09-20T00:31:02.612Z";
	resp.server = "webserv";
	resp.connection = "close";
	resp.response_body = fileContent;

	Response httpRes(resp);
	httpRes.sendResponse(clientFd);
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
	httpReq.getHeaderValue("");
	response_object resp;
	std::string fileContent = "nice";

	resp.http_version = "1.1";
	resp.status_code = "200";
	resp.status_text = "OK";
	resp.content_type = "text/html";
	resp.content_length = itos(fileContent.length());
	resp.date = "2023-09-20T00:31:02.612Z";
	resp.server = "webserv";
	resp.connection = "close";
	resp.response_body = fileContent;

	Response httpRes(resp);
	httpRes.sendResponse(clientFd);
}

Resource::Resource(Request &httpReq, int clientFd) {
	resourceToFileMapping["/"] = "index.html";
	resourceToFileMapping["/index.html"] = "index.html";
	resourceToFileMapping["/notFound"] = "notFound.html";
	resourceToFileMapping["/test.py"] = "test.py";
	resourceToFileMapping["/files"] = "files.html";

	std::string resourcePath = httpReq.getPath();
	std::string method = httpReq.getMethod();
	std::map<std::string, std::string> pathVariables = httpReq.getPathVariables();

	if (resourceToFileMapping.find(resourcePath) != resourceToFileMapping.end()) {
		std::string filePath = resourceToFileMapping[resourcePath];
		if (ft_find(filePath, ".py"))
			handleCGI(filePath, httpReq, clientFd);
		else if (method == "GET")
			serveFile(filePath, clientFd);
		else if (method == "POST")
			uploadFile(httpReq, clientFd);
	} else {
		serveFile("notFound.html", clientFd);
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

std::string Resource::readFile(std::string filePath) const {
	// Create an input file stream
	std::string fullPath = "./_Files/" + filePath;
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
    }

    // Close the file
    inputFile.close();
	return res;
}