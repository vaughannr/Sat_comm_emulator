
#include "ground_station.hpp"
#include "logging.hpp"
#include "str_const.hpp"

int main() {
    auto logger = std::make_shared<Logger>("ground_station", "./logs/Ground_"); 
    GroundStation ground_station(logger);
    ground_station.runThreads();
    return 0;
}