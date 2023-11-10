//
// Created by houdini on 08.09.2023.
//

#include "Crypto.h"
#include <cryptopp/eccrypto.h>
#include <cryptopp/asn.h>
#include <cryptopp/oids.h>
#include <cryptopp/seckey.h>
#include <optional>
#include "../Logger/ILogger.h"
#include "../Logger/DefaultLoggers.h"
#include <sstream>
namespace BlockChainCore {
    namespace ASN1 = CryptoPP::ASN1;
    using ECDSA256 = CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>;

    std::optional<ECDSA256::PublicKey> ImportPublicKey(const std::pair<std::string, std::string>& publicKey) noexcept{
        std::optional<ECDSA256::PublicKey> result = ECDSA256 ::PublicKey{};
        {
            auto log = ConstructTraceStartingLog("BlockChainCore::ImportPublicKey");
            Log(log);
        }
        try {
            CryptoPP::RandomNumberGenerator rng;
            std::istringstream ss(publicKey.first + " " + publicKey.second);
            //ss.exceptions(std::ios::failbit);
            CryptoPP::Integer x;
            CryptoPP::Integer y;
            ss >> x >> y;
            if (ss.fail()){
                using namespace std::string_literals;
                using namespace std::string_view_literals;
                std::string message = "There was a problem during the parsing of the public key."
                                      " Cant validate public key. ss >> x >> y set failbit\n"
                                      " Public key: "s + publicKey.first + " " + publicKey.second ;
                auto log = ConstructDefaultLog("BlockChainCore::ImportPublicKey", LogTypeEnum::Warn, message,
                                    ConstructWhatHappened("ss >> x >> y set failbit\n "
                                                          "ss has type std::istringstream"));
                Log(log);
                return {};
            }
            const CryptoPP::ECPPoint point(x, y);
            result.value().Initialize(ASN1::secp256k1(), point);
            auto isOk = result.value().Validate(rng, 3);
            if(!isOk){
                using namespace std::string_literals;
                using namespace std::string_view_literals;
                std::string message = "There was a problem during the import of the public key."
                                      " Cant validate public key. result.value().Validate(rng, 3) return false\n"
                                      " Public key: "s + publicKey.first + " " + publicKey.second ;
                auto log = ConstructDefaultLog("BlockChainCore::ImportPublicKey", LogTypeEnum::Warn, message,
                                    ConstructWhatHappened("result.value().Validate(rng, 3) return false"));
                Log(log);
                result.reset();
            }
        }
        catch (...){
            using namespace std::string_literals;
            using namespace std::string_view_literals;
            std::string message = "There was a problem during the import of the public key."
                                       " CryptoPP is poorly documented, it`s throw unknown exception,"
                                       " so it was not possible to get accurate information."
                                       " Public key: "s + publicKey.first + " " + publicKey.second ;
            auto exceptionContext = ConstructExceptionAdditionalContext("Unknown"sv);
            auto log = ConstructDefaultLog("BlockChainCore::ImportPublicKey", LogTypeEnum::Warn, message,
                                exceptionContext.first, exceptionContext.second);
            Log(log);
            result.reset();
        }
        return result;
    }
    [[nodiscard]]
    bool Crypto::TryToVerifyECDSA_CryptoPP(const ByteVector& signature, const ByteVector& blockData, const std::pair<std::string, std::string>& publicKey) noexcept{
        try {
            {
                auto log = ConstructTraceStartingLog("BlockChainCore::Crypto::TryToVerifyECDSA_CryptoPP");
                Log(log);
            }
            auto pubKeyImported = ImportPublicKey(publicKey);
            if(!pubKeyImported.has_value()){
                return false; //уже логируется. Если же начать выводить сигнатуру и данные блока, получится слишком много.
            }
            ECDSA256 ::Verifier verifier(pubKeyImported.value());
            auto isOk = verifier.VerifyMessage((CryptoPP::byte*)(blockData.data()), blockData.size(),
                                   (CryptoPP::byte*)(signature.data()), signature.size());
            if (!isOk){
                using namespace std::string_literals;
                using namespace std::string_view_literals;
                //add url64 encoding fot signature and blockData
                std::string message = "There was a problem during verifying BlockData."
                                      " verifier.VerifyMessage return false\n"
                                      //" signature:\n "s + std::string((char*)signature.data()) +"\n"+
                                      //" BlockData: \n"s + std::string((char*)signature.data())
                                      " Public key: "s + publicKey.first + " " + publicKey.second ;
                auto log = ConstructDefaultLog("BlockChainCore::Crypto::TryToVerifyECDSA_CryptoPP", LogTypeEnum::Warn, message,
                                    ConstructWhatHappened("verifier.VerifyMessage return false"));
                Log(log);
            }
            return isOk;
        } catch(...){
            return false;
        }
    }
} // BlockChainCore