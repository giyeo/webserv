#pragma once

#include "../Master.hpp"

typedef struct {
	std::map<std::string, std::string> strConf;
	std::map<std::string, int> intConf;
} Config;

class Configuration {
	public:
		Configuration();
		~Configuration();
		Configuration(char *path_to_file);
		Config &getConfig();
};
