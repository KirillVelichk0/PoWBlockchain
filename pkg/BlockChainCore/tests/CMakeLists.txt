cmake_minimum_required(VERSION 3.20)
project(TestsProject)
add_executable(Tests ./main.cpp ./CryptoTests.cpp ./BlockTests.cpp)
target_include_directories(Tests PUBLIC ../src)
target_link_libraries(Tests Crypto Block Boost::boost Boost::random fmt::fmt GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
add_test(NAME BlockChainCoreTests COMMAND Tests)

