#include "LRUCache.h"
#include "cli.h"
#include <iostream>

int main()
{
    LRUCache cache{ 4 };

    startCLI(cache);
    
    return 0;
}