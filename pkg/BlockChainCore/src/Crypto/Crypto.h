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
  //! Проверяет подпись переданных данных.
  //! needToValidateKey определяет, доверяем ли мы переданному ключу и нужно ли
  //! его валидировать. Валидация ключа - достаточно дорогая операция
  [[nodiscard]] static tl::expected<std::true_type, NestedError>
  TryToVerifyECDSA_CryptoPP(
      const ByteVector &signature, const ByteVector &blockData,
      const std::pair<std::string, std::string> &publicKey,
      bool needToValidateKey) noexcept;
  //! Подписывает переданные данные с помощью переданного ключа.
  //! needToValidateKey определяет, доверяем ли мы переданному ключу и нужно ли
  //! его валидировать. Валидация ключа - достаточно дорогая операция
  [[nodiscard]] static tl::expected<ByteVector, NestedError>
  TryToSign(const ByteVector &data, const std::string &privateKey,
            bool needToValidateKey);
  //! Создает приватный и публичные ключи
  [[nodiscard]] static std::pair<std::string,
                                 std::pair<std::string, std::string>>
  GenerateKeys();
  //! Создает публичный ключ из приватного
  [[nodiscard]] static std::pair<std::string, std::string>
  ConstructPublicKey(const std::string &privateKey);
};

} // namespace BlockChainCore

#endif // BLOCKCHAINCORE_CRYPTO_H
