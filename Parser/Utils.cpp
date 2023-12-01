#include "Utils.hpp"

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

// std::vector<std::string> extractAndRemoveBracesContent(std::string& input) {
//     std::vector<std::string> extractedContent;
//     std::regex pattern("\\{[^{}]*\\}");

//     // Search for and iterate through all matches
//     std::sregex_iterator it(input.begin(), input.end(), pattern);
//     std::sregex_iterator end;

//     while (it != end) {
//         std::smatch match = *it;
//         extractedContent.push_back(match.str()); // Add the matched content to the vector
//         it++;
//     }

//     // Remove the matched content from the input string
//     input = std::regex_replace(input, pattern, "");

//     return extractedContent;
// }

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