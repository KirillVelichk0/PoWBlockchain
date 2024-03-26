//
// Created by houdini on 08.09.2023.
//
#ifndef BLOCKCHAINCORE_BLOCK_H
#define BLOCKCHAINCORE_BLOCK_H
#include "../nested_error/nested_error.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <cstdint>
#include <string>
#include <tl/expected.hpp>
#include <type_traits>
#include <vector>
namespace BlockChainCore {
#ifdef BLOCKCHAICNCORE_BLOCK_SIZE_CUSTOM
constexpr std::uint32_t BlockSize = BLOCKCHAICNCORE_BLOCK_SIZE_CUSTOM;
#else
constexpr std::uint32_t BlockSize = 2000000;
#endif

class Block;
using ByteVector = std::vector<unsigned char>;

using CryptValidator = tl::expected<std::true_type, NestedError> (*)(
    const ByteVector &, const ByteVector &,
    const std::pair<std::uint64_t, std::uint64_t> &);
using CryptSigner = tl::expected<ByteVector, NestedError> (*)(
    const ByteVector &, const std::pair<std::uint64_t, std::uint64_t> &);
using UnixTime = boost::posix_time::ptime;

//! \brief Информация о хэше блока
//! \details Информация о хэше блока. Хранит подписанный хэш предыдущей и
//! текущей записи. boost.serialize используется только для валидации блока
struct BlockHashInfo {
  ByteVector prevSignedHash;
  ByteVector curSignedHash;
};

//! Информация, необходимая алгоритму консенсуса
struct BlockConsensusInfo {
  std::uint64_t miningPoint = 0; // для алгоритма консенсуса
  double luck = 0.0; // для алгоритма консенсуса
};
//! \brief Класс, хранящий информацию о блоке
//! \details Класс, хранящий информацию о блоке. По сути, его основная задача -
//! обеспечение сериализации данных.
class Block {
private:
  BlockHashInfo hashInfo;
  UnixTime timestamp;
  std::pair<std::int64_t, std::int64_t>
      minedBy; // два числа в тексовом виде, разделенные точкой
  std::uint64_t ledgerId = 1; // для идентификации в БД
  BlockConsensusInfo consensusInfo;
  ByteVector containedData;
  Block() = default;

public:
  explicit Block(const BlockHashInfo &hashInfo, const UnixTime &timestamp,
                 const std::pair<std::int64_t, std::int64_t> &minedBy,
                 const std::uint64_t &ledgerId,
                 const BlockConsensusInfo &consensusInfo,
                 const ByteVector &containedData);
  explicit Block(BlockHashInfo &&hashInfo, const UnixTime &timestamp,
                 std::pair<std::int64_t, std::int64_t> &&minedBy,
                 const std::uint64_t &ledgerId,
                 const BlockConsensusInfo &consensusInfo,
                 ByteVector &&containedData);
  //! Инициализирует начальный блок.
  static Block init();
  //! Данный метод не создает блок полностью, т.к. не получает miningPoint и
  //! текущий хэш
  [[nodiscard]] static tl::expected<Block, NestedError>
  PrepareBlock(const Block &lastBlock);
  [[nodiscard]] auto GetHashInfo() const noexcept;
  //! Проверяет валидность блока (с точки зрения криптографии)
  [[nodiscard]] tl::expected<std::true_type, NestedError>
  CheckBlockIsCryptValid() noexcept;
  //! Проверяет валидность блока (с точки зрения криптографии) с помощью
  //! стороннего валидатора
  [[nodiscard]] tl::expected<std::true_type, NestedError>
  CheckBlockIsCryptValid(CryptValidator validator) noexcept;

  //! Осуществляет сериализацию блока для подсчета его хеша.
  [[nodiscard]] ByteVector SerializeForHashing() const;
  [[nodiscard]] [[nodiscard]] auto GetTransactionId() const noexcept;
  void SetCurHash(const ByteVector &curHash);
  void SetCurHash(ByteVector &&curHash);
  void SetPrevHash(const ByteVector &prevHash);
  void SetPrevHash(ByteVector &&prevHash);
  void SetTransactionId(const std::uint64_t &ledgerId);
  void SetTransactionId(std::uint64_t &&ledgerId);
  [[nodiscard]] auto GetTimestamp() const noexcept;
  void SetTimestamp(const UnixTime &timestamp);
  [[nodiscard]] auto GetMinedBy() const noexcept;
  void SetMinedBy(const std::pair<std::int64_t, std::int64_t> &minedBy);
  void SetMinedBy(std::pair<std::int64_t, std::int64_t> &&minedBy);
  [[nodiscard]] auto GetConsensusInfo() const noexcept;
  void SetLuck(double luck);
  void SetMiningPoint(std::uint64_t miningPoint);
  [[nodiscard]] auto GetContainedData() const noexcept;
  void SetContainedData(const ByteVector &contData);
  void SetContainedData(ByteVector &&contData);
  //! Размер данных только для хэширования
  std::uint64_t GetHashingBlockSize() const noexcept;
  //! Преобразует блок в его представление в protobuf
  static tl::expected<std::string, NestedError>
  ConvertToProto(Block &block) noexcept;
  //! Создает блок из его представления в protobuf
  static tl::expected<Block, NestedError>
  CreateFromProto(const std::string &data) noexcept;
};

} // namespace BlockChainCore

#endif // BLOCKCHAINCORE_BLOCK_H
