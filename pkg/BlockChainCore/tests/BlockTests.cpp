#include "../src/Block/Block.h"
#include <../src/Crypto/Crypto.h>
#include <boost/date_time.hpp>
#include <boost/date_time/posix_time/conversion.hpp>
#include <boost/random.hpp>
#include <boost/random/random_device.hpp>
#include <chrono>
#include <gtest/gtest.h>
#include <iostream>
TEST(BlockChainCoreTest_Block, HashingTestGood) {
  for (int i = 0; i < 100; i++) {
    BlockChainCore::BlockHashInfo hashInfo;
    auto now = std::chrono::high_resolution_clock::now();
    BlockChainCore::UnixTime timestamp = boost::posix_time::from_time_t(
        std::chrono::high_resolution_clock::to_time_t(now));
    hashInfo.prevSignedHash = {3, 1, 2, 5, 1, 23, 115};
    auto keys = BlockChainCore::Crypto::GenerateKeys();
    auto consInfo = BlockChainCore::BlockConsensusInfo();
    boost::random::random_device rnd;
    boost::uniform_int<unsigned char> distr(0, 255);
    BlockChainCore::ByteVector data(1000);
    for (auto &elem : data) {
      elem = distr(rnd);
    }
    BlockChainCore::Block block(hashInfo, timestamp, keys.second,
                                std::uint64_t(3), consInfo, data);
    ASSERT_NO_THROW(block.SerializeForHashing());
    auto serialized = block.SerializeForHashing();
    ASSERT_EQ(serialized.size(), block.GetHashingBlockSize());
    auto signedData =
        BlockChainCore::Crypto::TryToSign(serialized, keys.first, false);
    ASSERT_TRUE(signedData.has_value());
    auto verifyResult = BlockChainCore::Crypto::TryToVerifyECDSA_CryptoPP(
        signedData.value(), serialized, keys.second, false);
    ASSERT_TRUE(verifyResult.has_value());
  }
}
