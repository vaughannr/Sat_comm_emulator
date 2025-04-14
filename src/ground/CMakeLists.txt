# Ground station application
add_executable(ground main.cpp ground_station.cpp)
target_link_libraries(ground zmq)