# Sattelite application
add_executable(sat main.cpp satellite.cpp)
# target_link_libraries(sat zmq ${GlOG_LIBRARIES})
add_dependencies(sat generate_protobuf_files)

add_library(satcom_proto STATIC ${CMAKE_BINARY_DIR}/include/satcom_messages.pb.cc)
target_include_directories(satcom_proto PRIVATE ${CMAKE_BINARY_DIR}/include)

target_link_libraries(sat
  PRIVATE satcom_proto
  ${ZeroMQ_LIBRARIES}
  glog::glog
  protobuf::libprotobuf
)