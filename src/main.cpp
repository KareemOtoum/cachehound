#include "LRUCache.h"
#include <iostream>

void printGet(LRUCache& cache, const std::string& key)
{
    auto value { cache.get(key) };

    std::cout << key << ": " << (value ? value.value() : "NOT FOUND") << "\n";
}

int main()
{
    LRUCache cache(2);

    cache.put("bob", "20");
    cache.put("kareem", "18");

    cache.get("bob"); 

    cache.put("elvis", "33"); // kareem should be evicted

    printGet(cache, "bob");
    printGet(cache, "kareem");
    printGet(cache, "elvis");

    return 0;
}