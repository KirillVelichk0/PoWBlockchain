//
// Created by houdini on 10.11.2023.
//

#ifndef BLOCKCHAINCORE_CRYPTOLOGS_IMPORTPUBKEY_H
#define BLOCKCHAINCORE_CRYPTOLOGS_IMPORTPUBKEY_H
#include <source_location>
#include <string>
#include <vector>
namespace BlockChainCore {


    class CryptoLogs_ImportPubKey {
    public:
        CryptoLogs_ImportPubKey() = delete;
        static void LogStringStreamFail(const std::pair<std::string, std::string>& publicKey, const std::source_location& loc = std::source_location::current());
        static void LogValidatingFail(const std::pair<std::string, std::string>& publicKey, const std::source_location& loc = std::source_location::current());
        static void LogCatchedUknownException(const std::pair<std::string, std::string>& publicKey, const std::source_location& loc = std::source_location::current());
    };
    class CryptoLogs_TryToVerifyECDSA_CryptoPP{
    public:
        using ByteVector = std::vector<unsigned char>;
        CryptoLogs_TryToVerifyECDSA_CryptoPP() = delete;
        static void LogVerifyMessageFail(const ByteVector& signature, const ByteVector& blockData, const std::pair<std::string, std::string>& publicKey, const std::source_location& loc = std::source_location::current());
        static void LogUknownExceptionCatched(const ByteVector& signature, const ByteVector& blockData, const std::pair<std::string, std::string>& publicKey, const std::source_location& loc = std::source_location::current());
    };
}


#endif //BLOCKCHAINCORE_CRYPTOLOGS_IMPORTPUBKEY_H
