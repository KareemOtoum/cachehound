#include "cli_server.h"

void printError(std::string_view err);
int setupServerSocket();

void startServerCLI() 
{
    struct sockaddr_storage client_addr;
    socklen_t sin_size;

    int serverSock { setupServerSocket() };

    std::cout << "Server listening on port " << ServerConstants::port << "...\n";

    serverSock = accept(serverSock, (struct sockaddr *)&client_addr, &sin_size);
    if(serverSock == -1)
    {
        printError("Server error: failed to accept client");
        return;
    }
    
    char ip4[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, (struct sockaddr_in *)&client_addr, ip4, INET_ADDRSTRLEN);
    printf("Got connection from: %s!\n", ip4);

    char response[] = "HTTP/1.1 200 OK\n"
    "Content-Type: text/html\n"
    "Content-Length: 21\n"
    "\n"
    "<h1>Hello World!</h1>";


    if(send(serverSock, response, sizeof(response), 0) == -1)
    {
        perror("Server: send");
        exit(1);
    }
    printf("Sent Hello World! closing down...\n");
    
    sleep(5);

    close(serverSock);
}

void printError(std::string_view err)
{
    std::cerr << err << "\n";
}

int setupServerSocket()
{
    struct addrinfo hints, *serverinfo;
    int rv;
    
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC; // dont care about ipv4 or ipv6
    hints.ai_socktype = SOCK_STREAM; // TCP socket

    if((rv = getaddrinfo("127.0.0.1", ServerConstants::port.data(), &hints, &serverinfo) != 0))
    {
        printError("Server error: getaddrinfo");
        return -1;
    }

    int serverSock { socket(serverinfo->ai_family, serverinfo->ai_socktype,
       serverinfo->ai_protocol)};

    if(serverSock == -1)
    {
        printError("Server error: server socket");
        return -1;    
    } 

    if(bind(serverSock, serverinfo->ai_addr, serverinfo->ai_addrlen) == -1)
    {
        close(serverSock);
        printError("Server error: failed to bind to server socket");
        return -1;
    }

    freeaddrinfo(serverinfo);

    if(listen(serverSock, 1) == -1)
    {
        printError("Server error: failed to listen");
        return -1;
    }

    return serverSock;
}