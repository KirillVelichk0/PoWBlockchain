cmake_minimum_required(VERSION 3.25)
project(BlockChainCore)

set(CMAKE_CXX_STANDARD 20)
find_package(Boost REQUIRED COMPONENTS serialization contract random)
find_package(cryptopp CONFIG REQUIRED)
add_library(BlockChainCore Block/Block.cpp Block/Block.h Crypto/Crypto.h Crypto/Crypto.cpp Block/BlockContainedData.h Block/BlockContainedData.cpp Logger/ILogger.cpp Logger/ILogger.h Predefs/predefs.h Logger/DefaultLoggers.h Logger/DefaultLoggers.cpp)
target_link_libraries(BlockChainCore PRIVATE Boost::boost Boost::random Boost::serialization Boost::contract cryptopp::cryptopp)