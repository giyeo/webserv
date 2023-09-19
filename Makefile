# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -Wextra -std=c++98

# Source files and output executable
SRCS = main.cpp SocketHandler.cpp Configuration.cpp Response.cpp
TARGET = server

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)
