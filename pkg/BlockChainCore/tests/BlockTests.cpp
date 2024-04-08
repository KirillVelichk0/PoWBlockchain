#include "Block/Block.h"
#include <Crypto/Crypto.h>
#include <chrono>
#include <cstdint>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/printf.h>
#include <gtest/gtest.h>
#include <random>
#include <strstream>

TEST(BlockChainCoreTest_Block, HashingTestGood) {
  for (int i = 0; i < 100; i++) {
    BlockChainCore::BlockHashInfo hashInfo;
    auto now = std::chrono::high_resolution_clock::now();
    std::uint64_t timestamp = now.time_since_epoch().count();
    hashInfo.prevSignedHash = {3, 1, 2, 5, 1, 23, 115};
    auto keys = BlockChainCore::Crypto::GenerateKeys();
    auto consInfo = BlockChainCore::BlockConsensusInfo();
    std::mt19937_64 gen(
        std::chrono::high_resolution_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<unsigned int> distr(0, 255);
    BlockChainCore::ByteVector data(1000);
    for (auto &elem : data) {
      elem = distr(gen);
    }
    BlockChainCore::Block block(hashInfo, timestamp, keys.second,
                                std::uint64_t(3), consInfo, data);
    ASSERT_NO_THROW(block.SerializeForHashing());
    auto serialized = block.SerializeForHashing();
    ASSERT_EQ(serialized.size(), block.GetHashingBlockSize());
    ASSERT_EQ(serialized[serialized.size() - 1],
              block.GetContainedData()[block.GetContainedData().size() - 1]);
    auto signedData =
        BlockChainCore::Crypto::TryToSign(serialized, keys.first, false);
    ASSERT_TRUE(signedData.has_value());
    auto verifyResult = BlockChainCore::Crypto::TryToVerifyECDSA_CryptoPP(
        signedData.value(), serialized, keys.second, false);
    ASSERT_TRUE(verifyResult.has_value());
  }
}
void PrintBlock(const BlockChainCore::Block &block, const std::string &name) {
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
             block.GetMinedBy().second, block.GetTimestamp(),
             std::string(block.GetContainedData().begin(),
                         block.GetContainedData().end()));
}
TEST(BlockChainCoreTest_Block, ProtoSerializationStringTest) {
  for (int i = 0; i < 100; i++) {
    BlockChainCore::BlockHashInfo hashInfo;
    auto now = std::chrono::high_resolution_clock::now();
    auto timestamp = now.time_since_epoch().count();
    hashInfo.prevSignedHash = {3, 1, 2, 5, 1, 23, 115};
    auto keys = BlockChainCore::Crypto::GenerateKeys();
    auto consInfo = BlockChainCore::BlockConsensusInfo();
    std::mt19937_64 gen(
        std::chrono::high_resolution_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<unsigned int> distr(0, 255);
    BlockChainCore::ByteVector data(1000);
    for (auto &elem : data) {
      elem = distr(gen);
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
TEST(BlockChainCoreTest_Block, ProtoSerializationStreamTest) {
  for (int i = 0; i < 100; i++) {
    BlockChainCore::BlockHashInfo hashInfo;
    auto now = std::chrono::high_resolution_clock::now();
    auto timestamp = now.time_since_epoch().count();
    hashInfo.prevSignedHash = {3, 1, 2, 5, 1, 23, 115};
    auto keys = BlockChainCore::Crypto::GenerateKeys();
    auto consInfo = BlockChainCore::BlockConsensusInfo();
    std::mt19937_64 gen(
        std::chrono::high_resolution_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<unsigned int> distr(0, 255);
    BlockChainCore::ByteVector data(1000);
    for (auto &elem : data) {
      elem = distr(gen);
    }
    BlockChainCore::Block block(hashInfo, timestamp, keys.second,
                                std::uint64_t(3), consInfo, data);
    std::ostrstream oss;
    auto converingResult = BlockChainCore::Block::ConvertToProto(block, oss);
    ASSERT_TRUE(converingResult.has_value());
    auto count = oss.pcount();
    std::istrstream iss(oss.str(), count);
    auto deserializedProto = BlockChainCore::Block::CreateFromProto(iss);
    ASSERT_TRUE(deserializedProto.has_value());
    ASSERT_EQ(deserializedProto.value(), block);
  }
}
