#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <sstream>
#include <map>
#include <algorithm>
#include <regex>
#include <fstream>

std::string trimString(const std::string& str);
bool contains(std::vector<std::string> vector, std::string target);
std::vector<std::string> parseLine(std::string input, char delimiter);
void allblank(size_t pivot, size_t location, std::string input);
std::vector<std::string> extractAndRemoveBracesContent(std::string& input);
void error(int line);
std::string readFile(std::string filePath);

#endif