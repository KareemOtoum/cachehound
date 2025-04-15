#include "LRUCache.h"
#include <iostream>

void printGet(LRUCache& cache, const std::string& key)
{
    auto value { cache.get(key) };

    std::cout << "| "<< key << ": " << (value ? value.value() : "NOT FOUND") << "\n";
}

int main()
{
    std::string logo { R"(
                _          _                           _ 
  ___ __ _  ___| |__   ___| |__   ___  _   _ _ __   __| |
 / __/ _` |/ __| '_ \ / _ \ '_ \ / _ \| | | | '_ \ / _` |
| (_| (_| | (__| | | |  __/ | | | (_) | |_| | | | | (_| |
 \___\__,_|\___|_| |_|\___|_| |_|\___/ \__,_|_| |_|\__,_|
                                                         
    )"};

    std::cout << logo << "\n----------------------------------------------------------\n\n\n";

    { // test saving to disk
        LRUCache cache(4);

        for(int i = 0; i < 4; ++i)
        {
            cache.put(std::to_string(i), std::to_string(i+1));
            printGet(cache, std::to_string(i));
        }

        std::cout << "Saving to disk..." << "\n";
        std::cout << "-----------------" << "\n";

        cache.saveToDisk(Persistence::cacheFile);
    }


    { // test loading from disk

        LRUCache cache{}; 

        std::cout << "Loading from disk..." << "\n";
        std::cout << "-----------------" << "\n";

        cache.loadFromDisk(Persistence::cacheFile);

        for (int i = 0; i < 4; i++)
        {
            printGet(cache, std::to_string(i));
        }

        std::cout << "-----------------" << "\n";
        std::cout << "Testing LRU usage list correctness" << "\n";
        std::cout << "-----------------" << "\n";
        
        cache.put("kareem", "18");
        cache.put("bob", "33");
    
        for (int i = 0; i < 4; i++)
        {
            printGet(cache, std::to_string(i));
        }

        printGet(cache, "kareem");
        printGet(cache, "bob");

    }
    
    return 0;
}