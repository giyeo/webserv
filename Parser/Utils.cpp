#include "Utils.hpp"

std::string final;

#include <iostream>
#include <sstream>

std::string intToString(int value) {
    std::stringstream ss;
    ss << value;
    return ss.str();
}

const char* concat(int count, ...) {
	// Initialize va_list
	final = "";
	va_list args;
	va_start(args, count);

	// Process each argument
	for (int i = 0; i < count; ++i) {
		// Access the current argument using va_arg
		final.append(va_arg(args, const char*));
	}
	// Clean up va_list
	va_end(args);
	return final.c_str();
}

void log(const char *file, int line, const char *description, LogLevel level) {
    std::string colorCode;
    
    // Choose color based on log level
    switch (level) {
        case LOG:
            colorCode = GREEN;
            break;
        case WARNING:
            colorCode = YELLOW;
            break;
        case ERROR:
            colorCode = RED;
            break;
		case FAILED:
			colorCode = RED;
			break;
		case LOGBLUE:
            colorCode = BLUE;
            break;
        default:
            colorCode = RESET;
            break;
    }
    // Print log with color
	std::string fileAndLine;
	fileAndLine.append(file);
	fileAndLine.append(":");
	fileAndLine.append(intToString(line));
    std::cout << colorCode << std::setw(30) << std::left <<  fileAndLine << " | " << description << RESET << "\n";
	description = "";
	if(level == ERROR)
		exit(1);
}

void error(int line, std::string des) {
	std::cout << "Error! line:" << line << " " << des << "\n";
	exit(1);
}

std::vector<std::string> tokenizer(std::string input, char delimiter) {
	std::vector<std::string> tokens;
	std::istringstream tokenStream(input);
	std::string token;
	
	while (std::getline(tokenStream, token, delimiter)) {
		if (allblank(0, token.size(), token, false))
			continue;
		tokens.push_back(token);
	}
	return tokens;
}

bool contains(std::vector<std::string> vector, std::string target) {
	if (std::find(vector.begin(), vector.end(), target) != vector.end()) 
       	return true;
	return false;
}

std::string trimString(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r"); // Find the first non-whitespace character
    size_t last = str.find_last_not_of(" \t\n\r");   // Find the last non-whitespace character

    if (first == std::string::npos) {
        return "";
    }

    return str.substr(first, last - first + 1);
}

std::vector<std::string> extractAndRemoveBracesContent(std::string& input) {
    std::vector<std::string> extractedContent;

    size_t posStart, posEnd;
    while ((posStart = input.find('{')) != std::string::npos) {
        posEnd = input.find('}');
        if (posEnd == std::string::npos) {
            // Handle the case where a closing brace is not found
            break;
        }

        // Extract the content between curly braces
        std::string content = input.substr(posStart + 1, posEnd - posStart - 1);
        extractedContent.push_back(content);

        // Remove the content (including braces) from the input string
        input.erase(posStart, posEnd - posStart + 1);
    }

    return extractedContent;
}
//Check if string has anything other than blank
bool allblank(size_t pivot, size_t location, std::string input, bool exit) {

	while(pivot < location) {
		if(input[pivot] != ' ' && input[pivot] != '\t' && input[pivot] != '\n' && input[pivot] != '\r') {
			if(exit)
				error(__LINE__, "unexpected token has been found");
			return false;
		}
		pivot++;
	}
	return true;
}

std::string readFile(std::string filePath) {
	// Create an input file stream
	std::string fullPath = filePath;
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

std::vector<std::string> myPushBack(int count, ...) {
	// Initialize va_list
	std::vector<std::string> names;
	va_list args;
	va_start(args, count);

	// Process each argument
	for (int i = 0; i < count; ++i) {
		// Access the current argument using va_arg
		const char* str = va_arg(args, const char*);
		names.push_back(str);
	}
	// Clean up va_list
	va_end(args);
	return names;
}

void printVector(std::vector<std::string> vec) {
	for (size_t i = 0; i < vec.size(); i++) {
		std::cout << i << ' ' << vec[i] << std::endl;
	}
}