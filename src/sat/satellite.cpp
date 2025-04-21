#include <thread>
#include <iostream>
#include <string>
#include <future>
#include <csignal>
#include "satellite.hpp"
#include "control_flags.hpp"

static ControlFlags controlFlags;
void sig_handler(int sig){
    std::cout << "Sattelite Received signal: " << sig << std::endl;
    controlFlags.closeAll();
}
void Satellite::runThreads(){
    auto tlm_future = std::async(std::launch::async, &Satellite::tlmThread, this);
    tlm_future.wait();
}
void Satellite::tlmThread(){
    std::signal(SIGINT, sig_handler);
    std::signal(SIGTERM, sig_handler);
    logger->Info("Starting satellite tlm pub loop");
    uint16_t count = 0u;
    while (!controlFlags.closeControlLoop) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        auto msgStr = std::string("Hello Ground Station! Sending packet #") + std::to_string(count);
        zmq::message_t topic(std::string("sat1/tlm"));
        zmq::message_t message(msgStr.size());
        memcpy(message.data(), msgStr.c_str(), msgStr.size());

        logger->Info("Sending message: " + message.to_string());

        pubSocket.send(topic, zmq::send_flags::sndmore);
        pubSocket.send(message, zmq::send_flags::dontwait);
        count++;
    }
}