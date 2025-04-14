#include <iostream>
#include <zmq.hpp>
#include <atomic>
#include <csignal>

std::atomic<bool> running(true);

void sig_handler(int sig){
    std::cout << "Received signal: " << sig << std::endl;
    running = false;
}

int main() {
    std::signal(SIGINT, sig_handler);
    std::signal(SIGTERM, sig_handler);

    zmq::context_t context(1);
    zmq::socket_t xSub_socket(context, zmq::socket_type::xsub);
    zmq::socket_t xPub_socket(context, zmq::socket_type::xpub);

    xSub_socket.bind("tcp://localhost:5555");
    xPub_socket.bind("tcp://localhost:5556");
    std::cout << "Proxy Starting: XSUB on port 5555, XPUB on port 5556" << std::endl;

    while (running) {
        try{
            zmq::proxy(xSub_socket, xPub_socket);
        }
        catch(zmq::error_t& e){
            std::cout << "Proxy ZMQError: " << e.what() << std::endl;
        }
    }

    std::cout << "Shutting down proxy...." << std::endl;

    // Close the sockets
    xSub_socket.close();
    xPub_socket.close();
    context.close();

    std::cout << "Proxy shutdown safely" << std::endl;
    return 0;
}