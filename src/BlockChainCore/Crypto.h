//
// Created by houdini on 08.09.2023.
//

#ifndef BLOCKCHAINCORE_CRYPTO_H
#define BLOCKCHAINCORE_CRYPTO_H
#include <vector>
#include <string>
namespace BlockChainCore {
    class Crypto {
    private:
        Crypto() = default;
    public:
        using ByteVector = std::vector<unsigned char>;
        [[nodiscard]]
        static bool TryToVerifyECDSA_CryptoPP(const ByteVector& signature, const ByteVector& blockData, const std::pair<std::string, std::string>& publicKey) noexcept;

    };

} // BlockChainCore

#endif //BLOCKCHAINCORE_CRYPTO_H
