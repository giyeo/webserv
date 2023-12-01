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

std::string trimString(const std::string& str);
bool contains(std::vector<std::string> vector, std::string target);
std::vector<std::string> tokenizer(std::string input, char delimiter);
bool allblank(size_t pivot, size_t location, std::string input, bool exit);
std::vector<std::string> extractAndRemoveBracesContent(std::string& input);
void error(int line, std::string des);
std::string readFile(std::string filePath);

#endif