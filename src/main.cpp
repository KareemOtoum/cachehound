#include "LRUCache.h"
#include "cli_debug.h"
#include "cli_server.h"
#include "cli_client.h"
#include "chprotocol.h"
#include <iostream>
#include <string.h>

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        std::cout << "cacheound client/server\n";
        return -1;
    }

    if(std::strncmp(argv[1], "server", 7) == 0)
    {
        LRUCache cache(4);
        startServerCLI(cache);
    } 
    else if(std::strncmp(argv[1], "client", 7) == 0)
    {
        startClientCLI("127.0.0.1", "8080");
    }
    
    return 0;
}