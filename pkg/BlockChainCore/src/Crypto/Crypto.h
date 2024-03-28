//
// Created by houdini on 08.09.2023.
//

#ifndef BLOCKCHAINCORE_CRYPTO_H
#define BLOCKCHAINCORE_CRYPTO_H
#include "../nested_error/nested_error.h"
#include <cstdint>
#include <tl/expected.hpp>
#include <type_traits>
#include <vector>
namespace BlockChainCore {
class Crypto {
private:
  Crypto() = default;

public:
  using ByteVector = std::vector<unsigned char>;
  [[nodiscard]] static tl::expected<std::true_type, NestedError>
  TryToVerifyECDSA_CryptoPP(
      const ByteVector &signature, const ByteVector &blockData,
      const std::pair<std::string, std::string> &publicKey) noexcept;
  [[nodiscard]] static tl::expected<ByteVector, NestedError>
  TryToSign(const ByteVector &data, const std::string &privateKey);
};

} // namespace BlockChainCore

#endif // BLOCKCHAINCORE_CRYPTO_H
