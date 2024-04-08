#include "Block/Mining.h"
#include "BlockChain_CXX_API/BlockChain_api.h"
#include "BlockChain_C_API/BlockChain_api.h"
#include <chrono>
#include <gtest/gtest.h>
TEST(BlockChainCoreTests_CAPI, InitStartBlock) {
  auto startBlock = InitStartBlock();
  ASSERT_TRUE(startBlock.dataSize > 0);
  ASSERT_TRUE(startBlock.errorSize == 0);
  ASSERT_TRUE(startBlock.data != nullptr);
  ASSERT_TRUE(startBlock.error == nullptr);
  std::unique_ptr<char[]> deferData(startBlock.data);
  auto block = BlockChainCore::CreateFromProto(
      std::string(startBlock.data, startBlock.data + startBlock.dataSize));
  ASSERT_TRUE(block.has_value());
}
TEST(BlockChainCoreTests_CAPI, CreatePrivateKey) {
  auto key = GeneratePrivateKey();
  ASSERT_TRUE(key.dataSize > 0);
  ASSERT_TRUE(key.errorSize == 0);
  ASSERT_TRUE(key.data != nullptr);
  ASSERT_TRUE(key.error == nullptr);
  delete[] key.data;
}
TEST(BlockChainCoreTests_CAPI, CreatePublicKey) {
  auto key = GeneratePrivateKey();
  // так то эта проверка уже есть, но плевать
  ASSERT_TRUE(key.dataSize > 0);
  ASSERT_TRUE(key.errorSize == 0);
  ASSERT_TRUE(key.data != nullptr);
  ASSERT_TRUE(key.error == nullptr);
  std::unique_ptr<char[]> deferKey(key.data);
  auto publicKey = GetPublikKeyFromPrivateNonDel(key.data, key.dataSize);
  ASSERT_TRUE(publicKey.error == nullptr);
  ASSERT_TRUE(publicKey.errorSize == 0);
  ASSERT_TRUE(publicKey.publicKey.key1Size > 0);
  ASSERT_TRUE(publicKey.publicKey.key2Size > 0);
  ASSERT_TRUE(publicKey.publicKey.key1 != nullptr);
  ASSERT_TRUE(publicKey.publicKey.key2 != nullptr);
}
TEST(BlockChainCoreTests_CAPI, ValidateBlock) {
  auto initBlock = BlockChainCore::InitStartBlock();
  auto keys = BlockChainCore::GenerateKeys();
  auto mined = BlockChainCore::MineBlockWithStandartMiner(
      {0, 1, 3, 2}, keys.first, initBlock, 0, std::chrono::nanoseconds(0),
      false);
  auto proto = BlockChainCore::ConvertToProto(mined.value());
  auto validateRes = ValidateBlockSignNonDel(proto.value().data(),
                                             proto.value().size(), false);
  ASSERT_TRUE(validateRes.data == nullptr);
  ASSERT_TRUE(validateRes.error == nullptr);
  ASSERT_TRUE(validateRes.dataSize == 0);
  ASSERT_TRUE(validateRes.errorSize == 0);
}
