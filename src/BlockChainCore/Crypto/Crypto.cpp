//
// Created by houdini on 08.09.2023.
//

#include "Crypto.h"
#include <cryptopp/eccrypto.h>
#include <cryptopp/asn.h>
#include <cryptopp/oids.h>
#include <cryptopp/seckey.h>
#include <optional>
#include "../Logger/DefaultLoggers.h"
#include "CryptoLogs_ImportPubKey.h"
#include <sstream>
namespace BlockChainCore {
    namespace ASN1 = CryptoPP::ASN1;
    using ECDSA256 = CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>;

    std::optional<ECDSA256::PublicKey> ImportPublicKey(const std::pair<std::string, std::string>& publicKey) noexcept{
        std::optional<ECDSA256::PublicKey> result = ECDSA256 ::PublicKey{};
        LogStartTrace(); //логируем запуск импорта
        try {
            CryptoPP::RandomNumberGenerator rng;
            std::istringstream ss(publicKey.first + " " + publicKey.second);
            CryptoPP::Integer x;
            CryptoPP::Integer y;
            ss >> x >> y;
            if (ss.fail()){
                CryptoLogs_ImportPubKey::LogStringStreamFail(publicKey);
                return {};
            }
            const CryptoPP::ECPPoint point(x, y);
            result.value().Initialize(ASN1::secp256k1(), point);
            auto isOk = result.value().Validate(rng, 3);
            if(!isOk){
                CryptoLogs_ImportPubKey::LogValidatingFail(publicKey);
                result.reset();
            }
        }
        catch (...){
            CryptoLogs_ImportPubKey::LogCatchedUknownException(publicKey);
            result.reset();
        }
        return result;
    }
    [[nodiscard]]
    bool Crypto::TryToVerifyECDSA_CryptoPP(const ByteVector& signature, const ByteVector& blockData, const std::pair<std::string, std::string>& publicKey) noexcept{
        try {
            LogStartTrace(); //логируем
            auto pubKeyImported = ImportPublicKey(publicKey);
            if(!pubKeyImported.has_value()){
                return false; //уже логируется. Если же начать выводить сигнатуру и данные блока, получится слишком много.
            }
            ECDSA256 ::Verifier verifier(pubKeyImported.value());
            auto isOk = verifier.VerifyMessage((CryptoPP::byte*)(blockData.data()), blockData.size(),
                                   (CryptoPP::byte*)(signature.data()), signature.size());
            if (!isOk){
                CryptoLogs_TryToVerifyECDSA_CryptoPP::LogVerifyMessageFail(signature, blockData, publicKey);
            }
            return isOk;
        } catch(...){
            CryptoLogs_TryToVerifyECDSA_CryptoPP::LogUknownExceptionCatched(signature, blockData, publicKey);
            return false;
        }
    }
} // BlockChainCore