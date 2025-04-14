cmake_minimum_required(VERSION 3.10)
project(SatelliteCommunicationEmulator)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find and configure pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(ZMQ REQUIRED libzmq)

# Include directories
include_directories(include)

# Add subdirectories
add_subdirectory(src/proxy)
add_subdirectory(src/ground)
add_subdirectory(src/sat)

# Link external libraries (if needed, e.g., zmq)
# target_link_libraries(SatelliteCommunicationEmulator zmq)

# Add unit tests
# enable_testing()
# add_subdirectory(tests)
