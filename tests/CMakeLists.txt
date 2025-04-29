pkg_check_modules(GTest REQUIRED gtest)
pkg_check_modules(GMock REQUIRED gmock)
include_directories(
    ${GTEST_INCLUDE_DIRS}
    ../src/ground
    ../src/sat
    ../src/proxy)
link_directories(${GTEST_LIBRARY_DIRS})

add_executable(test_ground_station test_ground_station.cpp)
add_test(NAME test_ground_station COMMAND test_ground_station)
target_link_libraries(test_ground_station gtest gtest_main ground_lib)