#pragma once
#include "mining_impl.h"
namespace BlockChainCore {
//! Майнинг блока с использованием внешнего майнера. Если complexity = 0, то
// майнится без консенсуса (централизованное создание блока)
template <class SHA256Generator>
[[nodiscard]] tl::expected<Block, NestedError> MineBlockWithExternalMiner(
    const ByteVector &data, const std::string &privateKey,
    const Block &previousBlock, std::uint32_t complexity,
    std::chrono::nanoseconds miningInterval, bool needToValidateKey,
    SHA256Generator &&generator) noexcept {
  return MineBlockImpl(data, privateKey, previousBlock, needToValidateKey,
                       std::forward<SHA256Generator>(generator));
}
//! Майнинг блока с использованием внешнего майнера. Если complexity = 0, то
// майнится без консенсуса (централизованное создание блока)
template <class SHA256Generator>
[[nodiscard]] tl::expected<Block, NestedError> MineBlockWithExternalMiner(
    ByteVector &&data, std::string &&privateKey, Block &&previousBlock,
    std::uint32_t complexity, std::chrono::nanoseconds miningInterval,
    bool needToValidateKey, SHA256Generator &&generator) noexcept {
  return MineBlockImpl(std::move(data), std::move(privateKey),
                       std::move(previousBlock), complexity,
                       std::forward<SHA256Generator>(generator));
}
//! Майнинг блока с использованием встроенного майнера. Если complexity = 0, то
// майнится без консенсуса (централизованное создание блока)
[[nodiscard]] tl::expected<Block, NestedError> MineBlockWithStandartMiner(
    const ByteVector &data, const std::string &privateKey,
    const Block &previousBlock, std::uint32_t complexity,
    std::chrono::nanoseconds miningInterval, bool needToValidateKey) noexcept;
//! Майнинг блока с использованием встроенного майнера. Если complexity = 0, то
// майнится без консенсуса (централизованное создание блока)
[[nodiscard]] tl::expected<Block, NestedError>
MineBlockWithStandartMiner(ByteVector &&data, std::string &&privateKey,
                           Block &&previousBlock, std::uint32_t complexity,
                           std::chrono::nanoseconds miningInterval,
                           bool needToValidateKey) noexcept;
//! Проверка блока на соблюдение complexity
[[nodiscard]] bool IsBlockMinedCorrectly(Block &block,
                                         std::uint32_t complexity);
} // namespace BlockChainCore
