#include <cassert>
#include "Request/Request.hpp"
#include <iostream>
#include <fstream>
#include <string>

std::string readFile(std::string filePath);

void simpleGetParserTest() {
    std::string file = readFile("simpleGetParserTest");
    assert(!file.empty());
    Request req(file.c_str());
    assert(req.getMethod() == "GET");
    assert(req.getPath() == "/index.html");
}

int main() {
    simpleGetParserTest();
    std::cout << "all tests passed\n";
}

std::string readFile(std::string filePath) {
	// Create an input file stream
	std::string fullPath = "./_TestFiles/" + filePath;
	std::ifstream inputFile(fullPath.c_str(), std::ifstream::in);

    // Check if the file was successfully opened
    if (!inputFile.is_open()) {
        std::cerr << "Failed to open file: " << fullPath << std::endl;
        return "";
    }

    // Read and print the content of the file line by line
	std::string res;
    std::string line;
    while (std::getline(inputFile, line)) {
		res += line;
        res += "\n";
    }

    // Close the file
    inputFile.close();
	return res;
}