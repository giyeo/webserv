# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -Wextra -std=c++98

# Source files and output executable
SRCS =	main.cpp \
		Communication/SocketHandler.cpp \
		Configuration/Configuration.cpp \
		Response/Response.cpp \
		Request/Request.cpp \
		Resource/Resource.cpp

TEST_SRCS =	test.cpp \
		Communication/SocketHandler.cpp \
		Configuration/Configuration.cpp \
		Response/Response.cpp \
		Request/Request.cpp \
		Resource/Resource.cpp

TARGET = server
TEST = test

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^

test: $(TEST)

$(TEST): $(TEST_SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)