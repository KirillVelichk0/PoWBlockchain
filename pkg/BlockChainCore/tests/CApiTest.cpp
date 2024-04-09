#include "Block/Mining.h"
#include "BlockChain_CXX_API/BlockChain_api.h"
#include "BlockChain_C_API/BlockChain_api.h"
#include <chrono>
#include <cstdint>
#include <gtest/gtest.h>
TEST(BlockChainCoreTests_CAPI, InitStartBlock) {
  void (*del)(Result *) = [](Result *res) { FreeResult(res); };
  auto startBlock =
      std::unique_ptr<Result, void (*)(Result *)>(InitStartBlock(), del);
  ASSERT_TRUE(GetResultErrorSize(startBlock.get()) == 0);
  auto sz = GetResultDataSize(startBlock.get());
  ASSERT_TRUE(sz > 0);
  std::unique_ptr<char[]> deferData(GetResultData(startBlock.get()));
  auto block = BlockChainCore::CreateFromProto(
      std::string(deferData.get(), deferData.get() + sz));
  ASSERT_TRUE(block.has_value());
}
TEST(BlockChainCoreTests_CAPI, CreatePrivateKey) {
  void (*del)(Result *) = [](Result *pk) { FreeResult(pk); };
  std::unique_ptr<Result, void (*)(Result *)> key(GeneratePrivateKey(), del);
  ASSERT_TRUE(GetResultDataSize(key.get()) > 0);
  ASSERT_TRUE(GetResultErrorSize(key.get()) == 0);
  std::unique_ptr<char[]> deferData(GetResultData(key.get()));
  ASSERT_TRUE(deferData != nullptr);
}
TEST(BlockChainCoreTests_CAPI, CreatePublicKey) {
  void (*del)(Result *) = [](Result *pk) { FreeResult(pk); };
  std::unique_ptr<Result, void (*)(Result *)> key(GeneratePrivateKey(), del);
  // так то эта проверка уже есть, но плевать
  uint32_t keySize = GetResultDataSize(key.get());
  ASSERT_TRUE(keySize > 0);
  ASSERT_TRUE(GetResultErrorSize(key.get()) == 0);
  std::unique_ptr<char[]> deferData(GetResultData(key.get()));
  ASSERT_TRUE(deferData != nullptr);
  std::unique_ptr<PublicKeyResult, void (*)(PublicKeyResult *)> publicKey(
      GetPublicKeyFromPrivateNonDel(deferData.get(), keySize),
      [](PublicKeyResult *pk) { FreePK(pk); });
  ASSERT_TRUE(GetPKErrorSize(publicKey.get()) == 0);
  auto key1Size = GetFirstKeySize(publicKey.get());
  auto key2Size = GetSecondKeySize(publicKey.get());
  ASSERT_TRUE(key1Size > 0);
  ASSERT_TRUE(key2Size > 0);
  std::unique_ptr<char[]> key1(GetFirstKey(publicKey.get()));
  std::unique_ptr<char[]> key2(GetSecondKey(publicKey.get()));
  ASSERT_TRUE(key1 != nullptr);
  ASSERT_TRUE(key2 != nullptr);
}
TEST(BlockChainCoreTests_CAPI, ValidateBlock) {
  auto initBlock = BlockChainCore::InitStartBlock();
  auto keys = BlockChainCore::GenerateKeys();
  auto mined = BlockChainCore::MineBlockWithStandartMiner(
      {0, 1, 3, 2}, keys.first, initBlock, 0, std::chrono::nanoseconds(0),
      false);
  auto proto = BlockChainCore::ConvertToProto(mined.value());
  std::unique_ptr<Result, void (*)(Result *)> validateRes(
      ValidateBlockSignNonDel(proto.value().data(), proto.value().size(),
                              false),
      [](Result *res) { FreeResult(res); });
  ASSERT_TRUE(GetResultErrorSize(validateRes.get()) == 0);
  auto valSz = GetResultDataSize(validateRes.get());
  ASSERT_TRUE(valSz == 0);
  std::unique_ptr<char[]> data(GetResultData(validateRes.get()));
  ASSERT_TRUE(data.get() == nullptr);
}
