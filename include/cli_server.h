#pragma once

#include "LRUCache.h"
#include <thread>
#include <mutex>
#include <stdio.h>
#include <string>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <sys/unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <chrono>

namespace ServerConstants
{
    constexpr std::string_view port { "8080" };
}

void startServerCLI();