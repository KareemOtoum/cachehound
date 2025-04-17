#include "chprotocol.h"

int serializeString(std::string_view str, Protocol::Buffer& buffer, int index);

// returns -1 on failure and 0 on sucess
// does not empty buffer on failure
int serialize(const Protocol::Packet& packet, Protocol::Buffer& buffer)
{
    std::cout << "checking action\n";
    // action is invalid
    if(packet.m_action < Protocol::PUT || packet.m_action > Protocol::LOAD)
    {
        return -1;
    }
    std::cout << "checking correct strings for put and get\n";
    

    std::cout << "checking no strings for other commands\n";
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
        std::cout << "checking strings and lengths\n";

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
    (void)buffer;
    (void)packet;
    return 0;
}


int serializeString(std::string_view str, Protocol::Buffer& buffer, int index)
{
    if(str.size() > Protocol::k_maxStringSize)
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
