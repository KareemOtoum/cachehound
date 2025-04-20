#include "print_error.h"

void printError(std::string_view err)
{
    std::cerr << err << " " << strerror(errno) << "\n";
}