// tests/test_ground_station.cpp

#include <gtest/gtest.h>
#include <memory>
#include <thread>
#include <csignal>
#include "ground_station.hpp"
#include "logging.hpp"

std::shared_ptr<Logger> logger = std::make_shared<Logger>("ground_station_test", "./test_logs");

TEST(GroundStationTest, Constructor) {
    GroundStation ground_station(logger);
    EXPECT_NE(&ground_station, nullptr);
}

TEST(GroundStationTest, Destructor) {
    GroundStation* ground_station = new GroundStation(logger);
    delete ground_station;
    EXPECT_EQ(nullptr, ground_station);
}

TEST(GroundStationTest, AddSubTopic) {
    zmq::context_t context(1);
    zmq::socket_t pub(context, zmq::socket_type::pub);
    pub.bind("tcp://localhost:5556");
    GroundStation ground_station(logger);
    std::string topic = "test_topic";
    ground_station.add_sub_topic(topic);
    // Verify that the topic is subscribed to
    // This might require a mock ZMQ socket or a test-specific implementation
}

TEST(GroundStationTest, SubscriberThread) {
    GroundStation ground_station(logger);
    // Start the subscriber thread
    std::thread subscriber_thread(&GroundStation::subscriberThread, &ground_station);
    // Send a message to the subscriber socket
    // Verify that the message is received and processed correctly
    subscriber_thread.join();
}

TEST(GroundStationTest, ControlThread) {
    GroundStation ground_station(logger);
    // Start the control thread
    std::thread control_thread(&GroundStation::controlThread, &ground_station);
    // Verify that the control loop runs correctly
    control_thread.join();
}

TEST(GroundStationTest, RunThreads) {
    GroundStation ground_station(logger);
    // Start the threads
    ground_station.runThreads();
    // Verify that both threads are running and can be joined
}

TEST(GroundStationTest, SigHandler) {
    GroundStation ground_station(logger);
    // Send a signal to the ground station
    std::raise(SIGINT);
    // Verify that the signal is handled correctly
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}