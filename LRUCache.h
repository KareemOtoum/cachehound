#pragma once

#include <unordered_map>
#include <list>
#include <string>
#include <iterator>
#include <optional>

class LRUCache
{
    using ListIt = std::list<std::string>::iterator;
    
    // least recently used entry is at the front of the list
    std::list<std::string> m_usage{};
    std::unordered_map<std::string, std::pair<std::string, ListIt>> m_map{};
    
    std::size_t m_capacity{};

    void evictLRU();
    void use(ListIt iterator);

public:
    explicit LRUCache(std::size_t size) 
        : m_capacity { size }
    {
        m_map.reserve(size);
    }

    std::optional<std::string>     get(const std::string& key);
    void                           put(const std::string& key, const std::string& value);
};