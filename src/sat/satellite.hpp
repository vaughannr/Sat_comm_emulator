#include <zmq.hpp>
#include <atomic>
#include <iostream>

class Satellite {
public:
    Satellite(/* args */) :
        context(1),
        subSocket(context, zmq::socket_type::sub),
        pubSocket(context, zmq::socket_type::pub)
    {
        pubSocket.connect("tcp://localhost:5555");
        subSocket.connect("tcp://localhost:5556");
    }

    ~Satellite() {
        pubSocket.close();
        subSocket.close();
        context.close();
        std::cout << "Satellite closed" << std::endl;
    }

    void tlmThread();
    void runThreads();

private:
    /* data */
    zmq::context_t context;
    zmq::socket_t subSocket;
    zmq::socket_t pubSocket;
};

