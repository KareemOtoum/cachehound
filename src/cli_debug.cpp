#include "cli_debug.h"

#define COMMAND_ARGS const std::vector<std::string>& args, LRUCache& cache

void printGet(LRUCache& cache, const std::string& key);
void printLogo();
void printCommands();

void testCache();

void startDebugCLI(LRUCache& cache)
{
    // commands hashmap
    // commands return true to continue and false to exit cli
    std::unordered_map<std::string, std::function<bool(const std::vector<std::string>&
        , LRUCache&)>> commandMap;

    commandMap["put"] = [](COMMAND_ARGS)
    {
        if(args.size() != 3)
        {
            std::cout << "Usage: put <key> <value>" << "\n";
            return true;
        }

        cache.put(args[1], args[2]);
        return true;
    };

    commandMap["get"] = [](COMMAND_ARGS)
    {
        if(args.size() != 2)
        {
            std::cout << "Usage: get <key>" << "\n";
            return true;
        }

        printGet(cache, args[1]);
        return true;
    };

    commandMap["getall"] = [](COMMAND_ARGS)
    {
        if(args.size() != 1)
        {
            std::cout << "Usage: getall" << "\n";
            return true;
        }

        const LRUCache::HashmapT& hashmap { cache.getAll() };
        
        for(const auto& [key, value] : hashmap)
        {
            std::cout << key << ": " << value.first << "\n";
        }

        return true;
    };

    commandMap["save"] = [](COMMAND_ARGS)
    {
        if(args.size() != 1)
        {
            std::cout << "Usage: save" << "\n";
            return true;
        }
        
        std::cout << "| Saving to disk..." << "\n";
        cache.saveToDisk();
        std::cout << "| Done" << "\n";
        return true;
    };

    commandMap["load"] = [](COMMAND_ARGS)
    {
        if(args.size() != 1)
        {
            std::cout << "Usage: load" << "\n";
            return true;
        }
        
        std::cout << "| Loading from disk..." << "\n";
        cache.loadFromDisk();
        std::cout << "| Done" << "\n";
        return true;
    };

    commandMap["exit"] = [](COMMAND_ARGS)
    {
        (void)cache;

        if(args.size() != 1)
        {
            std::cout << "Usage: exit" << "\n";
            return true;
        }

        return false;
    };

    printLogo();
    printCommands();

    std::string input{};

    while(1)
    {
        std::cout << "> ";
        std::getline(std::cin, input);

        if(!std::cin) return;

        std::istringstream iss(input);
        std::vector<std::string> tokens;
        std::string token;

        while (iss >> token) tokens.push_back(token);
        
        if(tokens.empty()) continue;

        if(commandMap.contains(tokens[0]))
        {
            if(!commandMap[tokens[0]](tokens, cache))
            {
                break; // exit cli if functions returns false
            }
        }
        else
        {
            std::cout << "Unknown command: " << tokens[0] << "\n";
        }
    }
}

void printGet(LRUCache& cache, const std::string& key)
{
    auto value { cache.get(key) };

    std::cout << key << ": " << (value ? value.value() : "NOT FOUND") << "\n";
}

void printLogo()
{
    std::string logo { R"(
                _          _                           _ 
  ___ __ _  ___| |__   ___| |__   ___  _   _ _ __   __| |
 / __/ _` |/ __| '_ \ / _ \ '_ \ / _ \| | | | '_ \ / _` |
| (_| (_| | (__| | | |  __/ | | | (_) | |_| | | | | (_| |
 \___\__,_|\___|_| |_|\___|_| |_|\___/ \__,_|_| |_|\__,_|
                                                         
    )"};

    std::cout << "\n" << logo << "\n----------------------------------------------------------\n\n";
}

void testCache()
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

        cache.saveToDisk();
    }


    { // test loading from disk

        LRUCache cache{4}; 

        std::cout << "Loading from disk..." << "\n";
        std::cout << "-----------------" << "\n";

        cache.loadFromDisk();

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
}

void printCommands()
{
    std::cout << "\t------------\n";
    std::cout << "\t| Commands |\n";
    std::cout << "\t------------\n";
    std::cout << "\t| put <key> <value>     - puts a key-value pair in the cache\n";
    std::cout << "\t| get <key>             - gets a value from the cache\n";
    std::cout << "\t| getall                - gets all values from the cache\n";
    std::cout << "\t| save                  - saves cache to file\n";
    std::cout << "\t| load                  - loads cache from file\n";
    std::cout << "\t| exit\n";
    std::cout << "\n";
}