#include "satellite.hpp"

#include <csignal>
#include <future>
#include <iostream>
#include <print>
#include <string>
#include <thread>
#include <zmq_addon.hpp>

#include "control_flags.hpp"

static ControlFlags controlFlags;
void sig_handler(int sig) {
  std::cout << "Sattelite Received signal: " << sig << std::endl;
  controlFlags.closeAll();
}

void Satellite::addSubTopic(std::string topic) {
  subSocket.set(zmq::sockopt::subscribe, topic);
  logger->Info("Satellite subscribed to topic: " + topic);
}

void Satellite::runThreads() {
  auto tlm_future = std::async(std::launch::async, &Satellite::tlmThread, this);
  auto listen_future =
      std::async(std::launch::async, &Satellite::listenThread, this);
  listen_future.wait();
  tlm_future.wait();
}

void Satellite::tlmThread() {
  std::signal(SIGINT, sig_handler);
  std::signal(SIGTERM, sig_handler);
  logger->Info("Starting satellite tlm pub loop");
  while (!controlFlags.closeControlLoop) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    auto telemetry = generateTelemetry();
    auto telemetryStr = telemetry.SerializeAsString();

    zmq::message_t topic(std::string("sat1/tlm"));
    zmq::message_t tlmMessage(telemetryStr.size());
    memcpy(tlmMessage.data(), telemetryStr.c_str(), telemetryStr.size());

    logger->Info("Sending message: " + tlmMessage.to_string());

    pubSocket.send(topic, zmq::send_flags::sndmore);
    pubSocket.send(tlmMessage, zmq::send_flags::dontwait);
  }
}

void Satellite::listenThread() {
  // Allow the pub on satelites to bind to port
  int attempts = 0;
  addSubTopic(topics::ground_ctrl);
  std::vector<zmq::message_t> message;
  while (!controlFlags.closeSubscriberLoop && attempts < 3) {
    auto res = zmq::recv_multipart(subSocket, std::back_inserter(message));
    if (res) {
      std::string topic = message[0].to_string();
      satcom::Command cmd;
      if (cmd.ParseFromArray(message[1].data(), message[1].size())) {
        processCmd(cmd);
      } else {
        logger->Error("Failed to parse command");
      }
      message.clear();
      attempts = 0;
    } else {
      ++attempts;
      logger->Warning("Received timeout: " + std::to_string(attempts) +
                      " attempts.");
    }
  }
}

void Satellite::processCmd(const satcom::Command& cmd) {
  switch (cmd.type()) {
    case satcom::Command_CommandType_CHANGE_ORBIT:
      std::println("Satellite Received Change Orbit Command");
      break;
    case satcom::Command_CommandType_TAKE_PHOTO:
      std::println("Satellite Received Take Photo Command");
      break;
    case satcom::Command_CommandType_POWER_SAVE:
      std::println("Satellite Received Power Save Command");
      break;
    case satcom::Command_CommandType_RESTART:
      std::println("Satellite Received Restart Command");
      break;
    case satcom::Command_CommandType_STOP:
      std::println("Satellite Received Stop Command");
      controlFlags.closeSubscriberLoop = true;
      break;
    default:
      logger->Warning("Unknown command type");
  }
}

void Satellite::updateTelemetry() {
  altitude_ = altitude_;
  latitude_ = latitude_;
  longitude_ = longitude_;
  battery_level_ = battery_level_;
  temperature_ = temperature_;
}

satcom::TelemetryData Satellite::generateTelemetry() {
  satcom::TelemetryData telemetry;
  telemetry.set_timestamp(
      std::chrono::system_clock::now().time_since_epoch().count());
  telemetry.set_altitude(altitude_);
  telemetry.set_latitude(latitude_);
  telemetry.set_longitude(longitude_);
  telemetry.set_battery_level(battery_level_);
  telemetry.set_temperature(temperature_);

  return telemetry;
}