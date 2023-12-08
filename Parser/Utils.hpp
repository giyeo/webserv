#pragma once

#include "../cpplibs.hpp"

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
