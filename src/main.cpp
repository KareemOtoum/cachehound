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

std::ostream& operator<<(std::ostream& cout, const Protocol::Packet& packet)
{
    cout << static_cast<int>(packet.m_action) << " ";
    if(packet.m_key) cout << packet.m_key.value();
    if(packet.m_value) cout << packet.m_value.value();
    return cout;
}


int main()
{
    Protocol::Packet packet {Protocol::LOAD};
    Protocol::Buffer buffer{};

    serialize(packet, buffer);
    std::cout << buffer << "\n";

    Protocol::Packet p2 {};

    deserialize(buffer, p2);

    std::cout << p2 << "\n";
    return 0;
}