#include "BlockChain_C_API/BlockChain_api.h"
extern "C" {
//! Проверяем корректность подписи блока. Всегда возвращает пустой data. В
//! случае ошибки возвращает ошибку. Очищает переданные указатели
void *WASMValidateBlockSignWithDel(char *blockInProtoFormat, uint32_t blockSize,
                                   bool needToValidateKey) {
  return ValidateBlockSignWithDel(blockInProtoFormat, blockSize,
                                  needToValidateKey);
}
//! Проверяем корректность подписи блока. Всегда возвращает пустой data. В
//! случае ошибки возвращает ошибку. Не очищает переданные указатели
void *WASMValidateBlockSignNonDel(char *blockInProtoFormat, uint32_t blockSize,
                                  bool needToValidateKey) {
  return ValidateBlockSignNonDel(blockInProtoFormat, blockSize,
                                 needToValidateKey);
}
//! Подпись произвольных данных с очисткой переданных ресурсов
void *WASMSignDataWithDel(char *data, uint32_t dataSize, char *privateKey,
                          uint32_t keySize, bool needToValidateKey) {
  return SignDataWithDel(data, dataSize, privateKey, keySize,
                         needToValidateKey);
}
//! Подпись произвольных данных без очисткой переданных ресурсов
void *WASMSignDataNonDel(char *data, uint32_t dataSize, char *privateKey,
                         uint32_t keySize, bool needToValidateKey) {
  return SignDataNonDel(data, dataSize, privateKey, keySize, needToValidateKey);
}

//! Проверяем соответствие сложности блока переданному числу 0. Очищает
//! переданные указатели
bool WASMValidateBlockComplexityWithDel(char *blockInProtoFormat,
                                        uint32_t blockSize,
                                        uint32_t complexity) {
  return ValidateBlockComplexityWithDel(blockInProtoFormat, blockSize,
                                        complexity);
}
//! Проверяем соответствие сложности блока переданному числу 0. Не очищает
//! переданные указатели
bool WASMValidateBlockComplexityNonDel(char *blockInProtoFormat,
                                       uint32_t blockSize,
                                       uint32_t complexity) {
  return ValidateBlockComplexityNonDel(blockInProtoFormat, blockSize,
                                       complexity);
}
//! Возвращает приватный ключ, либо ошибку
void *WASMGeneratePrivateKey() { return GeneratePrivateKey(); }
//! Генерирует публичный ключ из приватного. Очищает переданные
//! указатели
void *WASMGetPublicKeyFromPrivateWithDel(char *privateKey, uint32_t keySize) {
  return GetPublicKeyFromPrivateWithDel(privateKey, keySize);
}
//! Генерирует публичный ключ из приватного. Не очищает
//! переданные указатели
void *WASMGetPublicKeyFromPrivateNonDel(char *privateKey, uint32_t keySize) {
  return GetPublicKeyFromPrivateNonDel(privateKey, keySize);
}
//! Создает начальный блок в формате proto.
void *WASMInitStartBlock() { return InitStartBlock(); }
//! Майнит блок, возвращает его в формате proto. Очищает переданные указатели
void *WASMMineBlockWithDel(char *data, uint32_t dataSize,
                           char *prevBlockInProtoFormat, uint32_t prevBlockSize,
                           char *privateKey, uint32_t keySize,
                           bool needToValidateKey) {
  return MineBlockWithDel(data, dataSize, prevBlockInProtoFormat, prevBlockSize,
                          privateKey, keySize, needToValidateKey);
}
//! Майнит блок, возвращает его в формате proto. Не очищает переданные указатели
void *WASMMineBlockNonDel(char *data, uint32_t dataSize,
                          char *prevBlockInProtoFormat, uint32_t prevBlockSize,
                          char *privateKey, uint32_t keySize,
                          bool needToValidateKey) {
  return MineBlockNonDel(data, dataSize, prevBlockInProtoFormat, prevBlockSize,
                         privateKey, keySize, needToValidateKey);
}
void WASMFreeResult(void *result) { FreeResult((Result *)result); }
void WASMFreePK(void *publicKey) { FreePK((PublicKeyResult *)publicKey); }
int WASMGetResultErrorSize(void *result) {
  return GetResultErrorSize((Result *)result);
}
char *WASMGetResultError(void *result) {
  return GetResultError((Result *)result);
}
int WASMGetResultDataSize(void *result) {
  return GetResultDataSize((Result *)result);
}
char *WASMGetResultData(void *result) {
  return GetResultData((Result *)result);
}
int WASMGetPKErrorSize(void *publicKey) {
  return GetPKErrorSize((PublicKeyResult *)publicKey);
}
char *WASMGetPKError(void *publicKey) {
  return GetPKError((PublicKeyResult *)publicKey);
}
int WASMGetFirstKeySize(void *publicKey) {
  return GetFirstKeySize((PublicKeyResult *)publicKey);
}
char *WASMGetFirstKey(void *publicKey) {
  return GetFirstKey((PublicKeyResult *)publicKey);
}
int WASMGetSecondKeySize(void *publicKey) {
  return GetSecondKeySize((PublicKeyResult *)publicKey);
}
char *WASMGetSecondKey(void *publicKey) {
  return GetSecondKey((PublicKeyResult *)publicKey);
}
void WASMClearBuf(char *buf) { ClearBuf(buf); }
}
int main() { return 0; }
