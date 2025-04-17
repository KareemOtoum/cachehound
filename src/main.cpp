#include "LRUCache.h"
#include "cli_debug.h"
#include "cli_server.h"
#include "chprotocol.h"
#include <iostream>

std::ostream& operator<<(std::ostream& cout, const Protocol::Buffer& buffer)
{
    for(const std::byte& byte : buffer)
    {
        cout << static_cast<int>(byte) << " ";
    }
    return cout;
}

int main()
{
    LRUCache cache{ 4 };

    startServerCLI(); 
    return 0;
}