#include "BlockChain_api.h"
#include "Block/Mining.h"
#include "BlockChain_CXX_API/BlockChain_api.h"
#include <cstring>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <strstream>
class Result {
public:
  std::unique_ptr<std::string> data;
  std::unique_ptr<std::string> error;
  ~Result() = default;
  Result() = default;
  Result(Result &&another) = default;
};
using PKType = std::pair<std::string, std::string>;
class PublicKeyResult {
public:
  std::unique_ptr<PKType> key;
  std::unique_ptr<std::string> error;
  ~PublicKeyResult() = default;
  PublicKeyResult() = default;
  PublicKeyResult(PublicKeyResult &&other) = default;
};
#ifdef WASMBUILD

auto GetFromProto_C(char *data, uint32_t dataSize) {
  std::istrstream iss(data, dataSize);
  return BlockChainCore::CreateFromProto(iss);
}

tl::expected<std::string, BlockChainCore::NestedError>
ToProto_C(BlockChainCore::Block &block) {
  std::ostringstream oss;
  return BlockChainCore::ConvertToProto(block, oss)
      .and_then([&oss](std::true_type &&_)
                    -> tl::expected<std::string, BlockChainCore::NestedError> {
        return oss.str();
      })
      .or_else([](BlockChainCore::NestedError &&err)
                   -> tl::expected<std::string, BlockChainCore::NestedError> {
        return tl::unexpected(std::move(err));
      });
}
#else
auto GetFromProto_C(char *data, uint32_t dataSize) {
  return BlockChainCore::CreateFromProto(std::string(data, data + dataSize));
}
tl::expected<std::string, BlockChainCore::NestedError>
ToProto_C(BlockChainCore::Block &block) {
  return BlockChainCore::ConvertToProto(block);
}
#endif
Result NestedToResult(const BlockChainCore::NestedError &nested) {
  try {
    Result result;
    result.error = std::make_unique<std::string>(nested.GetFullErrorMsg());
    return result;
  } catch (...) {
    Result result;
    result.error = std::make_unique<std::string>("Nested convertion error");
    return result;
  }
}
Result ErrorStringToResult(const std::string &error) {
  try {
    Result result;
    result.error = std::make_unique<std::string>(error);
    return result;
  } catch (...) {
    Result result;
    result.error = std::make_unique<std::string>("String convertion error");
    return result;
  }
}
Result ErrorStringToResult(std::string &&error) {
  try {
    Result result;
    result.error = std::make_unique<std::string>(std::move(error));
    return result;
  } catch (...) {
    Result result;
    result.error = std::make_unique<std::string>("String convertion error");
    return result;
  }
}
Result DataToResult(const std::string &data) {
  try {
    Result result;
    result.data = std::make_unique<std::string>(data);
    return result;
  } catch (...) {
    Result result;
    result.error = std::make_unique<std::string>("String convertion error");
    return result;
  }
}
Result DataToResult(std::string &&data) {
  try {
    Result result;
    result.data = std::make_unique<std::string>(std::move(data));
    return result;
  } catch (...) {
    Result result;
    result.error = std::make_unique<std::string>("String convertion error");
    return result;
  }
}
Result DataToResult(const BlockChainCore::ByteVector &data) {
  try {
    Result result;
    result.data =
        std::make_unique<std::string>(std::string(data.begin(), data.end()));
    return result;
  } catch (...) {
    Result result;
    result.error = std::make_unique<std::string>("String convertion error");
    return result;
  }
}
Result DataToResult(BlockChainCore::ByteVector &&data) {
  try {
    Result result;
    result.data =
        std::make_unique<std::string>(std::string(data.begin(), data.end()));
    return result;
  } catch (...) {
    Result result;
    result.error = std::make_unique<std::string>("String convertion error");
    return result;
  }
}
PublicKeyResult NestedToPKResult(const BlockChainCore::NestedError &nested) {
  try {
    PublicKeyResult result;
    result.error = std::make_unique<std::string>(nested.GetFullErrorMsg());
    return result;
  } catch (...) {
    PublicKeyResult result;
    result.error = std::make_unique<std::string>("Nested convertion error");
    return result;
  }
}
PublicKeyResult ErrorStringToPKResult(const std::string &error) {
  try {
    PublicKeyResult result;
    result.error = std::make_unique<std::string>(error);
    return result;
  } catch (...) {
    PublicKeyResult result;
    result.error = std::make_unique<std::string>("String convertion error");
    return result;
  }
}
PublicKeyResult ErrorStringToPKResult(std::string &&error) {
  try {
    PublicKeyResult result;
    result.error = std::make_unique<std::string>(std::move(error));
    return result;
  } catch (...) {
    PublicKeyResult result;
    result.error = std::make_unique<std::string>("String convertion error");
    return result;
  }
}
PublicKeyResult
DataToPKResult(const std::pair<std::string, std::string> &data) {
  try {
    PublicKeyResult result;
    result.key = std::make_unique<PKType>(data);
    return result;
  } catch (...) {
    PublicKeyResult result;
    result.error = std::make_unique<std::string>("String convertion error");
    return result;
  }
}
PublicKeyResult DataToPKResult(std::pair<std::string, std::string> &&data) {
  try {
    PublicKeyResult result;
    result.key = std::make_unique<PKType>(std::move(data));
    return result;
  } catch (...) {
    PublicKeyResult result;
    result.error = std::make_unique<std::string>("String convertion error");
    return result;
  }
}
//! Проверяем корректность подписи блока. Всегда возвращает пустой data. В
//! случае ошибки возвращает ошибку. Очищает переданные указатели
Result *ValidateBlockSignWithDel(char *blockInProtoFormat, uint32_t blockSize,
                                 bool needToValidateKey) {
  std::unique_ptr<char[]> deferData(blockInProtoFormat);
  return ValidateBlockSignNonDel(blockInProtoFormat, blockSize,
                                 needToValidateKey);
}
//! Проверяем корректность подписи блока. Всегда возвращает пустой data. В
//! случае ошибки возвращает ошибку. Не очищает переданные указатели
Result *ValidateBlockSignNonDel(char *blockInProtoFormat, uint32_t blockSize,
                                bool needToValidateKey) {
  try {
    if (blockInProtoFormat == nullptr || blockSize < 2) {
      throw std::invalid_argument("Uncorrect args passed");
    }
    auto block = GetFromProto_C(blockInProtoFormat, blockSize);
    if (!block.has_value()) {
      return new Result(NestedToResult(block.error()));
    }
    auto isValid =
        BlockChainCore::ValidateBlock(block.value(), needToValidateKey);
    if (!isValid.has_value()) {
      return new Result(NestedToResult(isValid.error()));
    }
    return new Result();
  } catch (std::exception &ex) {
    return new Result(ErrorStringToResult(ex.what()));
  } catch (...) {
    return new Result(ErrorStringToResult("Uknown error"));
  }
}
//! Проверяем соответствие сложности блока переданному числу 0. Очищает
//! переданные указатели
bool ValidateBlockComplexityWithDel(char *blockInProtoFormat,
                                    uint32_t blockSize, uint32_t complexity) {
  std::unique_ptr<char[]> deferBlock(blockInProtoFormat);
  return ValidateBlockComplexityNonDel(blockInProtoFormat, blockSize,
                                       complexity);
}
//! Проверяем соответствие сложности блока переданному числу 0. Не очищает
//! переданные указатели
bool ValidateBlockComplexityNonDel(char *blockInProtoFormat, uint32_t blockSize,
                                   uint32_t complexity) {
  try {
    if (blockInProtoFormat == nullptr || blockSize < 2) {
      throw std::invalid_argument("Uncorrect args passed");
    }
    auto block = GetFromProto_C(blockInProtoFormat, blockSize);
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
Result *GeneratePrivateKey() {
  try {
    auto keys = BlockChainCore::GenerateKeys();
    return new Result(DataToResult(std::move(keys.first)));
  } catch (std::exception &ex) {
    return new Result(ErrorStringToResult(ex.what()));
  } catch (...) {
    return new Result(ErrorStringToResult("Uknown error"));
  }
}
//! Генерирует публичный ключ в формате proto из приватного. Очищает переданные
//! указатели
PublicKeyResult *GetPublicKeyFromPrivateWithDel(char *privateKey,
                                                uint32_t keySize) {
  std::unique_ptr<char[]> deferPrivateKey(privateKey);
  return GetPublicKeyFromPrivateNonDel(privateKey, keySize);
}
//! Генерирует публичный ключ в формате proto из приватного. Не очищает
//! переданные указатели
PublicKeyResult *GetPublicKeyFromPrivateNonDel(char *privateKey,
                                               uint32_t keySize) {
  try {
    if (privateKey == nullptr || keySize < 2) {
      throw std::invalid_argument("Invalid args passed");
    }
    auto publickKey = BlockChainCore::ConstructPublicKey(
        std::string(privateKey, privateKey + keySize));
    return new PublicKeyResult(DataToPKResult(std::move(publickKey)));
  } catch (std::exception &ex) {
    return new PublicKeyResult(ErrorStringToPKResult(ex.what()));
  } catch (...) {
    return new PublicKeyResult(ErrorStringToPKResult("Uknown error"));
  }
}
//! Создает начальный блок в формате proto.
Result *InitStartBlock() {
  try {
    auto block = BlockChainCore::InitStartBlock();
    auto proto = ToProto_C(block);
    if (!proto.has_value()) {
      return new Result(NestedToResult(proto.error()));
    }
    return new Result(DataToResult(proto.value()));
  } catch (std::exception &ex) {
    return new Result(ErrorStringToResult(ex.what()));
  } catch (...) {
    return new Result(ErrorStringToResult("Uknown error"));
  }
}
//! Майнит блок, возвращает его в формате proto. Очищает переданные указатели
Result *MineBlockWithDel(char *data, uint32_t dataSize,
                         char *prevBlockInProtoFormat, uint32_t prevBlockSize,
                         char *privateKey, uint32_t keySize,
                         bool needToValidateKey) {
  std::unique_ptr<char[]> deferData(data);
  std::unique_ptr<char[]> deferPrevBlock(prevBlockInProtoFormat);
  std::unique_ptr<char[]> deferPrivateKey(privateKey);
  return MineBlockNonDel(data, dataSize, prevBlockInProtoFormat, prevBlockSize,
                         privateKey, keySize, needToValidateKey);
}
//! Майнит блок, возвращает его в формате proto. Не очищает переданные указатели
Result *MineBlockNonDel(char *data, uint32_t dataSize,
                        char *prevBlockInProtoFormat, uint32_t prevBlockSize,
                        char *privateKey, uint32_t keySize,
                        bool needToValidateKey) {
  try {
    if (data == nullptr || dataSize < 2 || prevBlockInProtoFormat == nullptr ||
        prevBlockSize < 2 || privateKey == nullptr || keySize < 2) {
      throw std::invalid_argument("Uncorrect args passed");
    }
    auto block = GetFromProto_C(prevBlockInProtoFormat, prevBlockSize);
    if (!block.has_value()) {
      return new Result(NestedToResult(block.error()));
    }
    auto minedBlock = BlockChainCore::MineBlockWithStandartMiner(
        BlockChainCore::Crypto::ByteVector(data, data + dataSize),
        std::string(privateKey, privateKey + keySize), block.value(),
        needToValidateKey);
    if (!minedBlock.has_value()) {
      return new Result(NestedToResult(minedBlock.error()));
    }
    auto serializedResult = ToProto_C(minedBlock.value());
    if (!serializedResult.has_value()) {
      return new Result(NestedToResult(minedBlock.error()));
    }
    return new Result(DataToResult(serializedResult.value()));
  } catch (std::exception &ex) {
    return new Result(ErrorStringToResult(ex.what()));
  } catch (...) {
    return new Result(ErrorStringToResult("Uknown error"));
  }
}
void FreeResult(Result *result) {
  if (result != nullptr) {
    delete result;
  }
}
void FreePK(PublicKeyResult *publicKey) {
  if (publicKey != nullptr) {
    delete publicKey;
  }
}
int GetResultErrorSize(Result *result) {
  try {
    int res = 0;
    if (result != nullptr && result->error != nullptr) {
      res = result->error->size();
    }
    return res;
  } catch (...) {
    return 0;
  }
}
char *GetResultError(Result *result) {
  try {
    char *res = nullptr;
    if (result != nullptr && result->error != nullptr &&
        result->error->size() >= 2) {
      res = new char[result->error->size()];
      std::memcpy(res, result->error->data(), result->error->size());
    }
    return res;
  } catch (...) {
    return nullptr;
  }
}
int GetResultDataSize(Result *result) {
  try {
    int res = 0;
    if (result != nullptr && result->data != nullptr) {
      res = result->data->size();
    }
    return res;
  } catch (...) {
    return 0;
  }
}
char *GetResultData(Result *result) {
  try {
    char *res = nullptr;
    if (result != nullptr && result->data != nullptr &&
        result->data->size() >= 2) {
      res = new char[result->data->size()];
      std::memcpy(res, result->data->data(), result->data->size());
    }
    return res;
  } catch (...) {
    return nullptr;
  }
}
int GetPKErrorSize(PublicKeyResult *publicKey) {
  try {
    int res = 0;
    if (publicKey != nullptr && publicKey->error != nullptr) {
      res = publicKey->error->size();
    }
    return res;
  } catch (...) {
    return 0;
  }
}
char *GetPKError(PublicKeyResult *publicKey) {
  try {
    char *res = nullptr;
    if (publicKey != nullptr && publicKey->error != nullptr &&
        publicKey->error->size() >= 2) {
      res = new char[publicKey->error->size()];
      std::memcpy(res, publicKey->error->data(), publicKey->error->size());
    }
    return res;
  } catch (...) {
    return nullptr;
  }
}
int GetFirstKeySize(PublicKeyResult *publicKey) {
  try {
    int res = 0;
    if (publicKey != nullptr && publicKey->key != nullptr) {
      res = publicKey->key->first.size();
    }
    return res;
  } catch (...) {
    return 0;
  }
}
char *GetFirstKey(PublicKeyResult *publicKey) {
  try {
    char *res = nullptr;
    if (publicKey != nullptr && publicKey->key != nullptr &&
        publicKey->key->first.size() >= 2) {
      res = new char[publicKey->key->first.size()];
      std::memcpy(res, publicKey->key->first.data(),
                  publicKey->key->first.size());
    }
    return res;
  } catch (...) {
    return nullptr;
  }
}
int GetSecondKeySize(PublicKeyResult *publicKey) {
  try {
    int res = 0;
    if (publicKey != nullptr && publicKey->key != nullptr) {
      res = publicKey->key->second.size();
    }
    return res;
  } catch (...) {
    return 0;
  }
}
char *GetSecondKey(PublicKeyResult *publicKey) {
  try {
    char *res = nullptr;
    if (publicKey != nullptr && publicKey->key != nullptr &&
        publicKey->key->second.size() >= 2) {
      res = new char[publicKey->key->second.size()];
      std::memcpy(res, publicKey->key->second.data(),
                  publicKey->key->second.size());
    }
    return res;
  } catch (...) {
    return nullptr;
  }
}
void ClearBuf(char *buf) { delete[] buf; }
//! Подпись произвольных данных с очисткой переданных ресурсов
Result *SignDataWithDel(char *data, uint32_t dataSize, char *privateKey,
                        uint32_t keySize, bool needToValidateKey) {
  std::unique_ptr<char[]> dataDefer(data);
  std::unique_ptr<char[]> privateKeyDefer(privateKey);
  return SignDataNonDel(data, dataSize, privateKey, keySize, needToValidateKey);
}
//! Подпись произвольных данных без очисткой переданных ресурсов
Result *SignDataNonDel(char *data, uint32_t dataSize, char *privateKey,
                       uint32_t keySize, bool needToValidateKey) {
  try {
    auto sign = BlockChainCore::Crypto::TryToSign(
        BlockChainCore::ByteVector(data, data + dataSize),
        std::string(privateKey, privateKey + keySize), needToValidateKey);
    if (!sign.has_value()) {
      return new Result(NestedToResult(sign.error()));
    }
    return new Result(DataToResult(std::move(sign.value())));
  } catch (std::exception &ex) {
    return new Result(ErrorStringToResult(ex.what()));
  } catch (...) {
    return new Result(ErrorStringToResult("Uknown error"));
  }
}
// //! Проверка подписи произвольных данных без очистки переданных ресурсов
// bool ValidateDataNonDel(char *data, uint32_t dataSize, char *pubKeyFirst,
//                         uint32_t pubKeyFirstSize, char *pubKeySecond,
//                         uint32_t pubKeySecondSize, bool needToValidateKey) {
//   try {
//     auto validateRes =
//     BlockChainCore::Crypto::TryToVerifyECDSA_CryptoPP(const ByteVector
//     &signature, const ByteVector &blockData, const std::pair<std::string,
//     std::string> &publicKey, bool needToValidateKey)
//   } catch (std::exception &ex) {
//     return new Result(ErrorStringToResult(ex.what()));
//   } catch (...) {
//     return new Result(ErrorStringToResult("Uknown error"));
//   }
// }
// //! Проверка подписи произвольных данных с очисткой переданных ресурсов
// bool ValidateDataWithDel(char *data, uint32_t dataSize, char *pubKeyFirst,
//                          uint32_t pubKeyFirstSize, char *pubKeySecond,
//                          uint32_t pubKeySecondSize, bool needToValidateKey) {
//   std::unique_ptr<char[]> deferData(data);
//   std::unique_ptr<char[]> deferPubKey1(pubKeyFirst);
//   std::unique_ptr<char[]> deferPubKey2(pubKeySecond);
//   return ValidateDataNonDel(data, dataSize, pubKeyFirst, pubKeyFirstSize,
//                             pubKeySecond, pubKeySecondSize,
//                             needToValidateKey)
// }
