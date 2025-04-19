enable_testing()

add_executable(test_ground_station test_ground_station.cpp)

add_test(NAME test_ground_station COMMAND test_ground_station)