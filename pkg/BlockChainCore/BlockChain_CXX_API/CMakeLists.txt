cmake_minimum_required(VERSION 3.20)
project(BlockchainCoreCxxAPIProject)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
add_library(CXXAPILib ./BlockChain_api.h ./BlockChain_api.cpp)
target_include_directories(CXXAPILib PUBLIC ../src)
target_link_libraries(CXXAPILib PUBLIC Block)

