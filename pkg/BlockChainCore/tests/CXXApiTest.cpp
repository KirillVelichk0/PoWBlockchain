#include "Block/Mining.h"
#include "BlockChain_CXX_API/BlockChain_api.h"
#include <gtest/gtest.h>
TEST(BlockChainCoreTests_CXXAPI, MineBlockWithoutComplexity) {
  auto keys = BlockChainCore::GenerateKeys();
  auto startBlock = BlockChainCore::Block::init();
  auto mined = BlockChainCore::MineBlockWithStandartMiner(
      {0, 1, 2, 3, 4}, keys.first, startBlock, true);
  ASSERT_TRUE(mined.has_value());
  mined = BlockChainCore::MineBlockWithStandartMiner(
      {0, 1, 2, 3}, std::move(keys.first), std::move(startBlock), false);
  ASSERT_TRUE(mined.has_value());
}
const auto EmptyCallback = [](const BlockChainCore::Block &,
                              std::array<unsigned char, 32> &&) {
  // nothing to do
};
const auto CounterCallbackFactory = [](std::uint32_t &counter) {
  return [&counter](const BlockChainCore::Block &,
                    BlockChainCore::Crypto::SHA256Hash &&) { counter++; };
};
TEST(BlockChainCoreTests_CXXAPI, MineBlockWithComplexity) {
  auto keys = BlockChainCore::GenerateKeys();
  auto startBlock = BlockChainCore::Block::init();
  auto mined = BlockChainCore::MineBlockWithExternalMiner(
      {0, 1, 2, 3, 4}, keys.first, startBlock, 1, std::chrono::seconds(1), true,
      &BlockChainCore::Crypto::GenerateSHA256, EmptyCallback);
  ASSERT_TRUE(mined.has_value());

  mined = BlockChainCore::MineBlockWithExternalMiner(
      {0, 1, 2, 3}, std::move(keys.first), std::move(startBlock), 1,
      std::chrono::seconds(1), false, &BlockChainCore::Crypto::GenerateSHA256,
      EmptyCallback);
  ASSERT_TRUE(mined.has_value());
}
//! Этот тест может не пройти, т.к. блок, теоретически может не успеть
//! замайнится за указанное время. Однако, это крайне маловероятно. Если тест не
//! проходит, это повод задуматься
TEST(BlockChainCoreTests_CXXAPI, CheckBlockComplexity) {
  auto keys = BlockChainCore::GenerateKeys();
  auto startBlock = BlockChainCore::Block::init();
  std::uint32_t counter = 0;
  auto mined = BlockChainCore::MineBlockWithExternalMiner(
      {0, 1, 2, 3, 4}, keys.first, startBlock, 3, std::chrono::seconds(180),
      true, &BlockChainCore::Crypto::GenerateSHA256,
      CounterCallbackFactory(counter));
  ASSERT_TRUE(mined.has_value());
  ASSERT_TRUE(BlockChainCore::IsBlockMinedCorrectly(mined.value(), 3));
  ASSERT_FALSE(BlockChainCore::IsBlockMinedCorrectly(mined.value(), 25));
  ASSERT_TRUE(counter > 0);
}
