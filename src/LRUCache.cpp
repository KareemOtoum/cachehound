#include "LRUCache.h"

// move element to the end of the list
void LRUCache::use(ListIt iterator)
{
    m_usage.splice(m_usage.end(), m_usage, iterator);
}

std::optional<std::string> LRUCache::get(const std::string& key)
{
    std::lock_guard<std::mutex> lock(m_cacheMutex);

    if(!m_map.contains(key))
    {
        return std::nullopt;
    }

    const std::pair<std::string, ListIt>& pair { m_map[key] }; 
    use(pair.second);

    return pair.first;
}

const LRUCache::HashmapT& LRUCache::getAll() const
{
    return m_map;
}

void LRUCache::evictLRU()
{
    if(m_map.size() == 0) return; // dont evict if cache is empty

    const std::string& key { m_usage.front() };
    m_map.erase(key);
    m_usage.pop_front();
}

void LRUCache::put(const std::string& key, const std::string& value)
{
    std::lock_guard<std::mutex> lock(m_cacheMutex);

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
    m_usage.push_back(std::string{key});

    m_map[key] = std::pair{value, --m_usage.end()};
}

void LRUCache::setCacheFile(std::string_view fileName)
{
    m_cacheFile = fileName;
}

// writes a size_t to a file in binary format
void writeSizeT(std::ofstream& file, size_t val)
{
    file.write(reinterpret_cast<const char*>(&val), sizeof(val));   
}

// writes the strings length then the string to a file in binary format
void writeString(std::ofstream& file, const std::string_view str)
{
    writeSizeT(file, str.size());
    file.write(&str[0], str.size());
}

void LRUCache::saveToDisk()
{
    std::lock_guard<std::mutex> lock(m_cacheMutex);

    std::ofstream file(m_cacheFile, std::ios::binary);

    writeString(file, Persistence::cacheFileHeader);    // "LRU-DB"
    writeSizeT(file, Persistence::formatVersion);       // format version

    // number of hashmap list entries
    writeSizeT(file, m_map.size());

    // write all hashmap key and values
    for(const auto& [key, value] : m_map)
    {
        writeString(file, key);
        writeString(file, value.first);
    }

    // number of usage list entries
    writeSizeT(file, m_usage.size());

    // write all usage list entries
    for(const std::string_view str : m_usage)
    {
        writeString(file, str);
    }
}

size_t readSizeT(std::ifstream& file)
{
    size_t value{};
    file.read(reinterpret_cast<char*>(&value), sizeof(size_t));
    return value;
}

std::string readString(std::ifstream& file)
{
    size_t stringLength { readSizeT(file) };
    std::string str(stringLength, '\0');
    file.read(str.data(), stringLength);
    return str;
}

void LRUCache::loadFromDisk()
{
    std::lock_guard<std::mutex> lock(m_cacheMutex);

    std::ifstream file(m_cacheFile, std::ios::binary);

    if(!file)
    {
        std::cerr << "Could not load from disk: file not found!" << "\n";
        return;
    }

    std::string magicHeader { readString(file) };
    
    if (strncmp(magicHeader.data(), Persistence::cacheFileHeader.data(), 
        Persistence::cacheFileHeader.size()) != 0) 
    { 
        std::cerr << "Could not load from disk: Invalid file format\n";
        return;
    }

    size_t version = readSizeT(file);
    
    if (version != Persistence::formatVersion) 
    {
        std::cerr << "Could not load from disk: Unsupported file format version\n";
        return;
    }
    
    // read hashmap size
    size_t mapSize { readSizeT(file) };

    m_map.clear();
    m_usage.clear();
    m_map.reserve(mapSize);

    // read hashmap entries
    for(size_t i{}; i < mapSize; ++i)
    {
        std::string key { readString(file) };
        std::string value { readString(file) };
        
        m_map[key] = std::pair<std::string, ListIt>{value, nullptr};
    }

    // read usage list size
    size_t usageListSize { readSizeT(file) };

    // read usage list entries
    for(size_t i {}; i < usageListSize; ++i)
    {
        std::string usageListEntry { readString(file) };
        m_usage.push_back(usageListEntry);  // entries are in chronological order 
                                            //least recently used -> most recently used

        m_map[usageListEntry].second = --m_usage.end(); // set hashmaps entry iterator to 
                                                        // this usage list entry 
    }
}