#include "Request.hpp"

Request::Request(const char *recv) {

	std::cout << "---------------\n"<< recv << "\n---------------\n";
	requestParser(recv);
	pathVariables = parsePathVariables(path);
	path = parsePath(path);
	std::string contentType = headers["Content-Type"];
	if(!contentType.empty() && contentType.find("boundary=") != contentType.npos)
		formDataBoundary = contentType.substr(contentType.find("boundary=") + 9, contentType.npos);
	// printRequest();
}
bool createFileFromBuffer(const std::string& filename, const char* buffer, std::size_t bufferSize);

int stringToInt(const std::string& str) {
    std::istringstream ss(str);
    int number;
    
    // Use the stringstream to extract the integer
    if (!(ss >> number)) {
        // Handle the conversion error here, e.g., by returning a default value or throwing an exception
        throw std::runtime_error("Failed to convert string to integer.");
    }
    
    return number;
}

void count(const char *buffer) {
	int i = 0;
	while(buffer[i] != '\0') {
		i++;
	}
	std::cout << i << "\n";
	exit(1);
}

void Request::requestParser(const char *recv) {
	std::string line;
	std::istringstream iss(recv);
	int nonBodySize = 0;
	std::string fline = " ";
	while (std::getline(iss, line)) {
		if(store(splitLine(line, fline)) == 1)
			break;
		nonBodySize += line.size();
		fline = ":";
	}
	// const char *requestBody =  recv + nonBodySize;
	//request body case is post;
	// count(requestBody);
	// createFileFromBuffer("myfile.png", requestBody, stringToInt(headers["Content-Length"]));
}

bool Request::store(std::vector<std::string> token) {

	if(token.empty() || token.size() == 0 || token[0].empty()) {
		return true;
	}

	std::vector<std::string> methods;
	methods.push_back("GET");
	methods.push_back("POST");
	methods.push_back("DELETE");
	
	if (this->method.empty() && std::find(methods.begin(), methods.end(), token[0]) != methods.end()) {
		this->method = token[0];
		this->path = token[1];
	} else {
		std::string key = token[0];
		std::string value = token[1];
		this->headers[key] = value.substr(1, value.npos);
	}
	return false;
}

std::vector<std::string> Request::splitLine(std::string line, std::string fline) const {

	char *token = (char *)line.c_str();
	std::vector<std::string> v;
	if(line.length() == 1)
		return v;
	char *p = strtok(token, fline.c_str());
	while (p != NULL) {
		v.push_back(p);
		p = strtok(NULL, fline.c_str());
	}
	return v;
}

void Request::printRequest() const {
	std::cout <<  "---------------------\n";
	std::cout << "Method:"<< method << "\n" << "Path:" << path << "\n";
	printMap(headers);
	printMap(pathVariables);
	std::cout <<  "---------------------\n";
}

void Request::printMap(std::map<std::string, std::string> map) const {
	for (std::map<std::string, std::string>::const_iterator it = map.begin(); it != map.end(); ++it) {
		std::cout << it->first << " " << it->second << std::endl;
	}
}

std::string Request::getMethod() const {
	return this->method;
}

std::string Request::getPath() const {
	return this->path;
}

std::map<std::string, std::string> Request::getHeaders() const {
	return this->headers;
}

std::map<std::string, std::string> Request::getPathVariables() const {
	return this->pathVariables;
}

std::string Request::getFormDataBoundary() const {
	return this->formDataBoundary;
}

std::string Request::getHeaderValue(std::string headerName) const {
	std::map<std::string, std::string>::const_iterator it = headers.find(headerName);
	if (it != headers.end())
       	return it->second;
	std::cout << "Header not found";
	return "";
}

std::map<std::string, std::string> Request::parsePathVariables(std::string fullPath) const {
    std::map<std::string, std::string> res;
    size_t pos = fullPath.find("?") + 1;
    while (pos != fullPath.npos) {
        size_t next = fullPath.find('&', pos); // Find the next '&'
        std::string param;

        if (next != fullPath.npos) {
            param = fullPath.substr(pos, next - pos); // Extract the parameter
        } else {
            param = fullPath.substr(pos); // Extract the last parameter
        }

        size_t equalPos = param.find('=');

        if (equalPos != param.npos) {
            std::string key = param.substr(0, equalPos);
            std::string value = param.substr(equalPos + 1);
            res[key] = value;
        }

        if (next == fullPath.npos) {
            break; // No more parameters to process
        }

        pos = next + 1; // Move to the character after '&'
    }

    return res;
}

std::string Request::parsePath(std::string fullPath) const {
	if(fullPath.find("?") == fullPath.npos)
		return fullPath;
	return fullPath.substr(0,fullPath.find("?"));
}
#include <fstream>
bool createFileFromBuffer(const std::string& filename, const char* buffer, std::size_t bufferSize) {
    // Open the file for writing
    std::ofstream file(filename.c_str(), std::ios::binary);
    
    // Check if the file was opened successfully
    if (!file) {
        std::cerr << "Failed to create or open the file." << std::endl;
        return false;
    }

    // Write the buffer content to the file
    file.write(buffer, bufferSize);

    // Check for write errors
    if (!file) {
        std::cerr << "Error occurred while writing to the file." << std::endl;
        return false;
    }

    // Close the file
    file.close();
    
    return true;
}