//
// Created by houdini on 08.09.2023.
//
#ifndef BLOCKCHAINCORE_BLOCK_H
#define BLOCKCHAINCORE_BLOCK_H
#include "nested_error.h"
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
    const std::pair<std::string, std::string> &);
using CryptSigner = tl::expected<ByteVector, NestedError> (*)(
    const ByteVector &, const std::string &);

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
  std::uint64_t timestamp;
  std::pair<std::string, std::string>
      minedBy; // два числа в тексовом виде, разделенные точкой
  std::uint64_t ledgerId = 1; // для идентификации в БД
  BlockConsensusInfo consensusInfo;
  ByteVector containedData;
  Block() = default;

public:
  explicit Block(const BlockHashInfo &hashInfo, const std::uint64_t &timestamp,
                 const std::pair<std::string, std::string> &minedBy,
                 const std::uint64_t &ledgerId,
                 const BlockConsensusInfo &consensusInfo,
                 const ByteVector &containedData);
  explicit Block(BlockHashInfo &&hashInfo, const std::uint64_t &timestamp,
                 std::pair<std::string, std::string> &&minedBy,
                 const std::uint64_t &ledgerId,
                 const BlockConsensusInfo &consensusInfo,
                 ByteVector &&containedData);
  //! Инициализирует начальный блок.
  static Block init();
  //! Данный метод не создает блок полностью, т.к. не получает miningPoint и
  //! текущий хэш
  [[nodiscard]] static tl::expected<Block, NestedError>
  PrepareBlock(const Block &lastBlock);
  [[nodiscard]] const BlockHashInfo &GetHashInfo() const noexcept;
  //! Проверяет валидность блока (с точки зрения криптографии). Важно!
  //! Валидность хранимого в блоке публичного ключа не проверяется,
  //! предполагается, что мы доверяем хранимым в БД ключам
  [[nodiscard]] tl::expected<std::true_type, NestedError>
  CheckBlockIsCryptValid() noexcept;
  //! Проверяет валидность блока (с точки зрения криптографии) с помощью
  //! стороннего валидатора
  [[nodiscard]] tl::expected<std::true_type, NestedError>
  CheckBlockIsCryptValid(CryptValidator validator) noexcept;

  //! Осуществляет сериализацию блока для подсчета его хеша.
  [[nodiscard]] ByteVector SerializeForHashing() const;
  [[nodiscard]] const std::uint64_t &GetTransactionId() const noexcept;
  void SetCurHash(const ByteVector &curHash);
  void SetCurHash(ByteVector &&curHash);
  void SetHashInfo(const BlockHashInfo &hashInfo);
  void SetHashInfo(BlockHashInfo &&hashInfo) noexcept;
  void SetPrevHash(const ByteVector &prevHash);
  void SetPrevHash(ByteVector &&prevHash);
  void SetTransactionId(const std::uint64_t &ledgerId);
  [[nodiscard]] const std::uint64_t &GetTimestamp() const noexcept;
  void SetTimestamp(const std::uint64_t &timestamp);
  [[nodiscard]] const std::pair<std::string, std::string> &
  GetMinedBy() const noexcept;
  void SetMinedBy(const std::pair<std::string, std::string> &minedBy);
  void SetMinedBy(std::pair<std::string, std::string> &&minedBy);
  [[nodiscard]] const BlockConsensusInfo &GetConsensusInfo() const noexcept;
  void SetLuck(double luck);
  void SetMiningPoint(std::uint64_t miningPoint);
  [[nodiscard]] const ByteVector &GetContainedData() const noexcept;
  void SetContainedData(const ByteVector &contData);
  void SetContainedData(ByteVector &&contData);
  //! Размер данных только для хэширования. Этот метод нужен для того, чтобы
  //! избавиться от лишних выделений памяит при сериализации для хэширования.
  //! Таки данный метод использует захардкоженные размеры.
  std::uint64_t GetHashingBlockSize() const noexcept;
  //! Преобразует блок в его представление в protobuf
  static tl::expected<std::string, NestedError>
  ConvertToProto(Block &block) noexcept;
  //! Преобразвует блок в его представление в protobuf. При этом, создание
  //! потока может быть недешевым, поэтому для эффективного использования
  //! данного метода следует эффективно работать с потоками ввода-вывода
  [[nodiscard]] static tl::expected<std::true_type, NestedError>
  ConvertToProto(Block &block, std::ostream &outputStream) noexcept;
  //! Создает блок из его представления в protobuf
  static tl::expected<Block, NestedError>
  CreateFromProto(const std::string &data) noexcept;
  //! Создает блок из его представления в protobuf. При этом, создание потока
  //! может быть недешевым, поэтому для эффективного использования данного
  //! метода следует эффективно работать с потоками ввода-вывода
  static tl::expected<Block, NestedError>
  CreateFromProto(std::istream &inputStream) noexcept;
};
bool operator==(const Block &lhs, const Block &rhs);

} // namespace BlockChainCore

#endif // BLOCKCHAINCORE_BLOCK_H
