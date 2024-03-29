#include "../src/Crypto/Crypto.h"
#include <fmt/core.h>
#include <fmt/format.h>
#include <gtest/gtest.h>
TEST(BlockChainCoreTests_Crypto, KeyGenNoThrow) {
  for (int i = 0; i < 1000; i++) {
    ASSERT_NO_THROW(BlockChainCore::Crypto::GenerateKeys());
  }
}
