#include "cli_server.h"

void printError(std::string_view err);
void logNewClient(const sockaddr_storage& clientAddr);
int setupServerSocket();
void makeNonBlocking(int fd);
void dispatchClient(int clientFD, std::vector<std::unique_ptr<Worker>>& workers);

void startServerCLI() 
{
    // setup worker threads
    // Worker is not copyable or moveable because of std::mutex 
    // probably should be changed?
    std::vector<std::unique_ptr<Worker>> workers;

    // worker count is the number of logical cores or 4 if hardware_concurrency() fails 
    const int workerCount { ServerConstants::k_workerCount > 0 ? ServerConstants::k_workerCount
         : ServerConstants::k_defaultWorkerCount };
    workers.reserve(workerCount);
    
    std::cout << "Setting up workers\n";
    for(int i{}; i < workerCount; ++i)
    {
        workers.push_back(std::make_unique<Worker>());

        // run worker thread on its own run function by passing 
        // its object address to the member func as a this ptr
        Worker& worker { *workers[i] };
        worker.m_thread = std::thread(&Worker::run, &worker);
        worker.m_epollFD = epoll_create1(0);
        worker.m_eventFD = eventfd(0, EFD_NONBLOCK);
    }

    std::cout << "Setting up server\n";

    // setup server socket on main thread

    int serverSock { setupServerSocket() };
    if(serverSock == -1) 
    {
        printError("Server error: failed to setup server socket");
        return;
    }

    makeNonBlocking(serverSock);

    std::cout << "Server listening on port " << ServerConstants::port << "...\n";

    struct sockaddr_storage client_addr{};
    socklen_t sin_size{};

    while(true)
    {
        // non blocking returns immediately
        // change to use epoll to lower cpu usage
        int newClientFD { accept(serverSock, (struct sockaddr *)&client_addr, &sin_size) };

        if(newClientFD == -1)
        {
            if(errno == EAGAIN || EWOULDBLOCK) // no connections
            {
                continue;
            }
            // real connection error
            printError("Failed to accept connection from client");
            continue;
        }

        logNewClient(client_addr);
        makeNonBlocking(newClientFD);
        dispatchClient(newClientFD, workers);
    }

    close(serverSock);
}

void dispatchClient(int clientFD, std::vector<std::unique_ptr<Worker>>& workers)
{
    // round robin scheduling
    static int nextWorker { 0 };

    int i { nextWorker++ % workers.size() };

    std::cout << "dispatching client to worker " << i << "\n";

    Worker& worker { *workers[i] };
    { // add client to workers socket queue
        std::lock_guard<std::mutex> lock(worker.m_queueMutex);
        worker.m_socketQueue.push(clientFD);
    }

    // write 1 to workers eventfd to signal an added client to its queue
    ServerConstants::DispatchEventT one { 1 };
    write(worker.m_eventFD, &one, sizeof(ServerConstants::DispatchEventT));
}

void printError(std::string_view err)
{
    std::cerr << err << "\n";
}

void printLog(std::string_view msg)
{
    std::cout << msg << "\n";
}

void makeNonBlocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

void logNewClient(const sockaddr_storage& clientAddr)
{
    std::array<char, INET_ADDRSTRLEN> ip4{};
    
    const sockaddr_in* addr_in = reinterpret_cast<const sockaddr_in*>(&clientAddr);
    if (inet_ntop(AF_INET, &(addr_in->sin_addr), ip4.data(), ip4.size())) {
        std::cout << "Got connection from: " << ip4.data() << "\n";
    } else {
        std::cerr << "Failed to convert IP address\n";
    }
}

int setupServerSocket()
{
    struct addrinfo hints, *serverinfo;
    int rv;
    
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC; // dont care about ipv4 or ipv6
    hints.ai_socktype = SOCK_STREAM; // TCP socket

    if((rv = getaddrinfo("127.0.0.1", ServerConstants::port.data(), &hints, &serverinfo) != 0))
    {
        printError("Server error: getaddrinfo");
        return -1;
    }

    int serverSock { socket(serverinfo->ai_family, serverinfo->ai_socktype,
       serverinfo->ai_protocol)};

    if(serverSock == -1)
    {
        printError("Server error: server socket");
        return -1;    
    } 

    if(bind(serverSock, serverinfo->ai_addr, serverinfo->ai_addrlen) == -1)
    {
        close(serverSock);
        printError("Server error: failed to bind to server socket");
        return -1;
    }

    freeaddrinfo(serverinfo);

    if(listen(serverSock, 1) == -1)
    {
        printError("Server error: failed to listen");
        return -1;
    }

    return serverSock;
}