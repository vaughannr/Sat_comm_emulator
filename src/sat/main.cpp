#include "logging.hpp"
#include "satellite.hpp"

int main() {
  auto logger = std::make_shared<Logger>("Satellite", "./logs/Sat_");
  Satellite satellite(logger);
  satellite.runThreads();
  return 0;
}