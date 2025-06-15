#include "cli_client.h"
#include "chprotocol.h"
#include <chrono>
#include <future>

void throughput_test(int client_fd, int reps);

int main()
{
    using namespace std::chrono;

    constexpr int client_count { 20'000 };
    constexpr int req_per_client { 2 };
    constexpr int request_repititions { 20 };

    std::array<int, client_count> connections{};
    for(int i{}; i < client_count; ++i) {
        connections[i] = setupClientSocket("127.0.0.1", "5050");
        if(connections[i] == -1) {
            std::cout << "A client failed to connect to database, exiting\n";
            return -1;
        }
    }
    
    std::cout << client_count << " clients connected to database, starting throughput test...\n";

    std::array<std::future<void>, client_count> futures;

    auto start = high_resolution_clock::now();
    for(int i{}; i < client_count; ++i) {
        futures[i] = std::async(std::launch::async, throughput_test, connections[i], request_repititions);
    }

    for(auto& future : futures) {
        future.wait();
    }
    auto end = high_resolution_clock::now();

    auto duration = duration_cast<milliseconds>(end - start);
    auto duration_sec = duration_cast<seconds>(end - start).count();

    if (duration_sec > 0) {
        auto throughput = client_count * req_per_client * request_repititions / duration_sec;
        std::cout << "Req/s: " << throughput << "\n";
    } else {
        std::cout << "Duration too short to measure throughput reliably (under 1 second).\n";
    }

    std::cout << "Total time taken: " << duration.count() << "ms" 
        << "\t" << duration_cast<seconds>(duration).count() << "s\n";

    return 0;
}

void throughput_test(int client_fd, int reqs)
{
    using namespace std::chrono;

    Protocol::Packet packet{};
    Protocol::Buffer buffer{};

    for(int i{}; i < reqs; ++i) {

        bufferFactory(buffer);
        packetFactory(packet);

        packet = { Protocol::PUT, "hello" + std::to_string(rand() % 100000), "value" + std::to_string(rand()) };
        serialize(packet, buffer);
        send(client_fd, buffer.data(), sizeof(Protocol::Buffer), 0);

        bufferFactory(buffer);
        packetFactory(packet);

        packet = { Protocol::GET, "hello" + std::to_string(rand() % 100000)};
        serialize(packet, buffer);
        send(client_fd, buffer.data(), sizeof(Protocol::Buffer), 0);

        size_t total = 0;
        while (total < sizeof(Protocol::Buffer)) {
            ssize_t r = recv(client_fd, buffer.data() + total, sizeof(Protocol::Buffer) - total, 0);
            if (r <= 0) {
                std::cerr << "Recv failed or connection closed\n";
                return;
            }
            total += r;
        }
    }
}
