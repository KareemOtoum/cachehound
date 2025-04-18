#include "LRUCache.h"
#include "cli_debug.h"
#include "cli_server.h"
#include "chprotocol.h"
#include <iostream>

int main()
{
    Protocol::Packet packet { Protocol::PUT , "hello", "WORLD"};
    Protocol::Buffer buffer{};

    serialize(packet, buffer);
    std::cout << buffer << "\n";

    Protocol::Packet p2 {};

    deserialize(buffer, p2);

    std::cout << p2 << "\n";
    return 0;
}