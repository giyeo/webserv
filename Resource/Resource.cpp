#include "Resource.hpp"

std::string readFile(std::string filePath) {
	// Create an input file stream
	std::string fullPath = "/home/giyeo/webserv/webserv/_Files/" + filePath;
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

void serveFile(std::string filePath, int clientFd) {

	std::string fileContent = readFile(filePath);
	std::stringstream ss;

    // Convert the integer to a string and store it in the stringstream
    ss << fileContent.length();

    // Retrieve the string representation of the integer from the stringstream
    std::string contentLength = ss.str();

	Response httpRes(
		/*http_version,status_code,status_text*/
			"1.1","200","OK",
		/*content_type*/	
			"text/html",
		/*content_length*/
			contentLength,
		/*date*/
			"2023-09-20T00:31:02.612Z",
		/*server*/
			"webserv",
		/*cache_control*/
			"",
		/*set_cookie*/
			"",
		/*location*/
			"",
		/*connection*/
			"close",
		/*response_body*/
			fileContent
	);

	// std::cout << httpRes.toString() << "\n";
	
	int bytesSent = send(clientFd, httpRes.toString().c_str(), httpRes.toString().size(), 0);
	if (bytesSent < 0) {
		std::cerr << "Error sending response to client" << std::endl;
	}
}

bool Resource::ft_find(std::string str, std::string to_find) {
    if (str.find(to_find) != std::string::npos)
        return true;
    return false;
}

void Resource::handleCGI(std::string filePath, Request &httpReq, int clientFd) {
	std::string fullPath = "/home/giyeo/webserv/webserv/_Files/" + filePath;
	std::string command = "python3 " + fullPath;	
	std::stringstream ss;
	
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
	// std::cout << res << std::endl;
    ss << res.length();
    std::string contentLength = ss.str();
	Response httpRes(
		/*http_version,status_code,status_text*/
			"1.1","200","OK",
		/*content_type*/	
			"text/html",
		/*content_length*/
			contentLength,
		/*date*/
			"2023-09-20T00:31:02.612Z",
		/*server*/
			"webserv",
		/*cache_control*/
			"",
		/*set_cookie*/
			"",
		/*location*/
			"",
		/*connection*/
			"close",
		/*response_body*/
			res
	);
	std::cout << httpRes.toString() << "\n";
	int bytesSent = send(clientFd, httpRes.toString().c_str(), httpRes.toString().size(), 0);
	if (bytesSent < 0) {
		std::cerr << "Error sending response to client" << std::endl;
	}
}

Resource::Resource(Request &httpReq, int clientFd) {
	resourceToFileMapping["/"] = "index.html";
	resourceToFileMapping["/notFound"] = "notFound.html";
	resourceToFileMapping["/test.py"] = "test.py";
	std::string resourcePath = httpReq.getPath();

	if (resourceToFileMapping.find(resourcePath) != resourceToFileMapping.end()) {
		std::string filePath = resourceToFileMapping[resourcePath];
        if (ft_find(filePath, ".py"))
            handleCGI(filePath, httpReq, clientFd);
        else
		    serveFile(filePath, clientFd);
	} else {
		serveFile("notFound.html", clientFd);
	}
}