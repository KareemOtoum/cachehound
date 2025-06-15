#pragma once

#include "chprotocol.h"
#include "cli_server.h"
#include "LRUCache.h"

#include <memory>
#include <unordered_set>
#include <iostream>
#include <vector>
#include <array>
#include <thread>
#include <mutex>
#include <queue>
#include <atomic>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>

// macro to enable logging 
//#define ENABLE_WORKER_LOGGING

#ifdef ENABLE_WORKER_LOGGING
    #define LOG(msg) do { std::cout << "[Worker]: " << msg << "\n"; } while(0)
#else 
    #define LOG(msg) do {} while(0)
#endif

namespace ServerConstants
{
    constexpr int k_maxEvents { 64 };
    using DispatchEventT = uint64_t;
}

struct Worker
{
    int m_epollFD{}; // epoll instance for worker
    int m_eventFD{}; // used to wakeup worker to handle new client

    std::thread m_thread{};
    std::mutex m_queueMutex{};
    std::queue<int> m_socketQueue{}; // client connections queue
    std::unordered_set<int> m_clientSockets;

    void run(LRUCache& cache);
    void cleanup();
};