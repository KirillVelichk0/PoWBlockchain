#include "Block/Block.h"
#include "Crypto/Crypto.h"
#include <benchmark/benchmark.h>
#include <chrono>
#include <random>
#include <strstream>
#ifdef WASMBUILD
constexpr auto testSZ = 5;
#else
constexpr auto testSZ = 100;
#endif
static void BM_BlockSerializingForHash(benchmark::State &state) {
  BlockChainCore::BlockHashInfo hashInfo;
  auto now = std::chrono::high_resolution_clock::now();
  auto timestamp = now.time_since_epoch().count();
  hashInfo.prevSignedHash = {3, 1, 2, 5, 1, 23, 115};
  auto keys = BlockChainCore::Crypto::GenerateKeys();
  auto consInfo = BlockChainCore::BlockConsensusInfo();
  std::mt19937_64 rnd(
      std::chrono::high_resolution_clock::now().time_since_epoch().count());
  std::uniform_int_distribution<unsigned char> distr(0, 255);
  BlockChainCore::ByteVector data(1000000);
  for (auto &elem : data) {
    elem = distr(rnd);
  }
  BlockChainCore::Block block(hashInfo, timestamp, keys.second,
                              std::uint64_t(3), consInfo, data);
  for (auto _ : state) {
    block.SerializeForHashing();
  }
}
BENCHMARK(BM_BlockSerializingForHash);
static void BM_BlockProtoSerializeString(benchmark::State &state) {
  BlockChainCore::BlockHashInfo hashInfo;
  auto now = std::chrono::high_resolution_clock::now();
  auto timestamp = now.time_since_epoch().count();
  hashInfo.prevSignedHash = {3, 1, 2, 5, 1, 23, 115};
  auto keys = BlockChainCore::Crypto::GenerateKeys();
  auto consInfo = BlockChainCore::BlockConsensusInfo();
  std::mt19937_64 rnd(
      std::chrono::high_resolution_clock::now().time_since_epoch().count());
  std::uniform_int_distribution<unsigned char> distr(0, 255);
  BlockChainCore::ByteVector data(1000000);
  for (auto &elem : data) {
    elem = distr(rnd);
  }
  BlockChainCore::Block block(hashInfo, timestamp, keys.second,
                              std::uint64_t(3), consInfo, data);
  for (auto _ : state) {
    auto proto = BlockChainCore::Block::ConvertToProto(block);
  }
}
BENCHMARK(BM_BlockProtoSerializeString);

static void BM_BlockProtoDeserializeString(benchmark::State &state) {
  BlockChainCore::BlockHashInfo hashInfo;
  auto now = std::chrono::high_resolution_clock::now();
  auto timestamp = now.time_since_epoch().count();
  hashInfo.prevSignedHash = {3, 1, 2, 5, 1, 23, 115};
  auto keys = BlockChainCore::Crypto::GenerateKeys();
  auto consInfo = BlockChainCore::BlockConsensusInfo();
  std::mt19937_64 rnd(
      std::chrono::high_resolution_clock::now().time_since_epoch().count());
  std::uniform_int_distribution<unsigned char> distr(0, 255);
  BlockChainCore::ByteVector data(1000000);
  for (auto &elem : data) {
    elem = distr(rnd);
  }
  BlockChainCore::Block block(hashInfo, timestamp, keys.second,
                              std::uint64_t(3), consInfo, data);
  auto proto = BlockChainCore::Block::ConvertToProto(block);
  for (auto _ : state) {
    BlockChainCore::Block::CreateFromProto(proto.value());
  }
}
BENCHMARK(BM_BlockProtoDeserializeString);

static void BM_BlockProtoSerializeStream(benchmark::State &state) {
  BlockChainCore::BlockHashInfo hashInfo;
  auto now = std::chrono::high_resolution_clock::now();
  auto timestamp = now.time_since_epoch().count();
  hashInfo.prevSignedHash = {3, 1, 2, 5, 1, 23, 115};
  auto keys = BlockChainCore::Crypto::GenerateKeys();
  auto consInfo = BlockChainCore::BlockConsensusInfo();
  std::mt19937_64 rnd(
      std::chrono::high_resolution_clock::now().time_since_epoch().count());
  std::uniform_int_distribution<unsigned char> distr(0, 255);
  BlockChainCore::ByteVector data(1000000);
  for (auto &elem : data) {
    elem = distr(rnd);
  }
  BlockChainCore::Block block(hashInfo, timestamp, keys.second,
                              std::uint64_t(3), consInfo, data);

  for (auto _ : state) {
    std::ostrstream oss;
    auto proto = BlockChainCore::Block::ConvertToProto(block, oss);
  }
}
BENCHMARK(BM_BlockProtoSerializeStream);

static void BM_BlockProtoDeserializeStream(benchmark::State &state) {
  BlockChainCore::BlockHashInfo hashInfo;
  auto now = std::chrono::high_resolution_clock::now();
  auto timestamp = now.time_since_epoch().count();
  hashInfo.prevSignedHash = {3, 1, 2, 5, 1, 23, 115};
  auto keys = BlockChainCore::Crypto::GenerateKeys();
  auto consInfo = BlockChainCore::BlockConsensusInfo();
  std::mt19937_64 rnd(
      std::chrono::high_resolution_clock::now().time_since_epoch().count());
  std::uniform_int_distribution<unsigned char> distr(0, 255);
  BlockChainCore::ByteVector data(1000000);
  for (auto &elem : data) {
    elem = distr(rnd);
  }
  BlockChainCore::Block block(hashInfo, timestamp, keys.second,
                              std::uint64_t(3), consInfo, data);
  std::unique_ptr<char[]> buf = std::make_unique<char[]>(1000000);
  char *rawBuf = buf.get();
  std::ostrstream oss(buf.release(), 1000000);
  auto proto = BlockChainCore::Block::ConvertToProto(block, oss);
  auto count = oss.pcount();
  std::istrstream iss(oss.str(), count);
  for (auto _ : state) {
    auto des = BlockChainCore::Block::CreateFromProto(iss);
    iss = std::istrstream(iss.str(), count);
  }
}
BENCHMARK(BM_BlockProtoDeserializeStream);
