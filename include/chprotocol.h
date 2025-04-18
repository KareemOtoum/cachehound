#pragma once

#include <array>
#include <bitset>
#include <optional>
#include <cstring>
#include <iostream>

// put <key> <value>
// get <value>
// save
// load

namespace Protocol
{
    // in bytes
    constexpr int k_maxStringSize { 32 }; 
    // 1 action byte | 2 bytes string sizes | 2 strings 
    constexpr int k_packetSize { 3 + k_maxStringSize * 2 };

    // only need 8 bits to represent the string size
    using StringSizeT = u_int8_t;
    using Buffer = std::array<std::byte, k_packetSize>;

    enum Action : u_int8_t
    {
        PUT,
        GET,
        SAVE,
        LOAD
    };

    struct Packet
    {
        Action m_action{};
        std::optional<std::string> m_key{};
        std::optional<std::string> m_value{};
    };
}

int serialize(const Protocol::Packet& packet, Protocol::Buffer& buffer);
int deserialize(const Protocol::Buffer& buffer, Protocol::Packet& packet);

// for printing packet, buffer and action to cout
std::ostream& operator<<(std::ostream& cout, const Protocol::Buffer& buffer);
std::ostream& operator<<(std::ostream& cout, const Protocol::Packet& packet);
std::ostream& operator<<(std::ostream& cout, const Protocol::Action& action);

