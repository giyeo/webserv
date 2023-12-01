#ifndef MAIN_HPP
#define MAIN_HPP 
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cerrno>
#include <cstdio>
#include "Communication/SocketHandler.hpp"
#include "Configuration/Configuration.hpp"
#include "Response/Response.hpp"
#include "Request/Request.hpp"
#include <sys/epoll.h>
#include <fcntl.h>
#include "Resource/Resource.hpp"

#define MAX_EVENTS 10

#endif