#include "Worker.h"

void Worker::run()
{
    std::array<epoll_event, ServerConstants::k_maxEvents> events{};

    // add m_eventFD to epoll to signal new clients
    epoll_event ev{};
    ev.data.fd = m_eventFD;
    ev.events = EPOLLIN;
    epoll_ctl(m_epollFD, EPOLL_CTL_ADD, m_eventFD, &ev);

    while(true) // change to a running flag to shutdown server and cleanup threads
    {
        // call epoll wait handle if new clients need to be added 
        // or if a client sends a packet

        // blocks indefinitely
        int eventsReady { epoll_wait(m_epollFD, events.data(), ServerConstants::k_maxEvents, -1) };

        for(int i{}; i < eventsReady; ++i)
        {
            int fd { events[i].data.fd };

            // new client in queue
            if(fd == m_eventFD)
            {
                ServerConstants::DispatchEventT readNum {};
                read(fd, &readNum, sizeof(ServerConstants::DispatchEventT));

                // add clients to epoll instance
                std::lock_guard<std::mutex> lock(m_queueMutex);

                while(!m_socketQueue.empty())
                {
                    int clientFD { m_socketQueue.front() };
                    m_socketQueue.pop();

                    
                    epoll_event clientEvent{};
                    clientEvent.events = EPOLLIN;
                    clientEvent.data.fd = clientFD;
                    epoll_ctl(m_epollFD, EPOLL_CTL_ADD, clientFD, &clientEvent);
                    std::cout << "Worker: added clientfd " << clientFD << " to epoll instance\n";
                }
            }
            else // new packet from client
            {
                // edge triggered client handling means we need to use a while loop
                // to receive packets
                std::array<uint8_t, 256> buffer{};
                while (true) 
                {
                    ssize_t bytes { recv(fd, buffer.data(), sizeof(buffer), 0) };
                    if (bytes == -1) 
                    {
                        if (errno == EAGAIN || errno == EWOULDBLOCK) // All data has been read
                        {
                            
                            break;
                        } 
                        else // Handle error
                        {
                            
                            break;
                        }
                    } 
                    else if (bytes == 0) // Client closed connection
                    {
                        std::cout << "client fd " << fd << " disconnected\n";
                        epoll_ctl(m_epollFD, EPOLL_CTL_DEL, fd, &events[i]);
                        close(fd);

                        break;
                    }
                
                    // Process data...
                }
            }
        }
    }
}