
#include "ground_station.hpp"
#include "logging.hpp"

int main() {
    auto logger = std::make_shared<Logger>("ground_station", "./logs"); 
    GroundStation ground_station(logger);
    ground_station.runThreads();
    return 0;
}