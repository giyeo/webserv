#ifndef GETPATH_HPP
#define GETPATH_HPP

#include "../Main.hpp"

typedef struct s_finalPath {
	std::string finalPath;
	std::string errorPage;
	std::string filename;
	int locationIndex;
}	t_finalPath;

class GetPath {
	public:
		GetPath();
		t_finalPath getFinalPath(Server &server, std::string uri);
		std::string getErrorPage(Server &server, int i);
		bool isValidURI(std::vector<std::string> pathTokens, std::vector<std::string> uriTokens);
		t_finalPath returnFinalPath(std::string finalPath, std::string errorPage, int i);
		t_finalPath uriIsRoot(Server &server);
		t_finalPath handleLocationRoot(Server &server, std::string uri, int i, std::vector<std::string> pathTokens, std::vector<std::string> uriTokens, std::string root);
		t_finalPath handleLocation(Server &server, std::string uri);
	private:
		std::string defaultErrorPage;
};
#endif