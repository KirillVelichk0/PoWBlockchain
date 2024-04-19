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
      {0, 1, 3, 2}, keys.first, initBlock, false);
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
TEST(BlockChainCoreTests_CAPI, MineBlockAndValidate) {
  std::vector<char> data = {1, 2, 3, 1, 32, 123, 52, 13};
  auto resDeleter = [](Result *res) { FreeResult(res); };
  auto pkDeleter = [](PublicKeyResult *pk) { FreePK(pk); };
  using ResDeleter = decltype(resDeleter);
  using PKDeleter = decltype(pkDeleter);
  std::unique_ptr<Result, ResDeleter> privateKey(GeneratePrivateKey(),
                                                 resDeleter);
  ASSERT_TRUE(privateKey != nullptr);
  auto pkSize = GetResultDataSize(privateKey.get());
  ASSERT_TRUE(pkSize > 0);
  std::unique_ptr<char[]> privateKeyData(GetResultData(privateKey.get()));
  ASSERT_TRUE(privateKeyData.get() != nullptr);
  std::unique_ptr<Result, ResDeleter> startBlock(InitStartBlock());
  ASSERT_TRUE(startBlock != nullptr);
  auto startBlockSz = GetResultDataSize(startBlock.get());
  ASSERT_TRUE(startBlockSz > 0);
  std::unique_ptr<char[]> startBlockData(GetResultData(startBlock.get()));
  ASSERT_TRUE(startBlockData != nullptr);
  std::unique_ptr<Result, ResDeleter> minedBlock(
      MineBlockNonDel(data.data(), data.size(), startBlockData.get(),
                      startBlockSz, privateKeyData.get(), pkSize, true));
  ASSERT_TRUE(minedBlock != nullptr);
  auto minedBlockSz = GetResultDataSize(minedBlock.get());
  ASSERT_TRUE(minedBlockSz > 0);
  std::unique_ptr<char[]> minedBlockData(GetResultData(minedBlock.get()));
  ASSERT_TRUE(minedBlockData != nullptr);
  std::unique_ptr<PublicKeyResult, PKDeleter> publicKey(
      GetPublicKeyFromPrivateNonDel(privateKeyData.get(), pkSize));
  ASSERT_TRUE(publicKey != nullptr);
  auto publicKey1Size = GetFirstKeySize(publicKey.get());
  auto publicKey2Size = GetSecondKeySize(publicKey.get());
  ASSERT_TRUE(publicKey1Size > 0);
  ASSERT_TRUE(publicKey2Size > 0);
  std::unique_ptr<char[]> publicKey1(GetFirstKey(publicKey.get()));
  std::unique_ptr<char[]> publicKey2(GetSecondKey(publicKey.get()));
  ASSERT_TRUE(publicKey1 != nullptr);
  ASSERT_TRUE(publicKey2 != nullptr);
  std::unique_ptr<Result, ResDeleter> validated(
      ValidateBlockSignNonDel(minedBlockData.get(), minedBlockSz, true));
  ASSERT_TRUE(validated != nullptr);
  auto validatedErrorSz = GetResultErrorSize(validated.get());
  ASSERT_TRUE(validatedErrorSz == 0);
  std::unique_ptr<Result, ResDeleter> validatedInit(
      ValidateBlockSignNonDel(startBlockData.get(), startBlockSz, true));
  ASSERT_TRUE(validatedInit != nullptr);
  auto initValidatedErrorSz = GetResultErrorSize(validatedInit.get());
  ASSERT_TRUE(initValidatedErrorSz > 0);
  std::unique_ptr<char[]> validatedInitError(
      GetResultError(validatedInit.get()));
  ASSERT_TRUE(validatedInitError.get() != nullptr);
}
static const auto resDeleter = [](Result *res) { FreeResult(res); };
using ResDeleter = decltype(resDeleter);
TEST(BlockChainCoreTests_CAPI, InitBlockTest) {
  for (auto i = 0; i < 1000; i++) {
    std::unique_ptr<Result, ResDeleter> startBlock(InitStartBlock(),
                                                   resDeleter);
    ASSERT_TRUE(startBlock != nullptr);
    auto startBlockSz = GetResultDataSize(startBlock.get());
    ASSERT_TRUE(startBlockSz > 0);
    std::unique_ptr<char[]> startBlockData(GetResultData(startBlock.get()));
    ASSERT_TRUE(startBlockData != nullptr);
  }
}
