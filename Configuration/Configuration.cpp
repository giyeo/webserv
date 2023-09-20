#include "Configuration.hpp"

Configuration::Configuration() {

}

Configuration::Configuration(char *path_to_file) {
	(void)path_to_file;
}

Configuration::~Configuration() {

}

Config &Configuration::getConfig() {
	Config *config = new Config;
	return *config;
}
