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
		Resource/Resource.cpp \
		Parser/Utils.cpp \
		Parser/Server.cpp\
		Parser/Location.cpp \
		Parser/Parser.cpp

TEST_SRCS =	test.cpp \
		Communication/SocketHandler.cpp \
		Configuration/Configuration.cpp \
		Response/Response.cpp \
		Request/Request.cpp \
		Resource/Resource.cpp

MINUNIT_SRCS_FILES =	minunit.c \
					Parser/Utils.cpp\
					Parser/Server.cpp\
					Parser/Location.cpp 
# Object files
OBJS = $(SRCS:.cpp=.o)
TEST_OBJS = $(TEST_SRCS:.cpp=.o)

TARGET = server
TEST_TARGET = test

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(TEST_TARGET): $(TEST_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Rule to compile source files to object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(TARGET)

minunit: $(MINUNIT_SRCS_FILES)
	$(CXX) $(CXXFLAGS) $(MINUNIT_SRCS_FILES) -o $(TEST) 
	./$(TEST)
	rm -f $(TEST)
	rm -f $(TARGET) $(TEST_TARGET) $(OBJS) $(TEST_OBJS)
