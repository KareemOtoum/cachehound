#include "LRUCache.h"
#include "cli_debug.h"
#include "cli_server.h"
#include "cli_client.h"
#include "chprotocol.h"
#include <iostream>
#include <string.h>

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        std::cout << "Usage: make <client/server> <arguements>\n";
        return -1;
    }

    if(std::strncmp(argv[1], "server", 7) == 0)
    {
        size_t capacity { CacheConstants::defaultCapacity };

        if(argc == 3) // if capacity is specified
        {
            try
            {
                size_t parsed { static_cast<size_t>(std::stoi(argv[2])) };
                if(parsed < 1 || parsed > CacheConstants::maxCapacity)
                {
                    std::cerr << "Invalid capacity using default: " << 
                        CacheConstants::defaultCapacity << "\n";
                }
                else
                {
                    capacity = parsed;
                }
            }
            catch(const std::exception& e)
            {
                std::cerr << "invalid parameters: " << e.what() << '\n';
            }
        } 

        LRUCache cache(capacity);

        startServerCLI(cache);
    } 
    else if(std::strncmp(argv[1], "client", 7) == 0)
    {
        std::string_view ip { "127.0.0.1" };
        std::string_view port { "5050" };

        if(argc == 4)
        {
            ip = argv[2];
            port = argv[3];
        }

        startClientCLI(ip, port);
    }
    
    return 0;
}