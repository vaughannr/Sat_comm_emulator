#include <atomic>
#include <iostream>
#include <memory>
#include <zmq.hpp>

#include "logging.hpp"
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
  void add_sub_topic(std::string topic);

 private:
  /* data */
  zmq::context_t context;
  zmq::socket_t subSocket;
  zmq::socket_t pubSocket;
  std::shared_ptr<Logger> logger;
};
