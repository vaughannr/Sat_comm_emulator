#ifndef GROUND_STATION_HPP
#define GROUND_STATION_HPP

#include <zmq.hpp>
#include <atomic>
#include <iostream>
#include <memory>

#include "logging.hpp"
#include "str_const.hpp"


class GroundStation
{
public:
    // GroundStation(/* args */);
    GroundStation(std::shared_ptr<Logger> logger_) : 
        context(1),
        subSocket(context, zmq::socket_type::sub),
        pubSocket(context, zmq::socket_type::pub),
        logger(logger_)
    {
        pubSocket.connect(params::ground_pub_address);
        subSocket.connect(params::ground_sub_address);
        subSocket.set(zmq::sockopt::rcvtimeo, 1000);
    }

    ~GroundStation()
    {
        pubSocket.close();
        subSocket.close();
        context.close();
        logger->Info("Ground station closed");
    }


    void subscriberThread();
    void controlThread();
    void runThreads();
    void add_sub_topic(std::string topic);
    
private:
    /* data */
    zmq::context_t context;
    zmq::socket_t subSocket;
    zmq::socket_t pubSocket;
    std::shared_ptr<Logger> logger;


};

#endif