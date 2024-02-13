//
// Created by houdini on 08.09.2023.
//
#include "Crypto.h"
#include <cryptopp/asn.h>
#include <cryptopp/eccrypto.h>
#include <cryptopp/integer.h>
#include <cryptopp/oids.h>
#include <cryptopp/seckey.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <source_location>
#include <sstream>
#include <tl/expected.hpp>
#include <type_traits>
namespace BlockChainCore {
namespace ASN1 = CryptoPP::ASN1;
using ECDSA256 = CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>;

auto CreateValidator(CryptoPP::Integer &x, CryptoPP::Integer &y,
                     std::source_location loc) {
  return [&x, &y, loc](ECDSA256::PublicKey &key)
             -> tl::expected<ECDSA256::PublicKey, NestedError> {
    CryptoPP::RandomNumberGenerator rng;
    const CryptoPP::ECPPoint point(x, y);
    key.Initialize(ASN1::secp256k1(), point);
    auto isOk = key.Validate(rng, 3);
    if (!isOk) {
      std::ostringstream oss;
      oss << "X: " << x << "\n Y:" << y;
      std::string numbersConv = oss.str();
      return tl::unexpected(

          NestedError(fmt::format("Key is not valid\n X: {0}", numbersConv),
                      loc));
    }
    return key;
  };
}

tl::expected<ECDSA256::PublicKey, NestedError>
ImportPublicKey(const std::pair<std::string, std::string> &publicKey) noexcept {
  tl::expected<ECDSA256::PublicKey, NestedError> result =
      ECDSA256 ::PublicKey{};
  try {
    std::istringstream ss(publicKey.first + " " + publicKey.second);
    CryptoPP::Integer x;
    CryptoPP::Integer y;
    ss >> x >> y;
    if (ss.fail()) {
      return tl::unexpected(NestedError(
          fmt::format("Cant convert\n{0}\nand\n{1}\n to ECDSA256 public key",
                      publicKey.first, publicKey.second),
          std::source_location::current()));
    }
    result =
        result.and_then(CreateValidator(x, y, std::source_location::current()));
  } catch (...) {
    result = tl::unexpected(
        NestedError("Some uknown exception", std::source_location::current()));
  }
  return result;
}
[[nodiscard]] tl::expected<std::true_type, NestedError>
Crypto::TryToVerifyECDSA_CryptoPP(
    const ByteVector &signature, const ByteVector &blockData,
    const std::pair<std::string, std::string> &publicKey) noexcept {
  auto loc = std::source_location::current();

  try {
    return ImportPublicKey(publicKey)
        .and_then([&blockData, &signature, &loc](ECDSA256::PublicKey &&key)
                      -> tl::expected<std::true_type, NestedError> {
          ECDSA256 ::Verifier verifier(key);
          auto isOk = verifier.VerifyMessage(
              (CryptoPP::byte *)(blockData.data()), blockData.size(),
              (CryptoPP::byte *)(signature.data()), signature.size());
          if (!isOk) {
            return tl::unexpected(NestedError("Cant verify message", loc));
          }
          return std::true_type{};
        })
        .map_error([&loc](NestedError &&err) {
          return NestedError("Cant verify. Not correct public key", err, loc);
        });
  } catch (...) {
    return tl::unexpected(NestedError("Some uknown exception", loc));
  }
}
} // namespace BlockChainCore
