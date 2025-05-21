#ifndef GROUND_STATION_HPP
#define GROUND_STATION_HPP

#include <iostream>
#include <memory>
#include <tuple>
#include <vector>
#include <zmq.hpp>

#include "logging.hpp"
#include "str_const.hpp"

class GroundStation {
 public:
  // GroundStation(/* args */);
  GroundStation(std::shared_ptr<Logger> logger,
                std::shared_ptr<zmq::context_t> ctxt,
                std::string pub_address = params::ground_pub_address,
                std::string sub_address = params::ground_sub_address)
      : context_(ctxt),
        subSocket_(*context_, zmq::socket_type::sub),
        pubSocket_(*context_, zmq::socket_type::pub),
        logger_(logger),
        pub_address_(pub_address),
        sub_address_(sub_address),
        running_(true) {
    logger_->Info("Constructing ground station");
    pubSocket_.connect(pub_address_);
    subSocket_.connect(sub_address_);
    logger_->Info("Ground station connected to satellite");
    subSocket_.set(zmq::sockopt::rcvtimeo, 1000);
    add_sub_topic(topics::sat1_tlm);
  }

  ~GroundStation() {
    pubSocket_.close();
    subSocket_.close();
    logger_->Info("Ground station closed");
  }

  void subscriberThread();
  void controlThread();
  void runThreads();
  void add_sub_topic(std::string topic);
  void send_command(std::string topic, std::string cmd);
  std::tuple<zmq::recv_result_t, std::string> recv_tlm(
      std::vector<zmq::message_t>& messages);
  void stop() { running_ = false; }
  bool is_running() { return running_; }

 private:
  /* data */
  std::shared_ptr<zmq::context_t> context_;
  zmq::socket_t subSocket_;
  zmq::socket_t pubSocket_;
  std::shared_ptr<Logger> logger_;
  std::string pub_address_;
  std::string sub_address_;
  bool running_;
};

#endif