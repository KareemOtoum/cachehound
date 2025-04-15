#include "LRUCache.h"
#include <iostream>

void printGet(LRUCache& cache, const std::string& key)
{
    auto value { cache.get(key) };

    std::cout << key << ": " << (value ? value.value() : "NOT FOUND") << "\n";
}

int main()
{
    { // test saving to disk
        LRUCache cache(4);

        for(int i = 0; i < 4; ++i)
        {
            cache.put(std::to_string(i), std::to_string(i+1));
            printGet(cache, std::to_string(i));
        }

        std::cout << "Saving to disk..." << "\n";
        std::cout << "-----------------" << "\n";

        cache.saveToDisk("cache.db");
    }


    { // test loading from disk

        LRUCache cache{}; 

        std::cout << "Loading from disk..." << "\n";
        std::cout << "-----------------" << "\n";

        cache.loadFromDisk("cache.db");

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