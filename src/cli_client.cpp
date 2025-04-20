#include "cli_client.h"

void printClientCommands();
int setupClientSocket(std::string_view ip, std::string_view port);

static Protocol::Buffer buffer;
static Protocol::Packet packet;

void startClientCLI(std::string_view ip, std::string_view port)
{
    int clientSocket {};

    if((clientSocket = setupClientSocket(ip, port)) == -1)
    {
        printError("Couldn't setup client socket");
        return;
    }

    printLogo();
    printClientCommands();

    std::cout << " | Connected to database\n";

    // commands hashmap
    // commands return true to continue and false to exit cli
    using ArgsT = const std::vector<std::string>&;
    
    std::unordered_map<std::string, std::function<bool(ArgsT)>> commandMap;

    commandMap["put"] = [&clientSocket](ArgsT args) -> bool
    {
        if(args.size() != 3)
        {
            std::cout << "Usage: put <key> <value>" << "\n";
            return true;
        }

        packet = { Protocol::PUT, args[1], args[2] };

        if(serialize(packet, buffer) == -1)
        {
            printError("Failed to serialize packet");
            return true;
        }

        send(clientSocket, buffer.data(), sizeof(buffer), 0);

        return true;
    };

    commandMap["get"] = [&clientSocket](ArgsT args) -> bool
    {
        if(args.size() != 2)
        {
            std::cout << "Usage: get <key>" << "\n";
            return true;
        }

        packet = { Protocol::GET, args[1] };

        if(serialize(packet, buffer) == -1)
        {
            printError("Failed to serialize packet");
            return true;
        }

        // send get request
        send(clientSocket, buffer.data(), sizeof(buffer), 0);
        
        // empty buffer and packet and receive packet
        bufferFactory(buffer);
        packetFactory(packet);

        ssize_t bytes { recv(clientSocket, buffer.data(), sizeof(buffer), 0) };

        if(bytes == -1)
        {
            printError("Bad response from database");
            return true;
        }

        if(deserialize(buffer, packet) == -1)
        {
            printError("Failed to deserialize buffer in response message");
            return true;
        }

        std::cout << " | " << packet.m_key.value() << "\n";

        return true;
    };

    commandMap["save"] = [&clientSocket](ArgsT args) -> bool
    {
        if(args.size() != 1)
        {
            std::cout << "Usage: save" << "\n";
            return true;
        }

        packet = { Protocol::SAVE };

        if(serialize(packet, buffer) == -1)
        {
            printError("Failed to serialize packet");
            return true;
        }

        send(clientSocket, buffer.data(), sizeof(buffer), 0);

        return true;
    };

    commandMap["load"] = [&clientSocket](ArgsT args) -> bool
    {
        if(args.size() != 1)
        {
            std::cout << "Usage: load" << "\n";
            return true;
        }

        packet = { Protocol::LOAD };

        if(serialize(packet, buffer) == -1)
        {
            printError("Failed to serialize packet");
            return true;
        }

        send(clientSocket, buffer.data(), sizeof(buffer), 0);

        return true;
    };

    commandMap["exit"] = [&clientSocket](ArgsT args) -> bool
    {
        if(args.size() != 1)
        {
            std::cout << "Usage: exit" << "\n";
            return true;
        }

        return false;
    };

    std::string input{};

    while(1)
    {
        std::cout << "> ";
        std::getline(std::cin, input);

        if(!std::cin) return;

        std::istringstream iss(input);
        std::vector<std::string> tokens;
        std::string token;

        while (iss >> token) tokens.push_back(token);
        
        if(tokens.empty()) continue;

        if(commandMap.contains(tokens[0]))
        {
            // empty buffer and packet before each commands
            bufferFactory(buffer);
            packetFactory(packet);

            if(!commandMap[tokens[0]](tokens))
            {
                break; // exit cli if functions returns false
            }
        }
        else
        {
            std::cout << "Unknown command: " << tokens[0] << "\n";
        }
    }
}

void printClientCommands()
{
    std::cout << "\t------------\n"; 
    std::cout << "\t| Commands |\n"; 
    std::cout << "\t------------\n"; 
    std::cout << "\t| put <key> <value>     - puts a key-value pair in the database\n";
    std::cout << "\t| get <key>             - gets a value from the database\n";
    std::cout << "\t| save                  - saves database to disk\n";
    std::cout << "\t| load                  - loads database from disk\n";
    std::cout << "\t| exit\n\n";
}

int setupClientSocket(std::string_view ip, std::string_view port)
{
    struct addrinfo hints, *serverinfo;
    int rv;
    
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC; // dont care about ipv4 or ipv6
    hints.ai_socktype = SOCK_STREAM; // TCP socket

    if((rv = getaddrinfo(ip.data(), port.data(), &hints, &serverinfo) != 0))
    {
        printError("Client error: getaddrinfo failed");
        return -1;
    }

    int clientSocket { socket(serverinfo->ai_family, serverinfo->ai_socktype,
       serverinfo->ai_protocol)};

    if(clientSocket == -1)
    {
        printError("Client error: client socket invalid");
        return -1;    
    } 

    if(connect(clientSocket, serverinfo->ai_addr, serverinfo->ai_addrlen) == -1)
    {
        printError("Client error: failed to connect");
        return -1;
    }

    return clientSocket;
}