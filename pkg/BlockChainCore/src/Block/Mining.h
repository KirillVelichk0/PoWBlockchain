#pragma once
#include "mining_impl.h"
#include <concepts>
#include <functional>
namespace BlockChainCore {
template <class Functor>
concept UnsuccessCallbackConcept =

    requires(Functor functor) {
      {
        functor
      } -> std::convertible_to<std::function<void(
          const BlockChainCore::Block &, std::array<unsigned char, 32> &&)>>;
    };
//! Майнинг блока с использованием внешнего майнера. Если complexity = 0, то
// майнится без консенсуса (централизованное создание блока)
template <class SHA256Generator,
          UnsuccessCallbackConcept UnsuccessMiningCallback>
[[nodiscard]] tl::expected<Block, NestedError> MineBlockWithExternalMiner(
    const ByteVector &data, const std::string &privateKey,
    const Block &previousBlock, std::uint32_t complexity,
    std::chrono::nanoseconds miningInterval, bool needToValidateKey,
    SHA256Generator &&generator, UnsuccessMiningCallback &&callback) noexcept {
  return MineBlockImpl(data, privateKey, previousBlock, complexity,
                       miningInterval, needToValidateKey,
                       std::forward<SHA256Generator>(generator),
                       std::forward<UnsuccessMiningCallback>(callback));
}
//! Майнинг блока с использованием внешнего майнера. Если complexity = 0, то
// майнится без консенсуса (централизованное создание блока)
template <class SHA256Generator,
          UnsuccessCallbackConcept UnsuccessMiningCallback>
[[nodiscard]] tl::expected<Block, NestedError>
MineBlockWithExternalMiner(ByteVector &&data, std::string &&privateKey,
                           Block &&previousBlock, uint32_t complexity,
                           std::chrono::nanoseconds miningInterval,
                           bool needToValidateKey, SHA256Generator &&generator,
                           UnsuccessMiningCallback &&callback) noexcept {
  return MineBlockImpl(std::move(data), std::move(privateKey),
                       std::move(previousBlock), complexity, miningInterval,
                       needToValidateKey,
                       std::forward<SHA256Generator>(generator),
                       std::forward<UnsuccessMiningCallback>(callback));
}
//! Майнинг блока с использованием встроенного майнера. Если complexity = 0, то
// майнится без консенсуса (централизованное создание блока)
[[nodiscard]] tl::expected<Block, NestedError> MineBlockWithStandartMiner(
    const ByteVector &data, const std::string &privateKey,
    const Block &previousBlock, bool needToValidateKey) noexcept;
//! Майнинг блока с использованием встроенного майнера. Если complexity = 0, то
// майнится без консенсуса (централизованное создание блока)
[[nodiscard]] tl::expected<Block, NestedError>
MineBlockWithStandartMiner(ByteVector &&data, std::string &&privateKey,
                           Block &&previousBlock,
                           bool needToValidateKey) noexcept;
//! Проверка блока на соблюдение complexity
[[nodiscard]] bool IsBlockMinedCorrectly(Block &block,
                                         std::uint32_t complexity);
} // namespace BlockChainCore
