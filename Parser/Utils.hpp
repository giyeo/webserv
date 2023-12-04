#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <sstream>
#include <map>
#include <algorithm>
#include <fstream>
#include <stdexcept>
#include <cstdarg>
#include <iomanip>

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"

enum LogLevel {
    LOG,
    WARNING,
    ERROR,
	LOGBLUE,
	FAILED
};

std::string trimString(const std::string& str);
bool contains(std::vector<std::string> vector, std::string target);
std::vector<std::string> tokenizer(std::string input, char delimiter);
bool allblank(size_t pivot, size_t location, std::string input, bool exit);
std::vector<std::string> extractAndRemoveBracesContent(std::string& input);
void error(int line, std::string des);
std::string readFile(std::string filePath);
std::vector<std::string> myPushBack(int count, ...);
void printVector(std::vector<std::string> vector);

void log(const char *file, int line, const char *description, LogLevel level);
const char* concat(int count, ...);
std::string intToString(int value);

#endif