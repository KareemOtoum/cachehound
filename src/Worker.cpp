#include "Worker.h"

void Worker::run(std::shared_ptr<LRUCache> cache)
{
    std::array<epoll_event, ServerConstants::k_maxEvents> events{};

    // add m_eventFD to epoll to signal new clients
    epoll_event ev{};
    ev.data.fd = m_eventFD;
    ev.events = EPOLLIN;
    epoll_ctl(m_epollFD, EPOLL_CTL_ADD, m_eventFD, &ev);

    while(ServerConstants::g_running.load()) 
    {
        // call epoll wait handle if new clients need to be added 
        // or if a client sends a packet

        int eventsReady { epoll_wait(m_epollFD, events.data(), ServerConstants::k_maxEvents, 100) };

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

                    m_clientSockets.insert(clientFD);

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

                Protocol::Buffer buffer{};

                ssize_t bytes { recv(fd, buffer.data(), sizeof(buffer), 0) };

                if (bytes == -1) 
                {
                    m_clientSockets.erase(fd);
                    epoll_ctl(m_epollFD, EPOLL_CTL_DEL, fd, &events[i]);
                    close(fd);
                    continue;
                } 
                else if (bytes == 0) // Client closed connection
                {
                    std::cout << "client fd " << fd << " disconnected\n";
                    m_clientSockets.erase(fd);
                    epoll_ctl(m_epollFD, EPOLL_CTL_DEL, fd, &events[i]);
                    close(fd);
                    continue;
                }
                
                // Process data...
                Protocol::Packet packet{};
                if(deserialize(buffer, packet) == -1)
                {
                    std::cerr << "Couldn't deserialize buffer from client\n";
                    continue;
                }

                std::optional<std::string> val {};
                std::lock_guard<std::mutex> lock(cache->m_cacheMutex);

                switch (packet.m_action)
                {
                        
                case Protocol::PUT:
                    if (!packet.m_key || !packet.m_value) {
                        std::cerr << "PUT packet missing key or value. Crashing avoided.\n";
                        continue;
                    }
                    cache->put(packet.m_key.value(), packet.m_value.value());
                    std::cout << "put " << packet.m_key.value() << " into database\n";
                    break;
                    
                case Protocol::GET:
                    val = { cache->get(packet.m_key.value()) };
                    packet.m_key = val ? val.value() : "NOT FOUND";
                    bufferFactory(buffer);
                    serialize(packet, buffer);
                    send(fd, buffer.data(), sizeof(buffer), 0);
                        
                    std::cout << "sent value " << packet.m_key.value() << "\n";
                    break;

                case Protocol::SAVE:
                    std::cout << "Saving to disk...\n";
                    cache->saveToDisk();
                    break;

                case Protocol::LOAD:
                    std::cout << "Loading from disk...\n";
                    cache->loadFromDisk();
                    break;

                default:
                    break;
                }
            }
        }
    }
    
    cleanup();
}

void Worker::cleanup()
{
    for(int fd : m_clientSockets)
    {
        close(fd);
    }
    
    close(m_eventFD);
    close(m_epollFD);
}