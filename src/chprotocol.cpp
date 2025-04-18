#include "chprotocol.h"

int serializeString(std::string_view str, Protocol::Buffer& buffer, int index);
int deserializeString(std::optional<std::string>& str, const Protocol::Buffer& buffer, int start, int size);
 
// returns -1 on failure and 0 on sucess
// does not empty buffer on failure
int serialize(const Protocol::Packet& packet, Protocol::Buffer& buffer)
{
    // action is invalid
    if(packet.m_action < Protocol::PUT || packet.m_action > Protocol::LOAD)
    {
        return -1;
    }

    // put doesnt have key and value strings
    if(packet.m_action == Protocol::PUT && (!packet.m_key || !packet.m_value))
    {
        return -1;
    }
    // get has incorrect arguements
    else if(packet.m_action == Protocol::GET && (!packet.m_key || packet.m_value))
    {
        return -1;
    }
    // other commands shouldnt have arguements
    else if((packet.m_action == Protocol::LOAD || packet.m_action == Protocol::SAVE) && 
        (packet.m_key || packet.m_value))
    {
        return -1;
    }
    
    // set action in the first byte
    buffer[0] = static_cast<std::byte>(packet.m_action);

    if(packet.m_key)
    {
        // write key string at index 1 of buffer
        if(serializeString(packet.m_key.value(), buffer, 1) == -1) return -1;

        // there can only be a value str if there is a key str
        if(packet.m_value)
        {
            // write the value string at index (key string size) + 2
            if(serializeString(packet.m_value.value(), buffer, 
                packet.m_key.value().size() + 2) == -1) return -1;
        }
    }

    return 0;
}

// returns -1 on failure and 0 on sucess
int deserialize(const Protocol::Buffer& buffer, Protocol::Packet& packet)
{
    if(buffer.size() != Protocol::k_packetSize)
    {
        return -1;
    }

    packet.m_action = static_cast<Protocol::Action>(buffer[0]);

    if(packet.m_action < Protocol::PUT || packet.m_action > Protocol::LOAD)
    {
        return -1;
    }

    // these shouldnt have arguements
    if(packet.m_action == Protocol::LOAD || packet.m_action == Protocol::SAVE)
    {
        return 0;
    }

    int deserializeIndex { 1 };
    Protocol::StringSizeT keyStrSize { static_cast<Protocol::StringSizeT>(buffer[deserializeIndex]) };
    ++deserializeIndex;

    if(keyStrSize > Protocol::k_maxStringSize) 
    {
        return -1;
    }

    if(deserializeString(packet.m_key, buffer, deserializeIndex, keyStrSize) == -1)
    {
        return -1;
    }
    deserializeIndex += keyStrSize;

    // get doesnt have value str
    if(packet.m_action == Protocol::GET)
    {
        return 0;
    }

    Protocol::StringSizeT valueStrSize { static_cast<Protocol::StringSizeT>(buffer[deserializeIndex]) };
    ++deserializeIndex;
    
    if(valueStrSize > Protocol::k_maxStringSize)
    {
        return -1;
    }

    if(deserializeString(packet.m_value, buffer, deserializeIndex, valueStrSize) == -1)
    {
        return -1;
    }

    return 0;
}

int serializeString(std::string_view str, Protocol::Buffer& buffer, int index)
{
    if(str.size() > Protocol::k_maxStringSize || str.size() == 0)
    {
        return -1;
    }
        
    // write the strings size as an unsigned 8 bit int and the 
    // string after it
    Protocol::StringSizeT size { static_cast<Protocol::StringSizeT>(str.size()) };
    buffer[index] = static_cast<std::byte>(size);
    std::memcpy(&buffer[index + 1], str.data(), size);

    return 0;
}

int deserializeString(std::optional<std::string>& str, const Protocol::Buffer& buffer, int start, int size)
{
    str = std::string(reinterpret_cast<const char*>(&buffer[start]), size);
    if(str && (str.value().size() > Protocol::k_maxStringSize || str.value().size() == 0)) 
    {
        str = std::nullopt;
        return -1;
    }
    return 0;
}

void bufferFactory(Protocol::Buffer& buffer)
{
    std::memset(buffer.data(), 0, sizeof(buffer));
}

void packetFactory(Protocol::Packet& packet)
{
    packet = {};
}

std::ostream& operator<<(std::ostream& cout, const Protocol::Buffer& buffer)
{
    for(const std::byte& byte : buffer)
    {
        cout << static_cast<int>(byte) << " ";
    }
    return cout;
}

std::ostream& operator<<(std::ostream& cout, const Protocol::Action& action)
{
    std::string_view str {};

    switch (action)
    {
    case Protocol::PUT:
        str = "PUT";
        break;
    case Protocol::GET:
        str = "GET";
        break;
    case Protocol::SAVE:
        str = "SAVE";
        break;
    case Protocol::LOAD:
        str = "LOAD";
        break;
    default:
        str = "NULL";
        break;
    }

    cout << str;
    return cout;
}

std::ostream& operator<<(std::ostream& cout, const Protocol::Packet& packet)
{
    cout << packet.m_action << " ";
    if(packet.m_key) cout << packet.m_key.value() << " ";
    if(packet.m_value) cout << packet.m_value.value();
    return cout;
}