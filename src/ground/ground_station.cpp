#include <thread>
#include <future>
#include <csignal>
#include <vector>
#include <string>
#include <iostream>
#include <zmq_addon.hpp>

#include "control_flags.hpp"
#include "ground_station.hpp"

static ControlFlags controlFlags;

void sig_handler(int sig){
    std::cout << "Received signal: " << sig << std::endl;
    controlFlags.closeAll();
}

void GroundStation::runThreads() {
    std::signal(SIGINT, sig_handler);
    std::signal(SIGTERM, sig_handler);
    auto sub_future = std::async(std::launch::async, &GroundStation::subscriberThread, this);
    auto control_future = std::async(std::launch::async, &GroundStation::controlThread, this);
    sub_future.wait();
    control_future.wait();
}

void GroundStation::add_sub_topic(std::string topic){
    subSocket.set(zmq::sockopt::subscribe, topic);
}

void GroundStation::subscriberThread() {
    // Allow the pub on satelites to bind to port
    add_sub_topic("sat1/tlm");
    std::vector<zmq::message_t> message;
    while (!controlFlags.closeSubscriberLoop) {
        auto res = zmq::recv_multipart(subSocket, std::back_inserter(message));
        assert(res && "recv failed");
        assert(res == 2);
        std::cout << "Ground station received message: " << message[1].to_string()
            << " from topic: " << message[0].to_string() << std::endl;
    }
}

void GroundStation::controlThread() {
    uint16_t count = 0;
    while (!controlFlags.closeControlLoop) {
        std::cout << "Ground station control loop iteration " << count++ <<  std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}