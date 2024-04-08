#include "BlockChain_api.h"
#include "Block/Mining.h"
#include "BlockChain_CXX_API/BlockChain_api.h"
#include <cstring>
#include <memory>
Result InitResult() {
  Result result;
  result.dataSize = 0;
  result.data = nullptr;
  result.error = nullptr;
  result.errorSize = 0;
  return result;
}
Result ResultFromErrorString(const std::string &error) {
  auto result = InitResult();
  result.errorSize = error.size();
  result.error = new char[result.errorSize];
  std::memcpy(result.error, error.data(), result.errorSize);
  return result;
}
Result ConvertNestedErrorToResult(const BlockChainCore::NestedError &error) {
  auto fullErrorMsg = error.GetFullErrorMsg();
  return ResultFromErrorString(fullErrorMsg);
}
Result ResultDataFromString(const std::string &data) {
  auto result = InitResult();
  result.dataSize = data.size();
  result.data = new char[result.dataSize];
  std::memcpy(result.data, data.data(), result.dataSize);
  return result;
}
PublicKeyResult InitPublicKeyResult() {
  PublicKeyResult result;
  result.publicKey.key1Size = 0;
  result.publicKey.key2Size = 0;
  result.publicKey.key1 = nullptr;
  result.publicKey.key2 = nullptr;
  result.error = nullptr;
  result.errorSize = 0;
  return result;
}
PublicKeyResult PublicKeyResultFromErrorString(const std::string &error) {
  auto result = InitPublicKeyResult();
  result.errorSize = error.size();
  result.error = new char[result.errorSize];
  std::memcpy(result.error, error.data(), result.errorSize);
  return result;
}
PublicKeyResult
ConvertNestedErrorToPublicKeyResult(const BlockChainCore::NestedError &error) {
  auto fullErrorMsg = error.GetFullErrorMsg();
  return PublicKeyResultFromErrorString(fullErrorMsg);
}
PublicKeyResult PublicKeyResultKeyFromStrings(const std::string &key1,
                                              const std::string &key2) {
  auto result = InitPublicKeyResult();
  result.publicKey.key1Size = key1.size();
  result.publicKey.key1 = new char[result.publicKey.key1Size];
  std::memcpy(result.publicKey.key1, key1.data(), result.publicKey.key1Size);
  result.publicKey.key2Size = key2.size();
  result.publicKey.key2 = new char[result.publicKey.key2Size];
  std::memcpy(result.publicKey.key2, key2.data(), result.publicKey.key2Size);
  return result;
}

//! Проверяем корректность подписи блока. Всегда возвращает пустой data. В
//! случае ошибки возвращает ошибку. Очищает переданные указатели
Result ValidateBlockSignWithDel(char *blockInProtoFormat, uint32_t blockSize,
                                bool needToValidateKey) {
  std::unique_ptr<char[]> deferData(blockInProtoFormat);
  return ValidateBlockSignNonDel(blockInProtoFormat, blockSize,
                                 needToValidateKey);
}
//! Проверяем корректность подписи блока. Всегда возвращает пустой data. В
//! случае ошибки возвращает ошибку. Не очищает переданные указатели
Result ValidateBlockSignNonDel(char *blockInProtoFormat, uint32_t blockSize,
                               bool needToValidateKey) {
  auto result = InitResult();
  try {
    auto block = BlockChainCore::CreateFromProto(
        std::string(blockInProtoFormat, blockInProtoFormat + blockSize));
    if (!block.has_value()) {
      return ConvertNestedErrorToResult(block.error());
    }
    auto isValid =
        BlockChainCore::ValidateBlock(block.value(), needToValidateKey);
    if (!isValid.has_value()) {
      return ConvertNestedErrorToResult(isValid.error());
    }
    return result;
  } catch (std::exception &ex) {
    return ResultFromErrorString(ex.what());
  } catch (...) {
    return ResultFromErrorString("Uknown error");
  }
}
//! Проверяем соответствие сложности блока переданному числу 0. Очищает
//! переданные указатели
bool ValidateBlockComplexityWithDel(char *blockInProtoFormat,
                                    uint32_t blockSize, bool needToValidateKey,
                                    uint32_t complexity) {
  std::unique_ptr<char[]> deferBlock(blockInProtoFormat);
  return ValidateBlockComplexityNonDel(blockInProtoFormat, blockSize,
                                       complexity);
}
//! Проверяем соответствие сложности блока переданному числу 0. Не очищает
//! переданные указатели
bool ValidateBlockComplexityNonDel(char *blockInProtoFormat, uint32_t blockSize,
                                   uint32_t complexity) {
  try {
    auto block = BlockChainCore::CreateFromProto(
        std::string(blockInProtoFormat, blockInProtoFormat + blockSize));
    if (!block.has_value()) {
      return false;
    }
    return BlockChainCore::IsBlockMinedCorrectly(block.value(), complexity);
  } catch (std::exception &ex) {
    return false;
  } catch (...) {
    return false;
  }
}
//! Возвращает приватный ключ, либо ошибку
Result GeneratePrivateKey() {
  try {
    auto keys = BlockChainCore::GenerateKeys();
    return ResultDataFromString(keys.first);
  } catch (std::exception &ex) {
    return ResultFromErrorString(ex.what());
  } catch (...) {
    return ResultFromErrorString("Uknown error");
  }
}
//! Генерирует публичный ключ в формате proto из приватного. Очищает переданные
//! указатели
PublicKeyResult GetPublikKeyFromPrivateWithDel(char *privateKey,
                                               uint32_t keySize) {
  std::unique_ptr<char[]> deferPrivateKey(privateKey);
  return GetPublikKeyFromPrivateNonDel(privateKey, keySize);
}
//! Генерирует публичный ключ в формате proto из приватного. Не очищает
//! переданные указатели
PublicKeyResult GetPublikKeyFromPrivateNonDel(char *privateKey,
                                              uint32_t keySize) {
  try {
    auto publickKey = BlockChainCore::ConstructPublicKey(
        std::string(privateKey, privateKey + keySize));
    return PublicKeyResultKeyFromStrings(publickKey.first, publickKey.second);
  } catch (std::exception &ex) {
    return PublicKeyResultFromErrorString(ex.what());
  } catch (...) {
    return PublicKeyResultFromErrorString("Uknown error");
  }
}
//! Создает начальный блок в формате proto.
Result InitStartBlock() {
  try {
    auto block = BlockChainCore::InitStartBlock();
    auto proto = BlockChainCore::ConvertToProto(block);
    if (!proto.has_value()) {
      return ConvertNestedErrorToResult(proto.error());
    }
    return ResultDataFromString(proto.value());
  } catch (std::exception &ex) {
    return ResultFromErrorString(ex.what());
  } catch (...) {
    return ResultFromErrorString("Uknown error");
  }
}
//! Майнит блок, возвращает его в формате proto. Очищает переданные указатели
Result MineBlockWithDel(char *data, uint32_t dataSize,
                        char *prevBlockInProtoFormat, uint32_t prevBlockSize,
                        char *privateKey, uint32_t keySize, uint32_t complexity,
                        uint32_t miningTime, bool needToValidateKey) {
  std::unique_ptr<char[]> deferData(data);
  std::unique_ptr<char[]> deferPrevBlock(prevBlockInProtoFormat);
  std::unique_ptr<char[]> deferPrivateKey(privateKey);
  return MineBlockNonDel(data, dataSize, prevBlockInProtoFormat, prevBlockSize,
                         privateKey, keySize, complexity, miningTime,
                         needToValidateKey);
}
//! Майнит блок, возвращает его в формате proto. Не очищает переданные указатели
Result MineBlockNonDel(char *data, uint32_t dataSize,
                       char *prevBlockInProtoFormat, uint32_t prevBlockSize,
                       char *privateKey, uint32_t keySize, uint32_t complexity,
                       uint32_t miningTime, bool needToValidateKey) {
  try {
    auto block = BlockChainCore::CreateFromProto(std::string(
        prevBlockInProtoFormat, prevBlockInProtoFormat + prevBlockSize));
    if (!block.has_value()) {
      return ResultFromErrorString(block.error().GetFullErrorMsg());
    }
    auto minedBlock = BlockChainCore::MineBlockWithStandartMiner(
        BlockChainCore::Crypto::ByteVector(data, data + dataSize),
        std::string(privateKey, privateKey + keySize), block.value(),
        complexity, std::chrono::nanoseconds(miningTime), needToValidateKey);
    if (!minedBlock.has_value()) {
      return ResultFromErrorString(minedBlock.error().GetFullErrorMsg());
    }
    auto serializedResult = BlockChainCore::ConvertToProto(minedBlock.value());
    if (!serializedResult.has_value()) {
      return ResultFromErrorString(minedBlock.error().GetFullErrorMsg());
    }
    return ResultDataFromString(serializedResult.value());
  } catch (std::exception &ex) {
    return ResultFromErrorString(ex.what());
  } catch (...) {
    return ResultFromErrorString("Uknown error");
  }
}
