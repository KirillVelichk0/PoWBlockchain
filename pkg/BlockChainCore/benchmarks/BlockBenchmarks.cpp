#include "../src/Block/Block.h"
#include "../src/Crypto/Crypto.h"
#include <benchmark/benchmark.h>
#include <boost/random.hpp>
#include <boost/random/random_device.hpp>
#include <chrono>
static void BM_BlockSerializingForHash(benchmark::State &state) {
  BlockChainCore::BlockHashInfo hashInfo;
  auto now = std::chrono::high_resolution_clock::now();
  BlockChainCore::UnixTime timestamp = boost::posix_time::from_time_t(
      std::chrono::high_resolution_clock::to_time_t(now));
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
