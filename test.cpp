#include <cassert>
#include "Request/Request.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include "Utils.hpp"
#define GREEN "\033[1;32m"
#define RESET "\033[0m"
typedef std::string (*FunctionPointer)();



std::string readFile(std::string filePath);
//Maybe create my own assert that uses __LINE__ to get the line number.
std::string simpleGetParserTest() {
    std::string file = readFile(__FUNCTION__);
    assert(!file.empty());
    Request req(file.c_str());
    assert(req.getMethod() == "GET");
    assert(req.getPath() == "/index.html");
    return __FUNCTION__;
}

std::string getWithPathVariables() {
    std::string file = readFile(__FUNCTION__);
    assert(!file.empty());
    Request req(file.c_str());
    assert(req.getMethod() == "GET");
    assert(req.getPath() == "/index.html");
    assert(!req.getPathVariables()["name"].compare("caio"));
    assert(!req.getPathVariables()["age"].compare("40"));
    return __FUNCTION__;
}

std::string getWithSomeHeaders() {
    std::string file = readFile(__FUNCTION__);
    assert(!file.empty());
    Request req(file.c_str());
    assert(req.getMethod() == "GET");
    assert(req.getPath() == "/index.html");
    assert(!req.getHeaders()["Authorization"].compare("Bearer"));
    assert(!req.getHeaders()["Content-Type"].compare("text for test"));
    return __FUNCTION__;
}
//may be useless
std::string postWithFormData() {
    std::string file = readFile(__FUNCTION__);
    assert(!file.empty());
    Request req(file.c_str());
    assert(req.getMethod() == "POST");
    assert(req.getPath() == "/index.html");
    assert(!req.getFormDataBoundary().compare("X-INSOMNIA-BOUNDARY"));
    return __FUNCTION__;
}

int main() {
    FunctionPointer f[] = {
        simpleGetParserTest,
        getWithPathVariables,
        getWithSomeHeaders
    };
    
    for (unsigned long i = 0; i < sizeof(f) / sizeof(f[0]); ++i) {
        std::string funcName = f[i](); // Call the function at index i
        std::cout << GREEN << "✅ PASSED " << RESET << funcName << std::endl;
    }
    std::cout << "✅ All tests passed\n";
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