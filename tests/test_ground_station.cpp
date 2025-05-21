// tests/test_ground_station.cpp

#include <gtest/gtest.h>
#include <pthread.h>

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

auto logger =
    std::make_shared<Logger>("ground_station_test", "./tests/logs/log");

class GroundStationTest : public ::testing::Test {
 public:
  GroundStationTest()
      : ctxt_(std::make_shared<zmq::context_t>(0)),
        test_ground_station_(logger, ctxt_, kTestSubAddress, kTestPubAddress) {
    // cout << "Initializing test fixture" << endl;
    pubSocket_ = zmq::socket_t(*ctxt_, zmq::socket_type::pub);
    subSocket_ = zmq::socket_t(*ctxt_, zmq::socket_type::sub);
    pubSocket_.bind(kTestPubAddress);
    subSocket_.connect(kTestSubAddress);
    subSocket_.set(zmq::sockopt::subscribe, kTestTopic);
    logger->Info("Test fixture initialized");
    // cout << "Test fixture initialized" << endl;
  }

  ~GroundStationTest() {
    // cout << "Destroying test fixture" << endl;
    pubSocket_.close();
    subSocket_.close();
    // ctxt_->close();
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

  std::shared_ptr<zmq::context_t> ctxt_;
  GroundStation test_ground_station_;
  zmq::socket_t subSocket_;
  zmq::socket_t pubSocket_;
};

TEST_F(GroundStationTest, Constructor) {
  // cout << "Testing ground station constructor" << endl;
  logger->Info("Testeing ground station constructor");
  GroundStation ground_station(logger, ctxt_, kTestSubAddress, kTestPubAddress);
  EXPECT_NE(&ground_station, nullptr);
}

TEST_F(GroundStationTest, Destructor) {
  std::unique_ptr<GroundStation> ground_station =
      std::make_unique<GroundStation>(logger, ctxt_, kTestSubAddress,
                                      kTestPubAddress);
  EXPECT_NE(ground_station, nullptr);
  ground_station.reset();
  EXPECT_EQ(ground_station, nullptr);
}

TEST_F(GroundStationTest, AddSubTopic) {
  std::vector<zmq::message_t> rec_msgs;
  test_ground_station_.add_sub_topic(kTestTopic);

  auto recv_thread_future = std::async(std::launch::async, [this, &rec_msgs]() {
    auto [res, msg] = this->test_ground_station_.recv_tlm(rec_msgs);
    EXPECT_TRUE(res);
    EXPECT_EQ(msg, kTestMessage);
  });

  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  send_command(kTestTopic, kTestMessage);
  recv_thread_future.wait();
}

TEST_F(GroundStationTest, SubscriberThread) {
  // Start the subscriber thread
  std::thread subscriber_thread(&GroundStation::subscriberThread,
                                &test_ground_station_);

  // Verify that the subscriber thread is closed gracefully with timeout
  subscriber_thread.join();
  EXPECT_TRUE(true);

  std::thread subscriber_thread2(&GroundStation::subscriberThread,
                                 &test_ground_station_);
  test_ground_station_.stop();
  subscriber_thread2.join();
  EXPECT_FALSE(test_ground_station_.is_running());
}

TEST_F(GroundStationTest, ControlThread) {
  // Start the control thread
  std::thread control_thread(&GroundStation::controlThread,
                             &test_ground_station_);

  // Verify that the control thread is closed gracefully
  EXPECT_TRUE(test_ground_station_.is_running());
  test_ground_station_.stop();
  control_thread.join();
  EXPECT_FALSE(test_ground_station_.is_running());
}

TEST_F(GroundStationTest, RunThreads) {
  // Start the threads
  std::thread ground_station_thread(&GroundStation::runThreads,
                                    &test_ground_station_);
  // Verify that the threads are running
  EXPECT_TRUE(test_ground_station_.is_running());
  // Stop the threads
  test_ground_station_.stop();
  ground_station_thread.join();
  EXPECT_FALSE(test_ground_station_.is_running());
}

TEST_F(GroundStationTest, SigHandler) {
  // Send a signal to the ground station
  std::thread ground_station_thread(&GroundStation::runThreads,
                                    &test_ground_station_);
  pthread_t thread_id = ground_station_thread.native_handle();

  pthread_kill(thread_id, SIGINT);
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  ground_station_thread.join();
  EXPECT_FALSE(test_ground_station_.is_running());
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}