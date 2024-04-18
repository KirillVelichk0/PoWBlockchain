#include "BenchLib.h"
#include <BlockChain_C_API/BlockChain_api.h>
#include <algorithm>
#include <chrono>
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
void BM_CAPIInitStartBlock(benchmark::State &state) {
  UniqueRes startBlock(nullptr, resDeleter);
  int i = 0;
  for (auto _ : state) {
    startBlock = UniqueRes(InitStartBlock(), resDeleter);
  }
}
