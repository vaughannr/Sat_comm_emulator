cmake_minimum_required(VERSION 3.10)
project(SatelliteCommunicationEmulator)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Define the source directories.
set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(TEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tests")

# Gather all C++ source and header files.
file(GLOB_RECURSE ALL_CXX_SOURCE_FILES
    "${SOURCE_DIR}/*.cpp"
    "${INCLUDE_DIR}/*.hpp"
    "${TEST_DIR}/*.cpp"
)

# Locate clang-format.
find_program(CLANG_FORMAT "clang-format")
if(NOT CLANG_FORMAT)
    message(FATAL_ERROR "clang-format not found! Please install it (e.g., 'sudo apt-get install clang-format').")
endif()
message(STATUS "Found clang-format at: ${CLANG_FORMAT}")

# A target to automatically reformat the source files in place.
add_custom_target(format
    COMMAND ${CLANG_FORMAT} -i ${ALL_CXX_SOURCE_FILES}
    COMMENT "Running clang-format to reformat all source files"
    VERBATIM
)

# Uncomment the following target if you want to perform a "lint-check" instead.
add_custom_target(format-check
    COMMAND ${CLANG_FORMAT} --dry-run --Werror ${ALL_CXX_SOURCE_FILES}
    COMMENT "Checking code style with clang-format (no changes will be made)"
    VERBATIM
)

find_program(CLANG_TIDY clang-tidy)
if (CLANG_TIDY)
  message(STATUS "clang-tidy found: ${CLANG_TIDY}")
  set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY}")
  set(CMAKE_CXX_CLANG_TIDY_CHECKS "./.clang-tidy")
  set(CMAKE_CXX_CLANG_TIDY_TARGETS ON)
else()
    message(WARNING "clang-tidy not found")
endif()

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
enable_testing()
add_subdirectory(tests)
