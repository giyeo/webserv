#include "Server.hpp"
typedef std::map<std::string, std::string> mapStr;

Server parseEachDirective(mapStr myConf, mapStr locations) {
	Server server;
	std::map<std::string, std::string>::iterator it;

	for (it = myConf.begin(); it != myConf.end(); ++it)
		server.dispatcher(it->first, trimString(it->second));
	for (it = locations.begin(); it != locations.end(); ++it)
		server.parseLocation(it->first, trimString(it->second));
	return server;
}

Server parseServerDirectives(std::string input) {
	std::vector<std::string> names = {"server_name", "listen", "root", "index", "error_page", "location", "client_max_body_size"};
	std::map<std::string, std::string> myConf;
	std::map<std::string, std::string> locations;
	std::string key;
	std::string value;
	std::vector<std::string> locationContent = extractAndRemoveBracesContent(input);
	std::vector<std::string> tokens = parseLine(input, ';');
	
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
				locations[trimString(value)] = locationContent[locations.size()];
			else
				myConf[key] = value;
		}
		else
			error(__LINE__);
	}
	return parseEachDirective(myConf, locations);
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

int main() {
	std::string input = readFile("default.conf");
	std::vector<std::map<std::string, std::vector<std::string>>> fullConfiguration;
	std::vector<std::string> parsedServers = parseServers(input);
	std::vector<Server> servers;
	
	for(int i = 0; i < parsedServers.size(); i++) {
		// std::cout << servers[i] << "\n";
		servers.push_back(parseServerDirectives(parsedServers[i]));
		std::cout << "SUCCESS!\n";
	}
}