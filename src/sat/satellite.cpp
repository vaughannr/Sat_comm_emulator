#include <thread>
#include <iostream>
#include <string>
#include <future>
#include <csignal>
#include <zmq_addon.hpp>
#include "satellite.hpp"
#include "control_flags.hpp"

static ControlFlags controlFlags;
void sig_handler(int sig){
    std::cout << "Sattelite Received signal: " << sig << std::endl;
    controlFlags.closeAll();
}

void Satellite::add_sub_topic(std::string topic){
    subSocket.set(zmq::sockopt::subscribe, topic);
    logger->Info("Satellite subscribed to topic: " + topic);
}

void Satellite::runThreads(){
    auto tlm_future = std::async(std::launch::async, &Satellite::tlmThread, this);
    auto listen_future = std::async(std::launch::async, &Satellite::listenThread, this);
    listen_future.wait();
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

void Satellite::listenThread(){
    // Allow the pub on satelites to bind to port
    int attempts = 0;
    add_sub_topic(topics::ground_ctrl);
    std::vector<zmq::message_t> message;
    while (!controlFlags.closeSubscriberLoop && attempts < 3) {
        auto res = zmq::recv_multipart(subSocket, std::back_inserter(message));
        if (res){
            std::string topic = message[0].to_string();
            std::string msg = message[1].to_string();
            logger->Info("Satellite received message: " + topic + ": " + msg);

            if(topic == topics::ground_ctrl && msg == "close"){
                logger->Info("Satellite received close command");
                controlFlags.closeControlLoop = true;
                break;
            }
            message.clear();
            attempts = 0;
        }
        else {
            ++attempts;
            logger->Warning( "Received timeout: " + std::to_string(attempts) + " attempts.");
        }
    }
}