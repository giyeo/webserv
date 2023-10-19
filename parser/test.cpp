#include <string>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <sstream>
#include <map>
#include <algorithm>
#include <regex>
#include "Server.hpp"
#include <fstream>

typedef std::map<std::string, std::string> mapStr;

void error(int line) {
	std::cout << "line:" << line << " Error\n";
	exit(1);
}

std::vector<std::string> extractAndRemoveBracesContent(std::string& input) {
    std::vector<std::string> extractedContent;
    std::regex pattern("\\{[^{}]*\\}");

    // Search for and iterate through all matches
    std::sregex_iterator it(input.begin(), input.end(), pattern);
    std::sregex_iterator end;

    while (it != end) {
        std::smatch match = *it;
        extractedContent.push_back(match.str()); // Add the matched content to the vector
        it++;
    }

    // Remove the matched content from the input string
    input = std::regex_replace(input, pattern, "");

    return extractedContent;
}

//Check if string has anything other than blank
void allblank(size_t pivot, size_t location, std::string input) {

	while(pivot < location) {
		if(input[pivot] != ' ' && input[pivot] != '\t' && input[pivot] != '\n' && input[pivot] != '\r') 
			error(__LINE__);
		pivot++;
	}
}

std::vector<std::string> parseServers(std::string input) {
	std::vector<std::string> servers;
	size_t pivot = 0;
	size_t openbrack = 0;
	size_t closebrack = 0;
	size_t location = 0;

	location = input.find("server");
	if(location == std::string::npos)
		error(__LINE__);
	while(location != std::string::npos) {
		allblank(pivot, location, input);
		pivot = location + 6;
		location = input.find("[");
		if(location == std::string::npos )
			error(__LINE__);
		openbrack = location;
		allblank(pivot, location, input);
		location = input.find("]");
		if(location == std::string::npos)
			error(__LINE__);
		closebrack = location;
		pivot = closebrack + 1;
		servers.push_back(input.substr(openbrack + 1, closebrack - openbrack - 1));
		input = input.substr(pivot, input.length());
		pivot = 0;
		location = input.find("server");
	}
	allblank(pivot, input.length(), input);
	return servers;
}

std::vector<std::string> parseLine(std::string input, char delimiter) {
	std::vector<std::string> tokens;
	std::istringstream tokenStream(input);
	std::string token;
	
	while (std::getline(tokenStream, token, delimiter))
		tokens.push_back(token);
	return tokens;
}

bool contains(std::vector<std::string> vector, std::string target) {
	if (std::find(vector.begin(), vector.end(), target) != vector.end()) 
       	return true;
	return false;
}

std::string trimString(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r{}"); // Find the first non-whitespace character
    size_t last = str.find_last_not_of(" \t\n\r{}");   // Find the last non-whitespace character

    // Check if the string contains only whitespace
    if (first == std::string::npos) {
        return "";
    }

    // Extract the substring containing non-whitespace characters
    return str.substr(first, last - first + 1);
}

Server parseDirectives(mapStr myConf, mapStr locations) {
	Server server;
	std::map<std::string, std::string>::iterator it;
	for (it = myConf.begin(); it != myConf.end(); ++it) {
		const std::string& key = it->first;
		const std::string& value = trimString(it->second);

		if(!key.compare("server_name"))
			server.parseServerName(value);
		if(!key.compare("listen"))
			server.parseListen(value);
		if(!key.compare("root"))
			server.parseRoot(value);
		if(!key.compare("index"))
			server.parseIndex(value);
		if(!key.compare("error_page"))
			server.parseErrorPage(value);
		if(!key.compare("client_max_body_size"))
			server.parseClientMaxBodySize(value);
	}
	for (it = locations.begin(); it != locations.end(); ++it) {
		std::vector<std::string> locationDirectives;
		const std::string& key = it->first;
		const std::string& value = trimString(it->second);

		server.parseLocation(key, value);
	}
	return server;
}

Server parserServerConf(std::string input) {
	std::map<std::string, std::string> myConf;
	std::map<std::string, std::string> locations;
	std::vector<std::string> locationContent = extractAndRemoveBracesContent(input);

	std::string key;
	std::string value;

	std::vector<std::string> tokens = parseLine(input, ';');//has each line;
	std::vector<std::string> names = {"server_name", "listen", "root", "index", "error_page", "location", "client_max_body_size"};
	
	for (const std::string& str : tokens) {
		std::vector<std::string> configuration = parseLine(str, ':');
		if(configuration.size() == 2) {
			key = trimString(configuration[0]);
			value = configuration[1];
		}
		else if(configuration.size() == 3 && trimString(configuration[0]).compare("listen") == 0) {
			key = trimString(configuration[0]);
			value = configuration[1] + ":" + configuration[2];
		}
		else if(configuration.size() == 1 && trimString(configuration[0]).empty())
			continue;
		else
			error(__LINE__);
		if(contains(names, key)) {
			if(key.compare("location") == 0)
				locations[value] = locationContent[locations.size()];
			else
				myConf[key] = value;
		}
		else
			error(__LINE__);
	}
	return parseDirectives(myConf, locations);
}

std::string readFile(std::string filePath) {
	// Create an input file stream
	std::string fullPath = filePath;
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
	return res;
}

int main() {
	// std::string input = "         server [server_name: myserv  ;listen: 114;]        server[server_name:a;]";
	std::string input = readFile("default.conf");
	std::vector<std::map<std::string, std::vector<std::string>>> fullConfiguration;
	std::vector<std::string> parsedServers = parseServers(input);
	std::vector<Server> servers;
	
	for(int i = 0; i < parsedServers.size(); i++) {
		// std::cout << servers[i] << "\n";
		servers.push_back(parserServerConf(parsedServers[i]));
		std::cout << "SUCCESS!\n";
	}
}