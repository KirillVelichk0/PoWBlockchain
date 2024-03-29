#include "../src/Crypto/Crypto.h"
#include <algorithm>
#include <benchmark/benchmark.h>
#include <boost/random.hpp>
#include <boost/random/random_device.hpp>
#include <tuple>
#include <vector>
using ByteVector = BlockChainCore::Crypto::ByteVector;

static void BM_KeysGenerate(benchmark::State &state) {
  // Perform setup here
  for (auto _ : state) {
    // This code gets timed
    BlockChainCore::Crypto::GenerateKeys();
  }
}
BENCHMARK(BM_KeysGenerate);
static void BM_PublicKeysConstructing(benchmark::State &state) {
  std::vector<decltype(BlockChainCore::Crypto::GenerateKeys())> keysCont(100);
  for (auto &keys : keysCont) {
    keys = BlockChainCore::Crypto::GenerateKeys();
  }
  int i = 0;
  for (auto _ : state) {
    BlockChainCore::Crypto::ConstructPublicKey(keysCont[i].first);
    i = (i + 1) % 100;
  }
}
BENCHMARK(BM_PublicKeysConstructing);
static void BM_SignDataWithoutValidating(benchmark::State &state) {
  std::vector<BlockChainCore::Crypto::ByteVector> randomData(100);
  boost::random::random_device rnd;
  boost::uniform_int<unsigned char> distr(0, 255);
  for (auto &byteVector : randomData) {
    byteVector = ByteVector(1000);
    std::for_each(byteVector.begin(), byteVector.end(),
                  [&rnd, &distr](auto &elem) { elem = distr(rnd); });
  }
  auto keys = BlockChainCore::Crypto::GenerateKeys();
  int i = 0;
  for (auto _ : state) {
    BlockChainCore::Crypto::TryToSign(randomData[i], keys.first, false);
    i = (i + 1) % 100;
  }
}
BENCHMARK(BM_SignDataWithoutValidating);
static void BM_SignDataWithValidating(benchmark::State &state) {
  std::vector<BlockChainCore::Crypto::ByteVector> randomData(100);
  boost::random::random_device rnd;
  boost::uniform_int<unsigned char> distr(0, 255);
  for (auto &byteVector : randomData) {
    byteVector = ByteVector(1000);
    std::for_each(byteVector.begin(), byteVector.end(),
                  [&rnd, &distr](auto &elem) { elem = distr(rnd); });
  }
  auto keys = BlockChainCore::Crypto::GenerateKeys();
  int i = 0;
  for (auto _ : state) {
    BlockChainCore::Crypto::TryToSign(randomData[i], keys.first, true);
    i = (i + 1) % 100;
  }
}
BENCHMARK(BM_SignDataWithValidating);
static void BM_VerifyWithValidating(benchmark::State &state) {
  std::vector<std::tuple<ByteVector, ByteVector,
                         decltype(BlockChainCore::Crypto::GenerateKeys())>>
      randomDataAndSigns(100); // данные, подпись, ключ
  boost::random::random_device rnd;
  boost::uniform_int<unsigned char> distr(0, 255);
  for (auto &elem : randomDataAndSigns) {
    auto &[data, sign, keys] = elem;
    data = ByteVector(1000);
    std::for_each(data.begin(), data.end(),
                  [&rnd, &distr](auto &elem) { elem = distr(rnd); });
    keys = BlockChainCore::Crypto::GenerateKeys();
    auto signPacked =
        BlockChainCore::Crypto::TryToSign(data, keys.first, false);
    if (!signPacked.has_value()) {
      state.SkipWithError("Uncorrect sign generated");
    }
    sign = signPacked.value();
  }
  int i = 0;
  for (auto _ : state) {
    auto &[data, sign, keys] = randomDataAndSigns[i];
    BlockChainCore::Crypto::TryToVerifyECDSA_CryptoPP(sign, data, keys.second,
                                                      true);
    i = (i + 1) % 100;
  }
}
BENCHMARK(BM_VerifyWithValidating);
static void BM_VerifyWithoutValidating(benchmark::State &state) {
  std::vector<std::tuple<ByteVector, ByteVector,
                         decltype(BlockChainCore::Crypto::GenerateKeys())>>
      randomDataAndSigns(100); // данные, подпись, ключ
  boost::random::random_device rnd;
  boost::uniform_int<unsigned char> distr(0, 255);
  for (auto &elem : randomDataAndSigns) {
    auto &[data, sign, keys] = elem;
    data = ByteVector(1000);
    std::for_each(data.begin(), data.end(),
                  [&rnd, &distr](auto &elem) { elem = distr(rnd); });
    keys = BlockChainCore::Crypto::GenerateKeys();
    auto signPacked =
        BlockChainCore::Crypto::TryToSign(data, keys.first, false);
    if (!signPacked.has_value()) {
      state.SkipWithError("Uncorrect sign generated");
    }
    sign = signPacked.value();
  }
  int i = 0;
  for (auto _ : state) {
    auto &[data, sign, keys] = randomDataAndSigns[i];
    BlockChainCore::Crypto::TryToVerifyECDSA_CryptoPP(sign, data, keys.second,
                                                      false);
    i = (i + 1) % 100;
  }
}
BENCHMARK(BM_VerifyWithoutValidating);
