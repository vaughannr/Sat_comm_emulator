#include <thread>
#include <chrono>

void simulate_latency(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}