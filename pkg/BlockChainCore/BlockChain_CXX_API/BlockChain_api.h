#pragma once
// не удалять include с Block/Mining, он нужен

#include "Block/Block.h"
#include "Block/Mining.h"
namespace BlockChainCore {
//
// Добавляется все содержимое из Mining.h
//

// Здесь добавляем новые функции

//! Проверяет корректность блока
auto ValidateBlock(const Block &block, bool needToValidate)
    -> decltype(block.CheckBlockIsCryptValid(needToValidate));

//! создает приватный и публичные ключи
[[nodiscard]] std::pair<std::string, std::pair<std::string, std::string>>
GenerateKeys();
//! Создает публичный ключ из приватного
[[nodiscard]] std::pair<std::string, std::string>
ConstructPublicKey(const std::string &privateKey);
//! Преобразует блок в его представление в protobuf
[[nodiscard]] tl::expected<std::string, NestedError>
ConvertToProto(Block &block) noexcept;
//! Преобразвует блок в его представление в protobuf. При этом, создание
//! потока может быть недешевым, поэтому для эффективного использования
//! данного метода следует эффективно работать с потоками ввода-вывода
[[nodiscard]] tl::expected<std::true_type, NestedError>
ConvertToProto(Block &block, std::ostream &outputStream) noexcept;
//! Создает блок из его представления в protobuf
[[nodiscard]] tl::expected<Block, NestedError>
CreateFromProto(const std::string &data) noexcept;
//! Создает блок из его представления в protobuf. При этом, создание потока
//! может быть недешевым, поэтому для эффективного использования данного
//! метода следует эффективно работать с потоками ввода-вывода
[[nodiscard]] tl::expected<Block, NestedError>
CreateFromProto(std::istream &inputStream) noexcept;
//! Создает первый блок в цепочке. Блок не несет никакой информации, его не
//! нужно никак валидировать. Он просто есть
[[nodiscard]] Block InitStartBlock();
} // namespace BlockChainCore
