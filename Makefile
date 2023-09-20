# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -Wextra -std=c++98

# Source files and output executable
SRCS =	main.cpp \
		Communication/SocketHandler.cpp \
		Configuration/Configuration.cpp \
		Response/Response.cpp \
		Request/Request.cpp
TARGET = server

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)
