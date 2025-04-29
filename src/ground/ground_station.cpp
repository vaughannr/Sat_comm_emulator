#include "ground_station.hpp"

#include <csignal>
#include <future>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <zmq_addon.hpp>

#include "control_flags.hpp"

static ControlFlags controlFlags;

void sig_handler(int sig) {
  std::cout << "Received signal: " << sig << std::endl;
  controlFlags.closeAll();
}

void GroundStation::runThreads() {
  std::signal(SIGINT, sig_handler);
  std::signal(SIGTERM, sig_handler);
  auto sub_future =
      std::async(std::launch::async, &GroundStation::subscriberThread, this);
  auto control_future =
      std::async(std::launch::async, &GroundStation::controlThread, this);
  sub_future.wait();
  control_future.wait();
}

void GroundStation::add_sub_topic(std::string topic) {
  subSocket_.set(zmq::sockopt::subscribe, topic);
  logger_->Info("Ground station subscribed to topic: " + topic);
}

std::tuple<zmq::recv_result_t, std::string> GroundStation::recv_tlm(
    std::vector<zmq::message_t>& messages) {
  try {
    auto res = zmq::recv_multipart(subSocket_, std::back_inserter(messages));
    if (res) {
      std::string msg = messages[1].to_string();
      return std::make_pair(res, msg);
    }
    return std::make_pair(zmq::recv_result_t(), "");
  } catch (zmq::error_t& e) {
    logger_->Error("Ground station ZMQError: " + std::string(e.what()));
    return std::make_pair(zmq::recv_result_t(), "");
  }
}

void GroundStation::subscriberThread() {
  std::vector<zmq::message_t> messages;
  messages.reserve(2);
  int attempts = 0;
  try {
    while (!controlFlags.closeSubscriberLoop && attempts < 3) {
      auto [res, msg] = recv_tlm(messages);
      if (res) {
        logger_->Info("Ground station received message: " + msg);
        attempts = 0;
      } else {
        ++attempts;
        logger_->Warning("Received timeout: " + std::to_string(attempts) +
                         " attempts.");
      }
      messages.clear();
    }
  } catch (zmq::error_t& e) {
    logger_->Error("Ground station ZMQError: " + std::string(e.what()));
  }

  logger_->Info("Closing ground station subscriber loop");
}

void GroundStation::controlThread() {
  uint16_t count = 0;
  while (!controlFlags.closeControlLoop) {
    logger_->Info("Ground station control loop iteration " +
                  std::to_string(count++));
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  logger_->Info("Sending cmd to close satellite");
  send_command(topics::ground_ctrl, "close");

  logger_->Info("Closing ground station control loop");
}

void GroundStation::send_command(std::string topic, std::string cmd) {
  zmq::message_t topic_msg(topic);
  zmq::message_t cmd_msg(cmd);
  pubSocket_.send(topic_msg, zmq::send_flags::sndmore);
  pubSocket_.send(cmd_msg, zmq::send_flags::dontwait);
}