#include <atomic>
#include <iostream>
#include <memory>
#include <zmq.hpp>

#include "logging.hpp"
#include "satcom_messages.pb.h"
#include "str_const.hpp"

class Satellite {
 public:
  Satellite(std::shared_ptr<Logger> logger_)
      : context(1),
        subSocket(context, zmq::socket_type::sub),
        pubSocket(context, zmq::socket_type::pub),
        logger(logger_) {
    pubSocket.connect(params::sat_pub_address);
    subSocket.connect(params::sat_sub_address);
  }

  ~Satellite() {
    pubSocket.close();
    subSocket.close();
    context.close();
    logger->Info("Satellite closed");
  }

  void tlmThread();
  void listenThread();
  void runThreads();
  void addSubTopic(std::string topic);
  void processCmd(const satcom::Command& cmd);
  void updateTelemetry();
  satcom::TelemetryData generateTelemetry();

 private:
  zmq::context_t context;
  zmq::socket_t subSocket;
  zmq::socket_t pubSocket;
  std::shared_ptr<Logger> logger;

  /* data */
  double altitude_;       // meters
  double latitude_;       // degrees
  double longitude_;      // degrees
  double battery_level_;  // percentage
  double temperature_;    // Celsius
};
