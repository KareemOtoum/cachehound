#include "cli_debug.h"
#include "chprotocol.h"
#include "print_error.h"
#include <iostream>

#include <sys/socket.h>
#include <netdb.h>

void startClientCLI(std::string_view ip, std::string_view port);
int setupClientSocket(std::string_view ip, std::string_view port);
