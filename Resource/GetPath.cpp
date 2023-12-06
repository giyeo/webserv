#include "GetPath.hpp"

std::string GetPath::getErrorPage(Server &server, int i) {

	std::string locationErrorPage = "";
	if(server.locations.size() > 0 && i >= 0)
		locationErrorPage = server.locations[i].errorPage;
	std::string serverErrorPage = server.errorPage;
	if (locationErrorPage != "")
		return locationErrorPage;
	else if (serverErrorPage != "")
		return serverErrorPage;
	else
		return defaultErrorPage;
}

bool GetPath::isValidURI(std::vector<std::string> pathTokens, std::vector<std::string> uriTokens) {
	for (size_t i = 0; i < pathTokens.size(); i++) {
		if(pathTokens[i] != uriTokens[i])
			return false;
	}
	return true;
}

t_finalPath GetPath::returnFinalPath(std::string finalPath, std::string errorPage, int i) {
	t_finalPath res;
	res.finalPath = finalPath;
	res.errorPage = errorPage;
	res.locationIndex = i;
	res.filename = tokenizer(finalPath, '/').back();
	return res;
}

t_finalPath GetPath::uriIsRoot(Server &server) {
	std::string indx = (server.index.empty()) ? "/index.html" : '/' + server.index;
	return returnFinalPath(server.root + indx, getErrorPage(server, -1), -1);
}

t_finalPath GetPath::handleLocationRoot(Server &server, std::string uri, int i, std::vector<std::string> pathTokens, std::vector<std::string> uriTokens, std::string root) {
	std::string finalPath = root;
	if(uriTokens.size() == pathTokens.size()) {
		finalPath += server.locations[i].path + '/';
		if (server.locations[i].index == "")
			return returnFinalPath(finalPath + server.index, getErrorPage(server, i), i);
		if (server.locations[i].index != "")
			return returnFinalPath(finalPath + server.locations[i].index, getErrorPage(server, i), i);
	}
	return returnFinalPath(finalPath + uri, getErrorPage(server, i), i);
}

t_finalPath GetPath::handleLocation(Server &server, std::string uri) {
	std::vector<std::string> uriTokens = tokenizer(uri, '/');
	for (size_t i = 0; i < server.locations.size(); i++) {
		std::vector<std::string> pathTokens = tokenizer(server.locations[i].path, '/');
		if (isValidURI(pathTokens, uriTokens)) {
			if (server.locations[i].root == "")
				return handleLocationRoot(server, uri, i, pathTokens, uriTokens, server.root);
			else
				return handleLocationRoot(server, uri, i, pathTokens, uriTokens, server.locations[i].root);
		}
	}
	log(__FILE__,__LINE__,concat(5, "default root:",server.root.c_str(),uri.c_str()," Error Page:",server.errorPage.c_str()), LOG);
	return returnFinalPath(server.root + uri, getErrorPage(server, -1), -1);
}

t_finalPath GetPath::getFinalPath(Server &server, std::string uri) {
	log(__FILE__, __LINE__, concat(2,"Requested URI:",uri.c_str()), LOG);

	if(uri == "/")
		return uriIsRoot(server);
	if(uri == defaultErrorPage)
		return returnFinalPath(defaultErrorPage, "", -1);
	return handleLocation(server, uri);
}

GetPath::GetPath() {
	char buffer[1024];
	getcwd(buffer, sizeof(buffer));
	this->defaultErrorPage = buffer;
	this->defaultErrorPage += "/defaultErrorPage.html";
}