#include "Block/Mining.h"
#include "api.h"
#include <gtest/gtest.h>
TEST(BlockChainCoreTests_CXXAPI, MineBlockWithoutComplexity) {
  auto keys = BlockChainCore::GenerateKeys();
  auto startBlock = BlockChainCore::Block::init();
  auto mined = BlockChainCore::MineBlockWithStandartMiner(
      {0, 1, 2, 3, 4}, keys.first, startBlock, 0, std::chrono::nanoseconds(100),
      true);
  ASSERT_TRUE(mined.has_value());
  mined = BlockChainCore::MineBlockWithStandartMiner(
      {0, 1, 2, 3}, std::move(keys.first), std::move(startBlock), 0,
      std::chrono::nanoseconds(100), false);
  ASSERT_TRUE(mined.has_value());
}
TEST(BlockChainCoreTests_CXXAPI, MineBlockWithComplexity) {
  auto keys = BlockChainCore::GenerateKeys();
  auto startBlock = BlockChainCore::Block::init();
  auto mined = BlockChainCore::MineBlockWithStandartMiner(
      {0, 1, 2, 3, 4}, keys.first, startBlock, 1, std::chrono::seconds(1),
      true);
  ASSERT_TRUE(mined.has_value());

  mined = BlockChainCore::MineBlockWithStandartMiner(
      {0, 1, 2, 3}, std::move(keys.first), std::move(startBlock), 1,
      std::chrono::seconds(1), false);
  ASSERT_TRUE(mined.has_value());
}
//! Этот тест может не пройти, т.к. блок, теоретически может не успеть
//! замайнится за указанное время. Однако, это крайне маловероятно. Если тест не
//! проходит, это повод задуматься
TEST(BlockChainCoreTests_CXXAPI, CheckBlockComplexity) {
  auto keys = BlockChainCore::GenerateKeys();
  auto startBlock = BlockChainCore::Block::init();
  auto mined = BlockChainCore::MineBlockWithStandartMiner(
      {0, 1, 2, 3, 4}, keys.first, startBlock, 3, std::chrono::seconds(180),
      true);
  ASSERT_TRUE(mined.has_value());
  ASSERT_TRUE(BlockChainCore::IsBlockMinedCorrectly(mined.value(), 3));
  ASSERT_FALSE(BlockChainCore::IsBlockMinedCorrectly(mined.value(), 25));
}
