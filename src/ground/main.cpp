
#include "ground_station.hpp"
#include "logging.hpp"
#include "str_const.hpp"

int main() {
  auto logger = std::make_shared<Logger>("ground_station", "./logs/Ground_");
  auto context = std::make_shared<zmq::context_t>(1);
  GroundStation ground_station(logger, context);
  ground_station.runThreads();
  return 0;
}