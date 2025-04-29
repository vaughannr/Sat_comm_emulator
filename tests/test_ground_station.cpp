// tests/test_ground_station.cpp

#include <gtest/gtest.h>

#include <csignal>
#include <future>
#include <iostream>
#include <memory>
#include <thread>

#include "ground_station.hpp"
#include "logging.hpp"

using std::cout;
using std::endl;
const std::string kTestTopic = "test_topic";
const std::string kTestMessage = "test_message";
const std::string kTestPubAddress = "inproc://testpub";
const std::string kTestSubAddress = "inproc://testsub";

auto logger = std::make_shared<Logger>("ground_station_test", "./logs");

class GroundStationTest : public ::testing::Test {
 public:
  GroundStationTest() {
    ctxt_ = std::make_shared<zmq::context_t>(0);
    pubSocket_ = zmq::socket_t(*ctxt_, zmq::socket_type::pub);
    subSocket_ = zmq::socket_t(*ctxt_, zmq::socket_type::sub);
    pubSocket_.bind(kTestPubAddress);
    subSocket_.connect(kTestSubAddress);
    subSocket_.set(zmq::sockopt::subscribe, kTestTopic);
    logger->Info("Test fixture initialized");
  }

  ~GroundStationTest() {
    pubSocket_.close();
    subSocket_.close();
    ctxt_->close();
    logger->Info("Test fixture destroyed");
  }

 protected:
  void SetUp() override {
    // Code here will be called just before the test executes.
    logger->Info("Setting up test fixture");
  }

  void TearDown() override {
    // Code here will be called just after the test executes.
    logger->Info("Tearing down test fixture");
  }

  void send_command(std::string topic, std::string cmd) {
    zmq::message_t topic_msg(topic);
    zmq::message_t cmd_msg(cmd);
    pubSocket_.send(topic_msg, zmq::send_flags::sndmore);
    pubSocket_.send(cmd_msg, zmq::send_flags::dontwait);
  }

  zmq::socket_t subSocket_;
  zmq::socket_t pubSocket_;
  std::shared_ptr<zmq::context_t> ctxt_;
};

TEST_F(GroundStationTest, Constructor) {
  logger->Info("Testeing ground station constructor");
  GroundStation ground_station(logger, ctxt_, kTestPubAddress, kTestSubAddress);
  EXPECT_NE(&ground_station, nullptr);
}

TEST_F(GroundStationTest, Destructor) {
  std::unique_ptr<GroundStation> ground_station =
      std::make_unique<GroundStation>(logger, ctxt_, kTestPubAddress,
                                      kTestSubAddress);
  EXPECT_NE(ground_station, nullptr);
  ground_station.reset();
  EXPECT_EQ(ground_station, nullptr);
}

TEST_F(GroundStationTest, AddSubTopic) {
  GroundStation ground_station(logger, ctxt_, kTestSubAddress, kTestPubAddress);
  std::vector<zmq::message_t> rec_msgs;
  ground_station.add_sub_topic(kTestTopic);

  auto recv_thread_future =
      std::async(std::launch::async, [&ground_station, &rec_msgs]() {
        auto [res, msg] = ground_station.recv_tlm(rec_msgs);
        EXPECT_TRUE(res);
        EXPECT_EQ(msg, kTestMessage);
      });

  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  send_command(kTestTopic, kTestMessage);
  recv_thread_future.wait();
}

// TEST_F(GroundStationTest, SubscriberThread) {
//   GroundStation ground_station(logger);
//   // Start the subscriber thread
//   std::thread subscriber_thread(&GroundStation::subscriberThread,
//                                 &ground_station);
//   // Send a message to the subscriber socket
//   // Verify that the message is received and processed correctly
//   subscriber_thread.join();
// }

// TEST_F(GroundStationTest, ControlThread) {
//   GroundStation ground_station(logger);
//   // Start the control thread
//   std::thread control_thread(&GroundStation::controlThread, &ground_station);
//   // Verify that the control loop runs correctly
//   control_thread.join();
// }

// TEST_F(GroundStationTest, RunThreads) {
//   GroundStation ground_station(logger);
//   // Start the threads
//   ground_station.runThreads();
//   // Verify that both threads are running and can be joined
// }

// TEST_F(GroundStationTest, SigHandler) {
//   GroundStation ground_station(logger);
//   // Send a signal to the ground station
//   std::raise(SIGINT);
//   // Verify that the signal is handled correctly
// }

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}