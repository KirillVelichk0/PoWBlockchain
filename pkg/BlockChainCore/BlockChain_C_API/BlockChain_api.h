#pragma once
#include <stdint.h>
extern "C" {
struct Result {
  char *data;
  uint32_t dataSize;
  char *error;
  uint32_t errorSize;
};
struct PublicKey {
  char *key1;
  uint32_t key1Size;
  char *key2;
  uint32_t key2Size;
};
struct PublicKeyResult {
  PublicKey publicKey;
  char *error;
  uint32_t errorSize;
};
//! Проверяем корректность подписи блока. Всегда возвращает пустой data. В
//! случае ошибки возвращает ошибку. Очищает переданные указатели
Result ValidateBlockSignWithDel(char *blockInProtoFormat, uint32_t blockSize,
                                bool needToValidateKey);
//! Проверяем корректность подписи блока. Всегда возвращает пустой data. В
//! случае ошибки возвращает ошибку. Не очищает переданные указатели
Result ValidateBlockSignNonDel(char *blockInProtoFormat, uint32_t blockSize,
                               bool needToValidateKey);
//! Проверяем соответствие сложности блока переданному числу 0. Очищает
//! переданные указатели
bool ValidateBlockComplexityWithDel(char *blockInProtoFormat,
                                    uint32_t blockSize, uint32_t complexity);
//! Проверяем соответствие сложности блока переданному числу 0. Не очищает
//! переданные указатели
bool ValidateBlockComplexityNonDel(char *blockInProtoFormat, uint32_t blockSize,
                                   uint32_t complexity);
//! Возвращает приватный ключ, либо ошибку
Result GeneratePrivateKey();
//! Генерирует публичный ключ из приватного. Очищает переданные
//! указатели
PublicKeyResult GetPublikKeyFromPrivateWithDel(char *privateKey,
                                               uint32_t keySize);
//! Генерирует публичный ключ из приватного. Не очищает
//! переданные указатели
PublicKeyResult GetPublikKeyFromPrivateNonDel(char *privateKey,
                                              uint32_t keySize);
//! Создает начальный блок в формате proto.
Result InitStartBlock();
//! Майнит блок, возвращает его в формате proto. Очищает переданные указатели
Result MineBlockWithDel(char *data, uint32_t dataSize,
                        char *prevBlockInProtoFormat, uint32_t prevBlockSize,
                        char *privateKey, uint32_t keySize, uint32_t complexity,
                        uint32_t miningTime, bool needToValidateKey);
//! Майнит блок, возвращает его в формате proto. Не очищает переданные указатели
Result MineBlockNonDel(char *data, uint32_t dataSize,
                       char *prevBlockInProtoFormat, uint32_t prevBlockSize,
                       char *privateKey, uint32_t keySize, uint32_t complexity,
                       uint32_t miningTime, bool needToValidateKey);
}
