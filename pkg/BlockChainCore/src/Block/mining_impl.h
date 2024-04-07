#pragma once
#include "Block.h"
#include "Crypto.h"
#include <fmt/core.h>
#include <fmt/format.h>
namespace BlockChainCore {
template <class SHA256Generator>
[[nodiscard]] tl::expected<Block, NestedError>
MineBlockImpl(const ByteVector &data, const std::string &privateKey,
              const Block &previousBlock, std::uint32_t complexity,
              std::chrono::nanoseconds miningInterval, bool needToValidateKey,
              SHA256Generator &&generator) noexcept {
  auto loc = std::source_location::current();
  try {
    // Так как используется SHA256, число 0 не может быть больше 32
    if (complexity > 32) {
      return tl::unexpected(NestedError(
          fmt::format("Max size of complexcity is 32. Cur complexcity - {}",
                      complexity),
          loc));
    }
    return Block::PrepareBlock(previousBlock)
        .map_error([loc](NestedError &&error) -> NestedError {
          return {"Cant mine block because cant prepare", std::move(error),
                  loc};
        })
        .and_then([&data, &privateKey, complexity, miningInterval,
                   needToValidateKey, loc,
                   generator = std::forward<SHA256Generator>(generator)](
                      Block &&block) -> tl::expected<Block, NestedError> {
          block.SetContainedData(data);
          block.SetMinedBy(Crypto::ConstructPublicKey(privateKey));
          if (complexity != 0) {
            const auto miningStartTime =
                std::chrono::high_resolution_clock::now();
            const auto miningEndTime = miningStartTime + miningInterval;
            auto now = miningStartTime;
            for (; now < miningEndTime;
                 now = std::chrono::high_resolution_clock::now()) {
              auto blockHash = generator(block.SerializeForHashing());
              const auto endIt = std::next(blockHash.begin(), complexity);
              if (std::find_if_not(
                      blockHash.begin(), endIt,
                      [](unsigned char byte) { return byte == 0; }) != endIt) {
                block.SetMiningPoint(block.GetConsensusInfo().miningPoint + 1);
              } else {
                break;
              }
            }
            if (now > miningEndTime) {
              return tl::unexpected(NestedError(
                  fmt::format("Cant serialize during time - {} nanoseconds",
                              miningInterval.count()),
                  loc));
            }
          }
          auto sign = Crypto::TryToSign(block.SerializeForHashing(), privateKey,
                                        needToValidateKey);
          if (!sign.has_value()) {
            return tl::unexpected(NestedError(
                "Cant mine block because cant sign", sign.error(), loc));
          }
          block.SetCurHash(std::move(sign.value()));
          return std::move(block);
        });
  } catch (std::exception &ex) {
    return tl::unexpected(NestedError(ex.what(), loc));
  } catch (...) {
    return tl::unexpected(NestedError("Handled unknown exception", loc));
  }
}
template <class SHA256Generator>
[[nodiscard]] tl::expected<Block, NestedError>
MineBlockImpl(ByteVector &&data, std::string &&privateKey,
              Block &&previousBlock, std::uint32_t complexity,
              std::chrono::nanoseconds miningInterval, bool needToValidateKey,
              SHA256Generator &&generator) noexcept {
  auto loc = std::source_location::current();
  try {
    // Так как используется SHA256, число 0 не может быть больше 32
    if (complexity > 32) {
      return tl::unexpected(NestedError(
          fmt::format("Max size of complexcity is 32. Cur complexcity - {}",
                      complexity),
          loc));
    }
    return Block::PrepareBlock(std::move(previousBlock))
        .map_error([loc](NestedError &&error) -> NestedError {
          return {"Cant mine block because cant prepare", std::move(error),
                  loc};
        })
        .and_then(
            [data = std::move(data), privateKey = std::move(privateKey),
             complexity, miningInterval, needToValidateKey, loc,
             generator = std::forward<SHA256Generator>(generator)](
                Block &&block) mutable -> tl::expected<Block, NestedError> {
              block.SetContainedData(std::move(data));
              block.SetMinedBy(Crypto::ConstructPublicKey(privateKey));
              if (complexity != 0) {
                const auto miningStartTime =
                    std::chrono::high_resolution_clock::now();
                const auto miningEndTime = miningStartTime + miningInterval;
                auto now = miningStartTime;
                for (; now < miningEndTime;
                     now = std::chrono::high_resolution_clock::now()) {
                  auto blockHash = generator(block.SerializeForHashing());
                  const auto endIt = std::next(blockHash.begin(), complexity);
                  if (std::find_if_not(blockHash.begin(), endIt,
                                       [](unsigned char byte) {
                                         return byte == 0;
                                       }) != endIt) {
                    block.SetMiningPoint(block.GetConsensusInfo().miningPoint +
                                         1);
                  } else {
                    break;
                  }
                }
                if (now > miningEndTime) {
                  return tl::unexpected(NestedError(
                      fmt::format("Cant serialize during time - {} nanoseconds",
                                  miningInterval.count()),
                      loc));
                }
              }
              auto sign = Crypto::TryToSign(block.SerializeForHashing(),
                                            privateKey, needToValidateKey);
              if (!sign.has_value()) {
                return tl::unexpected(NestedError(
                    "Cant mine block because cant sign", sign.error(), loc));
              }
              block.SetCurHash(std::move(sign.value()));
              return std::move(block);
            });
  } catch (std::exception &ex) {
    return tl::unexpected(NestedError(ex.what(), loc));
  } catch (...) {
    return tl::unexpected(NestedError("Handled unknown exception", loc));
  }
}
} // namespace BlockChainCore
