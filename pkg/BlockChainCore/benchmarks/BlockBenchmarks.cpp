#include "../src/Block/Block.h"
#include "../src/Crypto/Crypto.h"
#include <benchmark/benchmark.h>
#include <boost/random.hpp>
#include <boost/random/random_device.hpp>
#include <chrono>
static void BM_BlockSerializingForHash(benchmark::State &state) {
  BlockChainCore::BlockHashInfo hashInfo;
  auto now = std::chrono::high_resolution_clock::now();
  auto timestamp = now.time_since_epoch().count();
  hashInfo.prevSignedHash = {3, 1, 2, 5, 1, 23, 115};
  auto keys = BlockChainCore::Crypto::GenerateKeys();
  auto consInfo = BlockChainCore::BlockConsensusInfo();
  boost::random::random_device rnd;
  boost::uniform_int<unsigned char> distr(0, 255);
  BlockChainCore::ByteVector data(3000);
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
static void BM_BlockProtoSerialize(benchmark::State &state) {
  BlockChainCore::BlockHashInfo hashInfo;
  auto now = std::chrono::high_resolution_clock::now();
  auto timestamp = now.time_since_epoch().count();
  hashInfo.prevSignedHash = {3, 1, 2, 5, 1, 23, 115};
  auto keys = BlockChainCore::Crypto::GenerateKeys();
  auto consInfo = BlockChainCore::BlockConsensusInfo();
  boost::random::random_device rnd;
  boost::uniform_int<unsigned char> distr(0, 255);
  BlockChainCore::ByteVector data(3000);
  for (auto &elem : data) {
    elem = distr(rnd);
  }
  BlockChainCore::Block block(hashInfo, timestamp, keys.second,
                              std::uint64_t(3), consInfo, data);
  for (auto _ : state) {
    auto proto = BlockChainCore::Block::ConvertToProto(block);
  }
}
BENCHMARK(BM_BlockProtoSerialize);

static void BM_BlockProtoDeserialize(benchmark::State &state) {
  BlockChainCore::BlockHashInfo hashInfo;
  auto now = std::chrono::high_resolution_clock::now();
  auto timestamp = now.time_since_epoch().count();
  hashInfo.prevSignedHash = {3, 1, 2, 5, 1, 23, 115};
  auto keys = BlockChainCore::Crypto::GenerateKeys();
  auto consInfo = BlockChainCore::BlockConsensusInfo();
  boost::random::random_device rnd;
  boost::uniform_int<unsigned char> distr(0, 255);
  BlockChainCore::ByteVector data(3000);
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
BENCHMARK(BM_BlockProtoDeserialize);
