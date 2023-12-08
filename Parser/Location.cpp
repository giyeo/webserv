#include "Location.hpp"

Location::Location() {}

void Location::parseRoot(std::string value){
	std::vector<std::string> tokens = tokenizer(value, ' ');
	if(tokens.size() != 1) {
		std::cerr << "Invalid number of arguments in a \"root\" directive" << std::endl;
		exit(EXIT_FAILURE);
	}
	this->root = value;
}

void Location::parseIndex(std::string value) {
	this->index = value;
}

void Location::parseErrorPage(std::string value) {
	std::vector<std::string> tokens = tokenizer(value, ' ');

	std::string errorPage = tokens.back();
	this->errorPage = errorPage;
}

void Location::parseProxyPass(std::string value) {
	this->proxyPass = value;
}

void Location::parseMethods(std::string value) {
	this->methods = value;
}

void Location::parseAutoIndex(std::string value) {
	this->autoindex = value;
}

void Location::parseReturn(std::string value) {
	std::vector<std::string> tokens = tokenizer(value, ' ');

	if(tokens.size() != 2)
		log(__FILE__,__LINE__,"invalid return directive", ERROR);
	
	this->returnCode = tokens[0];
	this->returnPath = tokens[1];
}

typedef void (Location::*MemberFunction)(std::string);

void Location::dispatcher(std::string key, std::string value) {
	std::vector<std::string> names;
	names.push_back("root");
	names.push_back("index");
	names.push_back("error_page");
	names.push_back("proxy_pass");
	names.push_back("methods");
	names.push_back("autoindex");
	names.push_back("return");

	std::vector<MemberFunction> functionPointers;
    functionPointers.push_back(&Location::parseRoot);
    functionPointers.push_back(&Location::parseIndex);
    functionPointers.push_back(&Location::parseErrorPage);
	functionPointers.push_back(&Location::parseProxyPass);
	functionPointers.push_back(&Location::parseMethods);
	functionPointers.push_back(&Location::parseAutoIndex);
	functionPointers.push_back(&Location::parseReturn);
	
	if(names.size() != functionPointers.size()) {
		std::cout << "diferent sizes in dispatcher\n";
		return ;
	}
	for(size_t i = 0; i < names.size(); i++) {
		try {
			if(names[i].compare(key) == 0) {
				(this->*functionPointers[i])(value);
			}
		} catch (std::invalid_argument& e) {
			std::cerr << e.what() << std::endl;
		} catch (std::exception& e) {
			std::cerr << e.what() << std::endl;
		}
	}
}