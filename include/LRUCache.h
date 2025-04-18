
#pragma once

#include <unordered_map>
#include <list>
#include <string>
#include <iterator>
#include <optional>
#include <fstream>
#include <iostream>
#include <cstring>
#include <thread>
#include <mutex>

namespace Persistence
{
    constexpr std::string_view cacheFileHeader { "CACHEHOUND" };
    constexpr std::string defaultCacheFile { "cache.db" };
    constexpr size_t formatVersion { 1 };
}

namespace CacheConstants
{
    constexpr int defaultCapacity { 10000 };
    constexpr int maxCapacity { 100000 };
}

class LRUCache
{
public:
    using ListIt = std::list<std::string>::iterator;
    using HashmapT = std::unordered_map<std::string, std::pair<std::string, ListIt>>;

    std::mutex m_cacheMutex{};

    explicit LRUCache(std::size_t capacity,
        std::string_view cacheFile=Persistence::defaultCacheFile) 
        : m_capacity { capacity }
        , m_cacheFile { cacheFile }
    {
        if(capacity < 1) m_capacity = 1; // cache cant have 0 capacity

        m_map.reserve(capacity);
    }

    std::optional<std::string>  get(const std::string& key);
    void                        put(const std::string& key, const std::string& value);
     
    const HashmapT&             getAll() const;

    void                        saveToDisk();
    void                        loadFromDisk();
    void                        setCacheFile(std::string_view fileName);

private:
    // least recently used entry is at the front of the list
    std::list<std::string> m_usage{};
    HashmapT m_map{};
    
    std::size_t m_capacity{};
    std::string m_cacheFile{};

    void evictLRU();
    void use(ListIt iterator);
};