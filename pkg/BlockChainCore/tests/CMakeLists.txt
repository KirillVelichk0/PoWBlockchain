cmake_minimum_required(VERSION 3.20)
project(TestsProject)
include_directories(../src)
link_directories(../src)
add_executable(Tests ./main.cpp ./CryptoTests.cpp)
target_link_libraries(Tests Crypto fmt::fmt GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)

