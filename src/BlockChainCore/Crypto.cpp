//
// Created by houdini on 08.09.2023.
//

#include "Crypto.h"
#include <cryptopp/eccrypto.h>
#include <cryptopp/asn.h>
#include <cryptopp/oids.h>
#include <cryptopp/seckey.h>
#include <optional>
#include <cryptopp/seckey.h>
#include <sstream>
namespace BlockChainCore {
    namespace ASN1 = CryptoPP::ASN1;
    using ECDSA256 = CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>;
    std::optional<ECDSA256::PublicKey> ImportPublicKey(const std::pair<std::string, std::string>& publicKey) noexcept{
        std::optional<ECDSA256::PublicKey> result = ECDSA256 ::PublicKey{};
        try {
            CryptoPP::RandomNumberGenerator rng;
            std::istringstream ss(publicKey.first + " " + publicKey.second);
            ss.exceptions(std::ios::failbit);
            CryptoPP::Integer x;
            CryptoPP::Integer y;
            ss >> x >> y;
            const CryptoPP::ECPPoint point(x, y);
            result.value().Initialize(ASN1::secp256k1(), point);
            auto isOk = result.value().Validate(rng, 3);
            if(!isOk){
                throw std::invalid_argument("");
            }
        } catch (...){
            result.reset();
        }
    }
    [[nodiscard]]
    bool Crypto::TryToVerifyECDSA_CryptoPP(const ByteVector& signature, const ByteVector& blockData, const std::pair<std::string, std::string>& publicKey) noexcept{
        try {
            auto pubKeyImported = ImportPublicKey(publicKey);
            if(!pubKeyImported.has_value()){
                throw std::invalid_argument("");
            }
            ECDSA256 ::Verifier verifier(pubKeyImported.value());
            auto isOk = verifier.VerifyMessage((CryptoPP::byte*)(blockData.data()), blockData.size(),
                                   (CryptoPP::byte*)(signature.data()), signature.size());
            return isOk;
        } catch(...){
            return false;
        }
    }
} // BlockChainCore