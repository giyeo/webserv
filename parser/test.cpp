#include <string>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <sstream>
#include <map>
#include <algorithm>
#include <regex>

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

void parseDirectives(mapStr myConf, mapStr locations) {
	std::vector<std::map<std::string, std::vector<std::string>>> result;
	std::map<std::string, std::vector<std::string>> temp;

	std::map<std::string, std::string>::iterator it;
	for (it = myConf.begin(); it != myConf.end(); ++it) {
		const std::string& key = it->first;
		const std::string& value = trimString(it->second);
		// temp[key] = functionCall()

		if(!key.compare("server_name"))
			std::cout << value << "\n";
		if(!key.compare("listen"))
			std::cout << value << "\n";
		if(!key.compare("root"))
			std::cout << value << "\n";
		if(!key.compare("index"))
			std::cout << value << "\n";
		if(!key.compare("error_page"))
			std::cout << value << "\n";
		if(!key.compare("location"))
			std::cout << value << "\n";
		if(!key.compare("client_max_body_size"))
			std::cout << value << "\n";
		//result.push_back(temp);
	}
	for (it = locations.begin(); it != locations.end(); ++it) {
		// temp[key] = functionCall()
		std::vector<std::string> locationDirectives;

		const std::string& key = it->first;
		const std::string& value = trimString(it->second);
		locationDirectives = parseLine(value, ';');
		for(const auto &d: locationDirectives) {
			std::cout << key << " " << trimString(d) << "\n";
		}
		//result.push_back(temp);
	}
}

std::map<std::string, std::string> parserServerConf(std::string input) {
	
	std::map<std::string, std::string> myConf;
	std::map<std::string, std::string> locations;
	std::vector<std::string> locationContent = extractAndRemoveBracesContent(input);

	std::string key;
	std::string value;

	std::vector<std::string> tokens = parseLine(input, ';');//has each line;
	std::vector<std::string> names = {"server_name", "listen", "root", "index", "error_page", "location", "client_max_body_size"};
	std::vector<std::string> locationNames = {"try_files", "root", "error_page", "fastcgi_pass"};
	
	for (const std::string& str : tokens) {
		std::vector<std::string> configuration = parseLine(str, ':');
		if(configuration.size() == 2) {
			key = trimString(configuration[0]);
			value = configuration[1];
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
	parseDirectives(myConf, locations);
	return myConf;
}

int main() {
	// std::string input = "         server [server_name: myserv  ;listen: 114;]        server[server_name:a;]";
	std::string input = "server [\
		listen: 80;\
		server_name: example.com www.example.com;\
		\
		root: /var/www/html;\
		index: index.html;\
		error_page: 404 /404.html;\
		location: / {\
			proxy_pass: http://backend-server; \
			try_files: $uri $uri/ =404; \
			}; \
		location: /s {\
			proxy_pass: http://backend-server; \
			try_files: $uri $uri/ =404; \
			}; \
	]";
	
	std::vector<std::string> servers = parseServers(input);
	std::vector<std::map<std::string, std::string>> serversConf;
	for(int i = 0; i < servers.size(); i++) {
		// std::cout << servers[i] << "\n";
		serversConf.push_back(parserServerConf(servers[i]));
		std::cout << "SUCCESS!\n";
	}
}