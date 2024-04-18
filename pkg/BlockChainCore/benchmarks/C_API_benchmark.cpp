#include "BenchLib.h"
#include <BlockChain_C_API/BlockChain_api.h>
#include <algorithm>
#include <benchmark/benchmark.h>
#include <chrono>
#include <iostream>
#include <memory>
#include <random>
#include <vector>
static const auto resDeleter = [](Result *result) { FreeResult(result); };
using ResDeleter = decltype(resDeleter);
using UniqueRes = std::unique_ptr<Result, void (*)(Result *)>;
static const auto pkDeleter = [](PublicKeyResult *pk) { FreePK(pk); };
using PKDeleter = decltype(pkDeleter);
using UniquePK = std::unique_ptr<PublicKeyResult, PKDeleter>;
const auto randomUnsignedCharGenerator =
    [generator = std::mt19937_64(std::chrono::high_resolution_clock::now()
                                     .time_since_epoch()
                                     .count())]() mutable -> unsigned char {
  std::uniform_int_distribution<unsigned char> distr(0, 255);
  return distr(generator);
};
static void BM_CAPIPrivateKeyGenerate(benchmark::State &state) {
  // Perform setup here
  for (auto _ : state) {
    // This code gets timed
    UniqueRes prKey(GeneratePrivateKey(), resDeleter);
  }
}
BENCHMARK(BM_CAPIPrivateKeyGenerate);
static void BM_CAPIPublicKeyGenerate(benchmark::State &state) {
  UniqueRes prKey(GeneratePrivateKey(), resDeleter);
  std::unique_ptr<char[]> prData(GetResultData(prKey.get()));
  auto prKeySz = GetResultDataSize(prKey.get());
  for (auto _ : state) {
    UniquePK pubKey(GetPublicKeyFromPrivateNonDel(prData.get(), prKeySz),
                    pkDeleter);
  }
}
BENCHMARK(BM_CAPIPublicKeyGenerate);
static void BM_CAPIPrivateKeyExtract(benchmark::State &state) {
  UniqueRes prKey(GeneratePrivateKey(), resDeleter);
  for (auto _ : state) {
    std::unique_ptr<char[]> prKeyData(GetResultData(prKey.get()));
  }
}
BENCHMARK(BM_CAPIPrivateKeyExtract);
BENCHMARK(BM_CAPIInitStartBlock);
static void BM_CAPIGenerateBlockWithoutKeyCheck(benchmark::State &state) {
  std::unique_ptr<Result, ResDeleter> startBlock(InitStartBlock(), resDeleter);
  auto startBlockSz = GetResultDataSize(startBlock.get());
  std::unique_ptr<char[]> startBlockData(GetResultData(startBlock.get()));
  UniqueRes prKey(GeneratePrivateKey(), resDeleter);
  std::unique_ptr<char[]> prData(GetResultData(prKey.get()));
  auto prKeySz = GetResultDataSize(prKey.get());
  std::vector<unsigned char> data(1000000);
  std::generate(data.begin(), data.end(), randomUnsignedCharGenerator);
  for (auto _ : state) {
    UniqueRes mined(MineBlockNonDel((char *)data.data(), data.size(),
                                    startBlockData.get(), startBlockSz,
                                    prData.get(), prKeySz, 0, 1000, false),
                    resDeleter);
  }
}
BENCHMARK(BM_CAPIGenerateBlockWithoutKeyCheck);
static void BM_CAPIGenerateBlockWithKeyCheck(benchmark::State &state) {
  std::unique_ptr<Result, ResDeleter> startBlock(InitStartBlock());
  auto startBlockSz = GetResultDataSize(startBlock.get());
  std::unique_ptr<char[]> startBlockData(GetResultData(startBlock.get()));
  UniqueRes prKey(GeneratePrivateKey(), resDeleter);
  std::unique_ptr<char[]> prData(GetResultData(prKey.get()));
  auto prKeySz = GetResultDataSize(prKey.get());
  std::vector<unsigned char> data(1000000);
  std::generate(data.begin(), data.end(), randomUnsignedCharGenerator);
  for (auto _ : state) {
    UniqueRes mined(MineBlockNonDel((char *)data.data(), data.size(),
                                    startBlockData.get(), startBlockSz,
                                    prData.get(), prKeySz, 0, 1000, true),
                    resDeleter);
  }
}
BENCHMARK(BM_CAPIGenerateBlockWithKeyCheck);
