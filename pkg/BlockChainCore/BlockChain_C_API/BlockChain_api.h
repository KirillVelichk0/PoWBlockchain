#pragma once
#include <stdint.h>
extern "C" {

class Result;
class PublicKeyResult;
//! Проверяем корректность подписи блока. Всегда возвращает пустой data. В
//! случае ошибки возвращает ошибку. Очищает переданные указатели
Result *ValidateBlockSignWithDel(char *blockInProtoFormat, uint32_t blockSize,
                                 bool needToValidateKey);
//! Проверяем корректность подписи блока. Всегда возвращает пустой data. В
//! случае ошибки возвращает ошибку. Не очищает переданные указатели
Result *ValidateBlockSignNonDel(char *blockInProtoFormat, uint32_t blockSize,
                                bool needToValidateKey);
//! Подпись произвольных данных с очисткой переданных ресурсов
Result *SignDataWithDel(char *data, uint32_t dataSize, char *privateKey,
                        uint32_t keySize, bool needToValidateKey);
//! Подпись произвольных данных без очисткой переданных ресурсов
Result *SignDataNonDel(char *data, uint32_t dataSize, char *privateKey,
                       uint32_t keySize, bool needToValidateKey);
// //! Проверка подписи произвольных данных без очистки переданных ресурсов
// bool ValidateDataNonDel(char *data, uint32_t dataSize, char *pubKeyFirst,
//                         uint32_t pubKeyFirstSize, char *pubKeySecond,
//                         uint32_t pubKeySecondSize, bool needToValidateKey);
// //! Проверка подписи произвольных данных с очисткой переданных ресурсов
// bool ValidateDataWithDel(char *data, uint32_t dataSize, char *pubKeyFirst,
//                          uint32_t pubKeyFirstSize, char *pubKeySecond,
//                          uint32_t pubKeySecondSize, bool needToValidateKey);
//! Проверяем соответствие сложности блока переданному числу 0. Очищает
//! переданные указатели
bool ValidateBlockComplexityWithDel(char *blockInProtoFormat,
                                    uint32_t blockSize, uint32_t complexity);
//! Проверяем соответствие сложности блока переданному числу 0. Не очищает
//! переданные указатели
bool ValidateBlockComplexityNonDel(char *blockInProtoFormat, uint32_t blockSize,
                                   uint32_t complexity);
//! Возвращает приватный ключ, либо ошибку
Result *GeneratePrivateKey();
//! Генерирует публичный ключ из приватного. Очищает переданные
//! указатели
PublicKeyResult *GetPublicKeyFromPrivateWithDel(char *privateKey,
                                                uint32_t keySize);
//! Генерирует публичный ключ из приватного. Не очищает
//! переданные указатели
PublicKeyResult *GetPublicKeyFromPrivateNonDel(char *privateKey,
                                               uint32_t keySize);
//! Создает начальный блок в формате proto.
Result *InitStartBlock();
//! Майнит блок, возвращает его в формате proto. Очищает переданные указатели
Result *MineBlockWithDel(char *data, uint32_t dataSize,
                         char *prevBlockInProtoFormat, uint32_t prevBlockSize,
                         char *privateKey, uint32_t keySize,
                         uint32_t complexity, uint32_t miningTime,
                         bool needToValidateKey);
//! Майнит блок, возвращает его в формате proto. Не очищает переданные указатели
Result *MineBlockNonDel(char *data, uint32_t dataSize,
                        char *prevBlockInProtoFormat, uint32_t prevBlockSize,
                        char *privateKey, uint32_t keySize, uint32_t complexity,
                        uint32_t miningTime, bool needToValidateKey);
void FreeResult(Result *result);
void FreePK(PublicKeyResult *publicKey);
int GetResultErrorSize(Result *result);
char *GetResultError(Result *result);
int GetResultDataSize(Result *result);
char *GetResultData(Result *result);
int GetPKErrorSize(PublicKeyResult *publicKey);
char *GetPKError(PublicKeyResult *publicKey);
int GetFirstKeySize(PublicKeyResult *publicKey);
char *GetFirstKey(PublicKeyResult *publicKey);
int GetSecondKeySize(PublicKeyResult *publicKey);
char *GetSecondKey(PublicKeyResult *publicKey);
void ClearBuf(char *buf);
}
