#include "src/Crypto/Crypto.h"
#include <algorithm>
#include <benchmark/benchmark.h>
#include <chrono>
#include <memory>
#include <random>
#include <tuple>
#include <vector>
using ByteVector = BlockChainCore::Crypto::ByteVector;
#ifdef WASMBUILD
constexpr auto testSZ = 5;
#else
constexpr auto testSZ = 100;
#endif
static void BM_KeysGenerate(benchmark::State &state) {
  // Perform setup here
  for (auto _ : state) {
    // This code gets timed
    BlockChainCore::Crypto::GenerateKeys();
  }
}
BENCHMARK(BM_KeysGenerate);
static void BM_PublicKeysConstructing(benchmark::State &state) {
  std::vector<decltype(BlockChainCore::Crypto::GenerateKeys())> keysCont(
      testSZ);
  for (auto &keys : keysCont) {
    keys = BlockChainCore::Crypto::GenerateKeys();
  }
  int i = 0;
  for (auto _ : state) {
    BlockChainCore::Crypto::ConstructPublicKey(keysCont[i].first);
    i = (i + 1) % testSZ;
  }
}
BENCHMARK(BM_PublicKeysConstructing);
static void BM_SignDataWithoutValidating(benchmark::State &state) {
  std::vector<BlockChainCore::Crypto::ByteVector> randomData(testSZ);
  std::mt19937_64 rnd(
      std::chrono::high_resolution_clock::now().time_since_epoch().count());
  std::uniform_int_distribution<unsigned char> distr(0, 255);
  for (auto &byteVector : randomData) {
    byteVector = ByteVector(1000000);
    std::for_each(byteVector.begin(), byteVector.end(),
                  [&rnd, &distr](auto &elem) { elem = distr(rnd); });
  }
  auto keys = BlockChainCore::Crypto::GenerateKeys();
  int i = 0;
  for (auto _ : state) {
    BlockChainCore::Crypto::TryToSign(randomData[i], keys.first, false);
    i = (i + 1) % testSZ;
  }
}
BENCHMARK(BM_SignDataWithoutValidating);
static void BM_SignDataWithValidating(benchmark::State &state) {
  std::vector<BlockChainCore::Crypto::ByteVector> randomData(testSZ);
  std::mt19937_64 rnd(
      std::chrono::high_resolution_clock::now().time_since_epoch().count());
  std::uniform_int_distribution<unsigned char> distr(0, 255);
  for (auto &byteVector : randomData) {
    byteVector = ByteVector(1000000);
    std::for_each(byteVector.begin(), byteVector.end(),
                  [&rnd, &distr](auto &elem) { elem = distr(rnd); });
  }
  auto keys = BlockChainCore::Crypto::GenerateKeys();
  int i = 0;
  for (auto _ : state) {
    BlockChainCore::Crypto::TryToSign(randomData[i], keys.first, true);
    i = (i + 1) % testSZ;
  }
}
BENCHMARK(BM_SignDataWithValidating);
static void BM_VerifyWithValidating(benchmark::State &state) {
  std::vector<std::tuple<ByteVector, ByteVector,
                         decltype(BlockChainCore::Crypto::GenerateKeys())>>
      randomDataAndSigns(testSZ); // данные, подпись, ключ
  std::mt19937_64 rnd(
      std::chrono::high_resolution_clock::now().time_since_epoch().count());
  std::uniform_int_distribution<unsigned char> distr(0, 255);
  for (auto &elem : randomDataAndSigns) {
    auto &[data, sign, keys] = elem;
    data = ByteVector(1000000);
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
    i = (i + 1) % testSZ;
  }
}
BENCHMARK(BM_VerifyWithValidating);
static void BM_VerifyWithoutValidating(benchmark::State &state) {
  std::vector<std::tuple<ByteVector, ByteVector,
                         decltype(BlockChainCore::Crypto::GenerateKeys())>>
      randomDataAndSigns(testSZ); // данные, подпись, ключ
  std::mt19937_64 rnd(
      std::chrono::high_resolution_clock::now().time_since_epoch().count());
  std::uniform_int_distribution<unsigned char> distr(0, 255);
  for (auto &elem : randomDataAndSigns) {
    auto &[data, sign, keys] = elem;
    data = ByteVector(1000000);
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
    i = (i + 1) % testSZ;
  }
}
BENCHMARK(BM_VerifyWithoutValidating);

// there is optimized versions ------------------------
static void BM_PrivateOptimizedKeyConstructing(benchmark::State &state) {
  std::vector<decltype(BlockChainCore::Crypto::GenerateKeys())> keysCont(
      testSZ);
  for (auto &keys : keysCont) {
    keys = BlockChainCore::Crypto::GenerateKeys();
  }
  int i = 0;
  for (auto _ : state) {
    BlockChainCore::PrivateKeyOptimizedImpl::FromClassicFormat(
        keysCont[i].first);
    i = (i + 1) % testSZ;
  }
}
BENCHMARK(BM_PrivateOptimizedKeyConstructing);
static void BM_PublicOptimizedKeysConstructing(benchmark::State &state) {
  std::vector<decltype(BlockChainCore::Crypto::GenerateKeys())> keysCont(
      testSZ);
  for (auto &keys : keysCont) {
    keys = BlockChainCore::Crypto::GenerateKeys();
  }
  int i = 0;
  for (auto _ : state) {
    BlockChainCore::PublicKeyOptimizedImpl::FromClassicFormat(
        keysCont[i].second);
    i = (i + 1) % testSZ;
  }
}
BENCHMARK(BM_PublicOptimizedKeysConstructing);
static void BM_SignDataOptimized(benchmark::State &state) {
  std::vector<BlockChainCore::Crypto::ByteVector> randomData(testSZ);
  std::mt19937_64 rnd(
      std::chrono::high_resolution_clock::now().time_since_epoch().count());
  std::uniform_int_distribution<unsigned char> distr(0, 255);
  for (auto &byteVector : randomData) {
    byteVector = ByteVector(1000000);
    std::for_each(byteVector.begin(), byteVector.end(),
                  [&rnd, &distr](auto &elem) { elem = distr(rnd); });
  }
  auto keys = BlockChainCore::Crypto::GenerateKeys();
  auto privateOptimized =
      BlockChainCore::PrivateKeyOptimizedImpl::FromClassicFormat(keys.first);
  int i = 0;
  for (auto _ : state) {
    BlockChainCore::Crypto::TryToSignOptimized(randomData[i], privateOptimized);
    i = (i + 1) % testSZ;
  }
}
BENCHMARK(BM_SignDataOptimized);

static void BM_VerifyOptimized(benchmark::State &state) {
  std::vector<std::tuple<
      ByteVector, ByteVector,
      std::pair<std::shared_ptr<BlockChainCore::PrivateKeyOptimizedImpl>,
                std::shared_ptr<BlockChainCore::PublicKeyOptimizedImpl>>>>
      randomDataAndSigns(testSZ); // данные, подпись, ключ
  std::mt19937_64 rnd(
      std::chrono::high_resolution_clock::now().time_since_epoch().count());
  std::uniform_int_distribution<unsigned char> distr(0, 255);
  for (auto &elem : randomDataAndSigns) {
    auto &[data, sign, keys] = elem;
    data = ByteVector(1000000);
    std::for_each(data.begin(), data.end(),
                  [&rnd, &distr](auto &elem) { elem = distr(rnd); });
    auto generatedKeys = BlockChainCore::Crypto::GenerateKeys();
    keys = std::make_pair(
        std::make_shared<BlockChainCore::PrivateKeyOptimizedImpl>(
            BlockChainCore::PrivateKeyOptimizedImpl::FromClassicFormat(
                generatedKeys.first)),
        std::make_shared<BlockChainCore::PublicKeyOptimizedImpl>(
            BlockChainCore::PublicKeyOptimizedImpl::FromClassicFormat(
                generatedKeys.second)));
    auto signPacked =
        BlockChainCore::Crypto::TryToSignOptimized(data, *keys.first);
    if (!signPacked.has_value()) {
      state.SkipWithError("Uncorrect sign generated");
    }
    sign = signPacked.value();
  }
  int i = 0;
  for (auto _ : state) {
    auto &[data, sign, keys] = randomDataAndSigns[i];
    BlockChainCore::Crypto::TryToVerifyECDSA_CryptoPPOptimized(sign, data,
                                                               *keys.second);
    i = (i + 1) % testSZ;
  }
}
BENCHMARK(BM_VerifyOptimized);
static void BM_SHA256Gen(benchmark::State &state) {
  std::vector<BlockChainCore::Crypto::ByteVector> randomData(testSZ);
  std::mt19937_64 rnd(
      std::chrono::high_resolution_clock::now().time_since_epoch().count());
  std::uniform_int_distribution<unsigned char> distr(0, 255);
  for (auto &byteVector : randomData) {
    byteVector = ByteVector(1000000);
    std::for_each(byteVector.begin(), byteVector.end(),
                  [&rnd, &distr](auto &elem) { elem = distr(rnd); });
  }
  int i = 0;
  for (auto _ : state) {
    BlockChainCore::Crypto::GenerateSHA256(randomData[i]);
    i = (i + 1) % testSZ;
  }
}
BENCHMARK(BM_SHA256Gen);
