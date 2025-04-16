#include <iostream>
#include <vector>
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

struct Worker
{
    int m_epollFD{}; // epoll instance for worker
    int m_eventFD{}; // used to wakeup worker to handle new client

    std::thread m_thread;
    std::mutex m_queueMutex{};
    std::queue<int> m_socketQueue{}; // client connections queue

    void run();
};