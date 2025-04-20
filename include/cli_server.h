#pragma once

#include "LRUCache.h"
#include "Worker.h"
#include "print_error.h"

#include <thread>
#include <memory>
#include <mutex>
#include <atomic>
#include <csignal>
#include <stdio.h>
#include <string>

#include <sys/socket.h>
#include <sys/types.h>

#include <fcntl.h>
#include <sys/epoll.h>

#include <sys/unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <chrono>
#include <errno.h>

namespace ServerConstants
{
    constexpr std::string_view port { "5050" };

    inline const int k_workerCount { static_cast<int>(std::thread::hardware_concurrency()) };
    constexpr int k_defaultWorkerCount { 4 };

    extern std::atomic<bool> g_running;
}

void startServerCLI(std::shared_ptr<LRUCache> cache);

void handleExit(int signal);
