# Proxy Application
add_executable(proxy proxy.cpp)
target_link_libraries(proxy ${ZeroMQ_LIBRARIES} glog::glog protobuf::libprotobuf)