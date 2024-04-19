#include "Mining.h"
#include "Crypto.h"
#include "mining_impl.h"
namespace BlockChainCore {
auto MineWithCrypto(const ByteVector &data) {
  return Crypto::GenerateSHA256(data);
}
const auto EmptyCallback = [](Block &block, Crypto::SHA256Hash &&hash) {
  // nothing to do
};
[[nodiscard]] tl::expected<Block, NestedError> MineBlockWithStandartMiner(
    const ByteVector &data, const std::string &privateKey,
    const Block &previousBlock, bool needToValidateKey) noexcept {
  return MineBlockImpl(data, privateKey, previousBlock, 0,
                       std::chrono::nanoseconds(0), needToValidateKey,
                       &MineWithCrypto, EmptyCallback);
}

[[nodiscard]] tl::expected<Block, NestedError>
MineBlockWithStandartMiner(ByteVector &&data, std::string &&privateKey,
                           Block &&previousBlock,
                           bool needToValidateKey) noexcept {
  return MineBlockImpl(std::move(data), std::move(privateKey),
                       std::move(previousBlock), 0, std::chrono::nanoseconds(0),
                       needToValidateKey, &MineWithCrypto, EmptyCallback);
}
bool IsBlockMinedCorrectly(Block &block, std::uint32_t complexity) {
  auto blockHash = Crypto::GenerateSHA256(block.SerializeForHashing());
  const auto endIt = std::next(blockHash.begin(), complexity);
  return std::find_if_not(blockHash.begin(), endIt, [](unsigned char byte) {
           return byte == 0;
         }) == endIt;
}
} // namespace BlockChainCore
