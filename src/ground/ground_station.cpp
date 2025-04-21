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
    logger->Info("Ground station subscribed to topic: " + topic);
}

void GroundStation::subscriberThread() {
    // Allow the pub on satelites to bind to port
    int attempts = 0;
    add_sub_topic(topics::sat1_tlm);
    std::vector<zmq::message_t> message;
    try{
        while (!controlFlags.closeSubscriberLoop && attempts < 3) {
            auto res = zmq::recv_multipart(subSocket, std::back_inserter(message));
            if (res){
                logger->Info("Ground station received message: " + message[1].to_string()
                    + "\n from topic: " + message[0].to_string());
                message.clear();
                attempts = 0;
            }
            else {
                ++attempts;
                logger->Warning( "Received timeout: " + std::to_string(attempts) + " attempts.");
            }
        }
    } catch(zmq::error_t& e){ 
        logger->Error("Ground station ZMQError: " + std::string(e.what()));
    }

    logger->Info("Closing ground station subscriber loop");
}

void GroundStation::controlThread() {
    uint16_t count = 0;
    while (!controlFlags.closeControlLoop) {
        logger->Info("Ground station control loop iteration " + std::to_string(count++));
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    logger->Info("Sending cmd to close satellite");
    zmq::message_t topic(topics::ground_ctrl);
    zmq::message_t message(std::string("close"));
    pubSocket.send(topic, zmq::send_flags::sndmore);
    pubSocket.send(message, zmq::send_flags::dontwait);

    logger->Info("Closing ground station control loop");
}