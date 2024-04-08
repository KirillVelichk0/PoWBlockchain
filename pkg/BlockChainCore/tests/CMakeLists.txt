cmake_minimum_required(VERSION 3.20)
project(TestsProject)
add_executable(Tests ./main.cpp ./CryptoTests.cpp ./BlockTests.cpp ./CXXApiTest.cpp ./CApiTest.cpp)
target_include_directories(Tests PUBLIC ../src)
target_include_directories(Tests PUBLIC ..)
target_link_libraries(Tests C_APILib CXXAPILib Crypto Block fmt::fmt GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
add_test(NAME BlockChainCoreTests COMMAND Tests)

