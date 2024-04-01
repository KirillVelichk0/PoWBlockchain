#include "../src/Block/Block.h"
#include <../src/Crypto/Crypto.h>
#include <boost/date_time.hpp>
#include <boost/date_time/posix_time/conversion.hpp>
#include <boost/random.hpp>
#include <boost/random/random_device.hpp>
#include <chrono>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/printf.h>
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
void PrintBlock(const BlockChainCore::Block &block, const std::string &name) {
  BlockChainCore::UnixTime epoch(boost::gregorian::date(1970, 1, 1));
  // если вдруг кто-то умудрится выставить время до 1970 года, то ловим UB.
  // Хы.
  std::uint64_t nanoSecondsSinseEpoch =
      (block.GetTimestamp() - epoch).total_nanoseconds();
  fmt::print("Current block is {}\n CurHashInfo: {}\n PrevHashInfo: {}\n "
             "CurLedgerId: {}\n Current luck: {}\n Current miningPoint "
             "{}\n Mined by: {}:{}\n Time: {}\n BlockData: {}\n\n",
             name,
             std::string(block.GetHashInfo().curSignedHash.begin(),
                         block.GetHashInfo().curSignedHash.end()),
             std::string(block.GetHashInfo().prevSignedHash.begin(),
                         block.GetHashInfo().prevSignedHash.end()),
             block.GetTransactionId(), block.GetConsensusInfo().luck,
             block.GetConsensusInfo().miningPoint, block.GetMinedBy().first,
             block.GetMinedBy().second, nanoSecondsSinseEpoch,
             std::string(block.GetContainedData().begin(),
                         block.GetContainedData().end()));
}
TEST(BlockChainCoreTest_Block, ProtoSerializationStringTest) {
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
    auto proto = BlockChainCore::Block::ConvertToProto(block);

    ASSERT_TRUE(proto.has_value());
    auto deserializedProto =
        BlockChainCore::Block::CreateFromProto(proto.value());
    ASSERT_TRUE(deserializedProto.has_value());
    ASSERT_EQ(deserializedProto.value(), block);
  }
}
