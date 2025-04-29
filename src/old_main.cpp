#include <chrono>
#include <iostream>
#include <thread>
#include <zmq.hpp>

#include "latency.hpp"
#include "noise.hpp"

// Satellite server thread function
void run_satellite_server() {
  zmq::context_t context(1);
  zmq::socket_t socket(context, ZMQ_REP);  // REP: Reply socket type
  socket.bind("tcp://*:5555");

  std::cout << "Satellite server started" << std::endl;

  //    while (std::cin.get() != 'q') {
  zmq::message_t request;

  // receive request
  auto rres = socket.recv(request);
  if (rres == 0) {
    std::cout << "Satellite server closed" << std::endl;
    return;
  }
  std::cout << "Satellite received request: " << request.to_string()
            << std::endl;

  // simulate latency and introduce noise
  std::string message = "Hello Ground Station!";
  simulate_latency(200);  // Simulate 200ms latency
  message = introduce_noise(request.to_string());

  // send response to the client
  zmq::message_t response(message.size());
  memcpy(response.data(), message.data(), message.size());
  auto sres = socket.send(response, zmq::send_flags::dontwait);
  if (sres == 0) {
    std::cout << "Satellite server closed" << std::endl;
    return;
  }

  std::cout << "Satellite sent response: " << response.to_string() << std::endl;
  //    }
}  // end run_satellite_server

void run_ground_station_client() {
  zmq::context_t context(1);
  zmq::socket_t socket(context, ZMQ_REQ);  // REQ: Request socket type
  socket.connect("tcp://localhost:5555");

  std::cout << "Ground station client started" << std::endl;

  // send message to satellite server
  std::string message = "Hello Satellite!";
  zmq::message_t request(message.size());
  memcpy(request.data(), message.data(), message.size());
  auto ret = socket.send(request, zmq::send_flags::dontwait);
  if (ret == 0) {
    std::cout << "Ground station client closed" << std::endl;
    return;
  }
  std::cout << "Ground station sent request: " << request.to_string()
            << std::endl;

  // recieve response
  zmq::message_t response;
  auto rret = socket.recv(response);
  if (rret == 0) {
    std::cout << "Ground station client closed" << std::endl;
    return;
  }
  std::cout << "Ground station received response: " << response.to_string()
            << std::endl;
}  // end run_ground_station_client

int main() {
  // run the satellite server in a separate thread
  std::thread satellite_thread(run_satellite_server);

  // run the ground station client
  run_ground_station_client();

  // wait for the satellite server thread to finish
  satellite_thread.join();

  return 0;
}  // end main
