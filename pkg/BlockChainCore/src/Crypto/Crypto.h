//
// Created by houdini on 08.09.2023.
//

#ifndef BLOCKCHAINCORE_CRYPTO_H
#define BLOCKCHAINCORE_CRYPTO_H
#include "../nested_error/nested_error.h"
#include <cstdint>
#include <memory>
#include <tl/expected.hpp>
#include <type_traits>
#include <vector>
namespace BlockChainCore {
class Crypto;
// Более эффективная имплементация публичного ключа, избегающая лишних
// преобразований
class PublicKeyOptimizedImpl {
private:
  friend class Crypto;
  std::shared_ptr<void> impl;
  PublicKeyOptimizedImpl(const std::shared_ptr<void> &impl);
  PublicKeyOptimizedImpl(std::shared_ptr<void> &&impl);

public:
  //! Используется валидация ключа
  static PublicKeyOptimizedImpl FromClassicFormat(
      const std::pair<std::string, std::string> &publicKey) noexcept(false);
  [[nodiscard]] std::pair<std::string, std::string> ToClassicFormat() const
      noexcept(false);
  PublicKeyOptimizedImpl() = delete;
  ~PublicKeyOptimizedImpl() = default;
  PublicKeyOptimizedImpl(const PublicKeyOptimizedImpl &impl) noexcept;
  PublicKeyOptimizedImpl &
  operator=(const PublicKeyOptimizedImpl &impl) noexcept;
  PublicKeyOptimizedImpl(PublicKeyOptimizedImpl &&impl) noexcept;
  PublicKeyOptimizedImpl &operator=(PublicKeyOptimizedImpl &&impl) noexcept;
  [[nodiscard]] std::shared_ptr<void> GetImpl() const noexcept;
};
class PrivateKeyOptimizedImpl {
private:
  friend class Crypto;
  std::shared_ptr<void> impl;
  PrivateKeyOptimizedImpl(const std::shared_ptr<void> &impl);
  PrivateKeyOptimizedImpl(std::shared_ptr<void> &&impl);

public:
  //! Используется валидация ключа
  static PrivateKeyOptimizedImpl
  FromClassicFormat(const std::string &key) noexcept(false);
  [[nodiscard]] std::string ToClassicFormat() const noexcept(false);
  PrivateKeyOptimizedImpl() = delete;
  ~PrivateKeyOptimizedImpl() = default;
  PrivateKeyOptimizedImpl(const PrivateKeyOptimizedImpl &key) noexcept;
  PrivateKeyOptimizedImpl &
  operator=(const PrivateKeyOptimizedImpl &key) noexcept;
  PrivateKeyOptimizedImpl(PrivateKeyOptimizedImpl &&key) noexcept;
  PrivateKeyOptimizedImpl &operator=(PrivateKeyOptimizedImpl &&key) noexcept;
  [[nodiscard]] std::shared_ptr<void> GetImpl() const noexcept;
};
class Crypto {
private:
  Crypto() = default;

public:
  using ByteVector = std::vector<unsigned char>;
  //! Проверяет подпись переданных данных, используя внутреннее представление
  //! ключа
  [[nodiscard]] static tl::expected<std::true_type, NestedError>
  TryToVerifyECDSA_CryptoPPOptimized(
      const ByteVector &signature, const ByteVector &blockData,
      const PublicKeyOptimizedImpl &publicKey) noexcept;
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
  //! Подписывает переданные данные с помощью переданного ключа во внунтреннем
  //! представлении
  [[nodiscard]] static tl::expected<ByteVector, NestedError>
  TryToSignOptimized(const ByteVector &data,
                     const PrivateKeyOptimizedImpl &privateKey);
  //! создает приватный и публичные ключи
  [[nodiscard]] static std::pair<std::string,
                                 std::pair<std::string, std::string>>
  GenerateKeys();

  //! Создает публичный ключ из приватного
  [[nodiscard]] static std::pair<std::string, std::string>
  ConstructPublicKey(const std::string &privateKey);
};

} // namespace BlockChainCore

#endif // BLOCKCHAINCORE_CRYPTO_H
