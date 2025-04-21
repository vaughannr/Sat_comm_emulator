cmake_minimum_required(VERSION 3.10)
project(SatelliteCommunicationEmulator)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Make log folders for each executable and a make
# target for each log folder to delete logs
set(LOG_DIRS
    ${CMAKE_BINARY_DIR}/src/ground/logs
    ${CMAKE_BINARY_DIR}/src/sat/logs
    ${CMAKE_BINARY_DIR}/src/proxy/logs
    ${CMAKE_BINARY_DIR}/tests/logs
)

foreach(LOG_DIR IN LISTS LOG_DIRS)
    file(MAKE_DIRECTORY ${LOG_DIR})
endforeach()

add_custom_target(cleanLogs
    COMMAND rm -rf ${LOG_DIRS}/*
    COMMENT "Cleaning logs"
)

# Find and configure pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(ZMQ REQUIRED libzmq)
pkg_check_modules(GLOG REQUIRED libglog)

# Link libraries globally
link_libraries(${ZMQ_LIBRARIES} ${GLOG_LIBRARIES})

# Include directories
include_directories(include)
include_directories(${GLOG_INCLUDE_DIRS})

# Add subdirectories
add_subdirectory(src/proxy)
add_subdirectory(src/ground)
add_subdirectory(src/sat)

# Add unit tests
add_subdirectory(tests)
