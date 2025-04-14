#include <zmq.hpp>
#include <atomic>
#include <iostream>


class GroundStation
{
public:
    // GroundStation(/* args */);
    GroundStation() : 
        context(1),
        subSocket(context, zmq::socket_type::sub),
        pubSocket(context, zmq::socket_type::pub)
    {
        pubSocket.connect("tcp://localhost:5555");
        subSocket.connect("tcp://localhost:5556");
    }

    ~GroundStation()
    {
        pubSocket.close();
        subSocket.close();
        context.close();
        std::cout << "Ground station closed" << std::endl;
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

};