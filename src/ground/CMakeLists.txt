add_library(ground_lib ground_station.cpp)
# Ground station application
add_executable(ground main.cpp)
target_link_libraries(ground ground_lib ${ZeroMQ_LIBRARIES} glog::glog protobuf::libprotobuf)