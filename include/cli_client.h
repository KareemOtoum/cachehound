#include "cli_debug.h"
#include "cli_server.h"
#include "chprotocol.h"
#include <iostream>

#include <sys/socket.h>
#include <sys/types.h>

#include <sys/unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

void startClientCLI(std::string_view ip, std::string_view port);