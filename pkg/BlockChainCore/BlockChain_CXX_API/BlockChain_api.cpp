#include "BlockChain_api.h"
#include "Crypto/Crypto.h"
namespace BlockChainCore {
//! Проверяет корректность блока
auto ValidateBlock(const Block &block, bool needToValidate)
    -> decltype(block.CheckBlockIsCryptValid(needToValidate)) {
  return block.CheckBlockIsCryptValid(needToValidate);
}

//! создает приватный и публичные ключи
[[nodiscard]] std::pair<std::string, std::pair<std::string, std::string>>
GenerateKeys() {
  return Crypto::GenerateKeys();
}
//! Создает публичный ключ из приватного
[[nodiscard]] std::pair<std::string, std::string>
ConstructPublicKey(const std::string &privateKey) {
  return Crypto::ConstructPublicKey(privateKey);
}
//! Преобразует блок в его представление в protobuf
[[nodiscard]] tl::expected<std::string, NestedError>
ConvertToProto(Block &block) noexcept {
  return Block::ConvertToProto(block);
}
//! Преобразвует блок в его представление в protobuf. При этом, создание
//! потока может быть недешевым, поэтому для эффективного использования
//! данного метода следует эффективно работать с потоками ввода-вывода
[[nodiscard]] tl::expected<std::true_type, NestedError>
ConvertToProto(Block &block, std::ostream &outputStream) noexcept {
  return Block::ConvertToProto(block, outputStream);
}
//! Создает блок из его представления в protobuf
[[nodiscard]] tl::expected<Block, NestedError>
CreateFromProto(const std::string &data) noexcept {
  return Block::CreateFromProto(data);
}
//! Создает блок из его представления в protobuf. При этом, создание потока
//! может быть недешевым, поэтому для эффективного использования данного
//! метода следует эффективно работать с потоками ввода-вывода
[[nodiscard]] tl::expected<Block, NestedError>
CreateFromProto(std::istream &inputStream) noexcept {
  return Block::CreateFromProto(inputStream);
}
[[nodiscard]] Block InitStartBlock() { return BlockChainCore::Block::init(); }
} // namespace BlockChainCore
