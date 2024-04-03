#include "Crypto/Crypto.h"
#include <algorithm>
#include <boost/random.hpp>
#include <boost/random/random_device.hpp>
#include <boost/random/uniform_int.hpp>
#include <fmt/core.h>
#include <fmt/format.h>
#include <gtest/gtest.h>
TEST(BlockChainCoreTests_Crypto, KeyGenNoThrow) {
  for (int i = 0; i < 100; i++) {
    ASSERT_NO_THROW(BlockChainCore::Crypto::GenerateKeys());
  }
}
TEST(BlockChainCoreTests_Crypto, PublicKeyImportNoThrow) {
  auto keys = BlockChainCore::Crypto::GenerateKeys();
  for (int i = 0; i < 100; i++) {
    ASSERT_NO_THROW(BlockChainCore::Crypto::ConstructPublicKey(keys.first));
  }
}
TEST(BlockChainCoreTests_Crypto, PublicKeyBadImport) {
  auto BadPrivateKey = "BadPrivateKey";
  for (int i = 0; i < 100; i++) {
    ASSERT_ANY_THROW(BlockChainCore::Crypto::ConstructPublicKey(BadPrivateKey));
  }
};
TEST(BlockChainCoreTests_Crypto, PublicKeyImportEq) {
  auto keys = BlockChainCore::Crypto::GenerateKeys();
  for (int i = 0; i < 100; i++) {
    auto publicKey = BlockChainCore::Crypto::ConstructPublicKey(keys.first);
    ASSERT_EQ(publicKey, keys.second);
  }
}
TEST(BlockChainCoreTests_Crypto, PrivateKeyOptimizedCasts) {
  for (int i = 0; i < 100; i++) {
    auto keys = BlockChainCore::Crypto::GenerateKeys();
    ASSERT_NO_THROW(
        BlockChainCore::PrivateKeyOptimizedImpl::FromClassicFormat(keys.first));
    auto optimized =
        BlockChainCore::PrivateKeyOptimizedImpl::FromClassicFormat(keys.first);
    ASSERT_NO_THROW(optimized.ToClassicFormat());
    ASSERT_EQ(keys.first, optimized.ToClassicFormat());
  }
}
TEST(BlockChainCoreTests_Crypto, PublicKeyOptimizedCasts) {
  for (int i = 0; i < 100; i++) {
    auto keys = BlockChainCore::Crypto::GenerateKeys();
    ASSERT_NO_THROW(
        BlockChainCore::PublicKeyOptimizedImpl::FromClassicFormat(keys.second));
    auto optimized =
        BlockChainCore::PublicKeyOptimizedImpl::FromClassicFormat(keys.second);
    ASSERT_NO_THROW(optimized.ToClassicFormat());
    ASSERT_EQ(keys.second, optimized.ToClassicFormat());
  }
}
TEST(BlockChainCoreTests_Crypto, PrivateKeyOptimizedImplBadImport) {
  ASSERT_ANY_THROW(
      BlockChainCore::PrivateKeyOptimizedImpl::FromClassicFormat("Bad key"));
}
TEST(BlockChainCoreTests_Crypto, PublicKeyOptimizedImplBadImport) {
  ASSERT_ANY_THROW(BlockChainCore::PublicKeyOptimizedImpl::FromClassicFormat(
      {"Bad", "Key"}));
}
TEST(BlockChainCoreTests_Crypto, GoodSignIsOk) {
  auto keys = BlockChainCore::Crypto::GenerateKeys();
  BlockChainCore::Crypto::ByteVector vec(1000);
  boost::random::random_device rnd;
  boost::uniform_int<unsigned char> distr(0, 255);
  for (int i = 0; i < 100; i++) {
    std::for_each(vec.begin(), vec.end(),
                  [&rnd, &distr](unsigned char &elem) { elem = distr(rnd); });
    // Один и тот же вектор подписывается с валидацией и без валидации ключа
    auto result = BlockChainCore::Crypto::TryToSign(vec, keys.first, false);
    ASSERT_TRUE(result.has_value());
    result = BlockChainCore::Crypto::TryToSign(vec, keys.first, true);
    ASSERT_TRUE(result.has_value());
  }
}
TEST(BlockChainCoreTests_Crypto, GoodSignIsOkOptimized) {
  auto keys = BlockChainCore::Crypto::GenerateKeys();
  auto privateKeyOptimized =
      BlockChainCore::PrivateKeyOptimizedImpl::FromClassicFormat(keys.first);
  BlockChainCore::Crypto::ByteVector vec(1000);
  boost::random::random_device rnd;
  boost::uniform_int<unsigned char> distr(0, 255);
  for (int i = 0; i < 100; i++) {
    std::for_each(vec.begin(), vec.end(),
                  [&rnd, &distr](unsigned char &elem) { elem = distr(rnd); });
    // Один и тот же вектор подписывается с валидацией и без валидации ключа
    auto result =
        BlockChainCore::Crypto::TryToSignOptimized(vec, privateKeyOptimized);
    ASSERT_TRUE(result.has_value());
  }
}
TEST(BlockChainCoreTests_Crypto, BadKeysSignIsNotOk) {
  BlockChainCore::Crypto::ByteVector vec(1000);
  boost::random::random_device rnd;
  boost::uniform_int<unsigned char> distr(0, 255);
  for (int i = 0; i < 100; i++) {
    std::for_each(vec.begin(), vec.end(),
                  [&rnd, &distr](unsigned char &elem) { elem = distr(rnd); });
    auto result = BlockChainCore::Crypto::TryToSign(vec, "Bad key there", true);
    ASSERT_FALSE(result.has_value());
  }
}
TEST(BlockChainCoreTests_Crypto, GoodVerify) {
  auto keys = BlockChainCore::Crypto::GenerateKeys();
  BlockChainCore::Crypto::ByteVector vec(1000);
  boost::random::random_device rnd;
  boost::uniform_int<unsigned char> distr(0, 255);
  for (int i = 0; i < 100; i++) {
    std::for_each(vec.begin(), vec.end(),
                  [&rnd, &distr](unsigned char &elem) { elem = distr(rnd); });
    auto signedData = BlockChainCore::Crypto::TryToSign(vec, keys.first, true);
    ASSERT_TRUE(signedData.has_value());
    auto &signedDataVal = signedData.value();
    // Один и тот же вектор проверяется с валидацией и без валидации ключа
    auto verified = BlockChainCore::Crypto::TryToVerifyECDSA_CryptoPP(
        signedDataVal, vec, keys.second, true);
    ASSERT_TRUE(verified.has_value());
    verified = BlockChainCore::Crypto::TryToVerifyECDSA_CryptoPP(
        signedDataVal, vec, keys.second, false);
    ASSERT_TRUE(verified.has_value());
  }
}
TEST(BlockChainCoreTests_Crypto, BadSignVerify) {
  auto keys = BlockChainCore::Crypto::GenerateKeys();
  BlockChainCore::Crypto::ByteVector vec(1000);
  boost::random::random_device rnd;
  boost::uniform_int<unsigned char> distr(0, 255);
  for (int i = 0; i < 100; i++) {
    std::for_each(vec.begin(), vec.end(),
                  [&rnd, &distr](unsigned char &elem) { elem = distr(rnd); });
    auto signedData = BlockChainCore::Crypto::TryToSign(vec, keys.first, true);
    ASSERT_TRUE(signedData.has_value());
    BlockChainCore::Crypto::ByteVector signedDataVal = signedData.value();
    signedDataVal[10] = (int(signedDataVal[10]) + 1) % 256;
    // Один и тот же вектор проверяется с валидацией и без валидации ключа
    auto verified = BlockChainCore::Crypto::TryToVerifyECDSA_CryptoPP(
        signedDataVal, vec, keys.second, true);
    ASSERT_FALSE(verified.has_value());
    verified = BlockChainCore::Crypto::TryToVerifyECDSA_CryptoPP(
        signedDataVal, vec, keys.second, false);
    ASSERT_FALSE(verified.has_value());
  }
}
TEST(BlockChainCoreTests_Crypto, BadPublicKeyVerify) {
  auto keys = BlockChainCore::Crypto::GenerateKeys();
  keys.second.first[0] = (int(keys.second.first[0]) + 1) % 256;
  BlockChainCore::Crypto::ByteVector vec(1000);
  boost::random::random_device rnd;
  boost::uniform_int<unsigned char> distr(0, 255);
  for (int i = 0; i < 100; i++) {
    std::for_each(vec.begin(), vec.end(),
                  [&rnd, &distr](unsigned char &elem) { elem = distr(rnd); });
    auto signedData = BlockChainCore::Crypto::TryToSign(vec, keys.first, true);
    ASSERT_TRUE(signedData.has_value());
    auto &signedDataVal = signedData.value();
    auto verified = BlockChainCore::Crypto::TryToVerifyECDSA_CryptoPP(
        signedDataVal, vec, keys.second, true);
    ASSERT_FALSE(verified.has_value());
  }
}
TEST(BlockChainCoreTests_Crypto, GoodVerifyOptimized) {
  auto keys = BlockChainCore::Crypto::GenerateKeys();
  auto privateKeyO =
      BlockChainCore::PrivateKeyOptimizedImpl::FromClassicFormat(keys.first);
  auto publicKeyO =
      BlockChainCore::PublicKeyOptimizedImpl::FromClassicFormat(keys.second);
  BlockChainCore::Crypto::ByteVector vec(1000);
  boost::random::random_device rnd;
  boost::uniform_int<unsigned char> distr(0, 255);
  for (int i = 0; i < 100; i++) {
    std::for_each(vec.begin(), vec.end(),
                  [&rnd, &distr](unsigned char &elem) { elem = distr(rnd); });
    auto signedData =
        BlockChainCore::Crypto::TryToSignOptimized(vec, privateKeyO);
    ASSERT_TRUE(signedData.has_value());
    auto &signedDataVal = signedData.value();
    // Один и тот же вектор проверяется с валидацией и без валидации ключа
    auto verified = BlockChainCore::Crypto::TryToVerifyECDSA_CryptoPPOptimized(
        signedDataVal, vec, publicKeyO);
    ASSERT_TRUE(verified.has_value());
    verified = BlockChainCore::Crypto::TryToVerifyECDSA_CryptoPPOptimized(
        signedDataVal, vec, publicKeyO);
    ASSERT_TRUE(verified.has_value());
  }
}
TEST(BlockChainCoreTests_Crypto, BadSignVerifyOptimized) {
  auto keys = BlockChainCore::Crypto::GenerateKeys();
  auto privateKeyO =
      BlockChainCore::PrivateKeyOptimizedImpl::FromClassicFormat(keys.first);
  auto publicKeyO =
      BlockChainCore::PublicKeyOptimizedImpl::FromClassicFormat(keys.second);
  BlockChainCore::Crypto::ByteVector vec(1000);
  boost::random::random_device rnd;
  boost::uniform_int<unsigned char> distr(0, 255);
  for (int i = 0; i < 100; i++) {
    std::for_each(vec.begin(), vec.end(),
                  [&rnd, &distr](unsigned char &elem) { elem = distr(rnd); });
    auto signedData =
        BlockChainCore::Crypto::TryToSignOptimized(vec, privateKeyO);
    ASSERT_TRUE(signedData.has_value());
    BlockChainCore::Crypto::ByteVector signedDataVal = signedData.value();
    signedDataVal[10] = (int(signedDataVal[10]) + 1) % 256;
    // Один и тот же вектор проверяется с валидацией и без валидации ключа
    auto verified = BlockChainCore::Crypto::TryToVerifyECDSA_CryptoPPOptimized(
        signedDataVal, vec, publicKeyO);
    ASSERT_FALSE(verified.has_value());
    verified = BlockChainCore::Crypto::TryToVerifyECDSA_CryptoPPOptimized(
        signedDataVal, vec, publicKeyO);
    ASSERT_FALSE(verified.has_value());
  }
}
TEST(BlockChainCoreTests_Crypto, SignsAndVerEq) {
  auto keys = BlockChainCore::Crypto::GenerateKeys();
  auto privateKeyO =
      BlockChainCore::PrivateKeyOptimizedImpl::FromClassicFormat(keys.first);
  auto publicKeyO =
      BlockChainCore::PublicKeyOptimizedImpl::FromClassicFormat(keys.second);
  BlockChainCore::Crypto::ByteVector vec(1000);
  boost::random::random_device rnd;
  boost::uniform_int<unsigned char> distr(0, 255);
  for (int i = 0; i < 100; i++) {
    std::for_each(vec.begin(), vec.end(),
                  [&rnd, &distr](unsigned char &elem) { elem = distr(rnd); });
    auto signedDataOptimized =
        BlockChainCore::Crypto::TryToSignOptimized(vec, privateKeyO);
    auto signedData = BlockChainCore::Crypto::TryToSign(vec, keys.first, true);
    auto &signedDataVal = signedData.value();
    auto &signedDataOptimizedVal = signedDataOptimized.value();
    auto verified1 = BlockChainCore::Crypto::TryToVerifyECDSA_CryptoPP(
        signedDataVal, vec, keys.second, true);
    auto verified2 = BlockChainCore::Crypto::TryToVerifyECDSA_CryptoPPOptimized(
        signedDataVal, vec, publicKeyO);
    auto verified3 = BlockChainCore::Crypto::TryToVerifyECDSA_CryptoPP(
        signedDataOptimizedVal, vec, keys.second, true);
    auto verified4 = BlockChainCore::Crypto::TryToVerifyECDSA_CryptoPPOptimized(
        signedDataOptimizedVal, vec, publicKeyO);
    ASSERT_EQ(verified1.value(), verified2.value());
    ASSERT_EQ(verified2.value(), verified3.value());
    ASSERT_EQ(verified3.value(), verified4.value());
  }
}
