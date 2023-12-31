#pragma once

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"

#define NOTFOUND 0
#define FOUND 1
#define REDIRECT_INDEX 2
#define MAX_EVENTS 10

enum LogLevel {
	LOG,
	WARNING,
	ERROR,
	LOGBLUE,
	FAILED
};

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cerrno>
#include <cstdio>
#include <sys/epoll.h>
#include <fcntl.h>
#include <map>
#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <vector>
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include <cstdarg>
#include <iomanip>
#include <sys/stat.h>
#include <dirent.h>
#include <arpa/inet.h>
#include <netdb.h>