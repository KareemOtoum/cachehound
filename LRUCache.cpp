#include "LRUCache.h"

// move element to the end of the list
void LRUCache::use(ListIt iterator)
{
    m_usage.splice(m_usage.end(), m_usage, iterator);
}

std::optional<std::string> LRUCache::get(const std::string& key)
{
    if(!m_map.contains(key))
    {
        return std::nullopt;
    }

    const std::pair<std::string, ListIt>& pair { m_map[key] }; 
    use(pair.second);

    return pair.first;
}

void LRUCache::evictLRU()
{
    const std::string& key { m_usage.front() };
    m_map.erase(key);
    m_usage.pop_front();
}

void LRUCache::put(const std::string& key, const std::string& value)
{
    if(m_map.contains(key)) 
    {
        use(m_map[key].second);
        m_map[key].first = value;
        return;
    }
    
    if(m_map.size() >= m_capacity) // need to evict LRU entry
    {
        evictLRU();
    }

    // add new entry
    m_usage.emplace_back(std::string{key});

    m_map[key] = std::pair{value, --m_usage.end()};
}