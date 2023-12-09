# Compiler
CXX = c++

# Compiler flags
CXXFLAGS = -Werror -Wall -Wextra -std=c++98 -fsanitize=address

# Source files and output executable
SRCS =	Parser/Location.cpp \
		Parser/Utils.cpp \
		Parser/Server.cpp \
		Parser/Parser.cpp \
		Communication/SocketHandler.cpp \
		Response/Response.cpp \
		Request/Request.cpp \
		Resource/GetPath.cpp \
		Config.cpp \
		Resource/Resource.cpp \
		main.cpp

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

TARGET = webserv
TEST_TARGET = test

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(TEST_TARGET): $(TEST_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Rule to compile source files to object files
%.o: %.cpp $(wildcard *.hpp)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(TARGET)

re: fclean all

fclean:
	rm -rf */*.o && rm -rf ./*.o

minunit: $(MINUNIT_SRCS_FILES)
	$(CXX) $(CXXFLAGS) $(MINUNIT_SRCS_FILES) -o $(TEST_TARGET)
	./$(TEST_TARGET)
	rm -f $(TEST_TARGET)
	rm -f $(TARGET) $(TEST_TARGET) $(OBJS) $(TEST_OBJS)
