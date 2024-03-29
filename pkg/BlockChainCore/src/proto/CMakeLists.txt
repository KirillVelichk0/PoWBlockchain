cmake_minimum_required(VERSION 3.20)
project(ProtoProject)
add_library(Proto ./gen/BlockExternal.pb.h ./gen/BlockExternal.pb.cc)
target_link_libraries(Proto protobuf::libprotobuf)
