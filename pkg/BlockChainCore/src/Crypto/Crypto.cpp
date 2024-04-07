//
// Created by houdini on 08.09.2023.
//
#include "Crypto.h"
#include <cryptopp/asn.h>
#include <cryptopp/eccrypto.h>
#include <cryptopp/integer.h>
#include <cryptopp/oids.h>
#include <cryptopp/osrng.h>
#include <cryptopp/seckey.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <source_location>
#include <sstream>
#include <stdexcept>
#include <tl/expected.hpp>
#include <type_traits>
namespace BlockChainCore {
namespace ASN1 = CryptoPP::ASN1;
using ECDSA256 = CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>;

auto CreatePublicKeyValidator(CryptoPP::Integer &x, CryptoPP::Integer &y,
                              std::source_location loc, bool needToValidate) {
  return [&x, &y, loc, needToValidate](ECDSA256::PublicKey &key)
             -> tl::expected<ECDSA256::PublicKey, NestedError> {
    try {
      const CryptoPP::ECPPoint point(x, y);
      key.Initialize(ASN1::secp256k1(), point);
      if (needToValidate) {
        CryptoPP::AutoSeededRandomPool rng;
        auto isOk = key.Validate(rng, 3);
        if (!isOk) {
          std::ostringstream oss;
          oss << "X: " << x << "\n Y:" << y;
          std::string numbersConv = oss.str();
          return tl::unexpected(

              NestedError(
                  fmt::format("Public key is not valid\n X: {0}", numbersConv),
                  loc));
        }
      }
      return key;
    } catch (std::exception &ex) {
      return tl::unexpected(NestedError(ex.what(), loc));
    } catch (...) {
      return tl::unexpected(NestedError("Unknown error", loc));
    }
  };
}
auto CreatePrivateKeyValidator(CryptoPP::Integer &x, std::source_location loc,
                               bool needToValidate) {
  return [&x, loc, needToValidate](ECDSA256::PrivateKey &key)
             -> tl::expected<ECDSA256::PrivateKey, NestedError> {
    try {
      key.Initialize(ASN1::secp256k1(), x);
      if (needToValidate) {
        CryptoPP::AutoSeededRandomPool rng;
        auto isOk = key.Validate(rng, 3);

        if (!isOk) {
          std::ostringstream oss;
          oss << "X: " << x << "\n";
          std::string numbersConv = oss.str();
          return tl::unexpected(

              NestedError(
                  fmt::format("Private key is not valid\n X: {0}", numbersConv),
                  loc));
        }
      }
      return key;
    } catch (std::exception &ex) {
      return tl::unexpected(NestedError(ex.what(), loc));
    } catch (...) {
      return tl::unexpected(NestedError("Unknown error", loc));
    }
  };
}
tl::expected<ECDSA256::PublicKey, NestedError>
ImportPublicKey(const std::pair<std::string, std::string> &publicKey,
                bool needToValidate) noexcept {
  tl::expected<ECDSA256::PublicKey, NestedError> result =
      ECDSA256 ::PublicKey{};
  try {

    CryptoPP::Integer x(publicKey.first.c_str());
    CryptoPP::Integer y(publicKey.second.c_str());
    result = result.and_then(CreatePublicKeyValidator(
        x, y, std::source_location::current(), needToValidate));
  } catch (...) {
    result = tl::unexpected(
        NestedError("Some uknown exception", std::source_location::current()));
  }
  return result;
}
tl::expected<ECDSA256::PrivateKey, NestedError>
ImportPrivateKey(const std::string &privateKey, bool needToValidate) noexcept {
  tl::expected<ECDSA256::PrivateKey, NestedError> result =
      ECDSA256 ::PrivateKey{};
  try {
    CryptoPP::Integer x(privateKey.c_str());
    result = result.and_then(CreatePrivateKeyValidator(
        x, std::source_location::current(), needToValidate));
  } catch (...) {
    result = tl::unexpected(
        NestedError("Some uknown exception", std::source_location::current()));
  }
  return result;
}
[[nodiscard]] tl::expected<std::true_type, NestedError>
Crypto::TryToVerifyECDSA_CryptoPP(
    const ByteVector &signature, const ByteVector &blockData,
    const std::pair<std::string, std::string> &publicKey,
    bool needToValidate) noexcept {
  auto loc = std::source_location::current();

  try {
    return ImportPublicKey(publicKey, needToValidate)
        .map_error([&loc](NestedError &&err) {
          return NestedError("Cant verify. Not correct public key",
                             std::move(err), loc);
        })
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
        });
  } catch (std::exception &ex) {
    return tl::unexpected(NestedError(ex.what(), loc));
  } catch (...) {
    return tl::unexpected(NestedError("Some uknown exception", loc));
  }
}
tl::expected<Crypto::ByteVector, NestedError>
Crypto::TryToSign(const ByteVector &data, const std::string &privateKey,
                  bool needToValidate) {
  auto loc = std::source_location::current();
  try {
    return ImportPrivateKey(privateKey, needToValidate)
        .map_error([&loc](NestedError &&err) {
          return NestedError("Cant sign. Not correct private key",
                             std::move(err), loc);
        })
        .and_then([&data, &loc](ECDSA256::PrivateKey &&key)
                      -> tl::expected<ByteVector, NestedError> {
          CryptoPP::AutoSeededRandomPool prng;
          ECDSA256::Signer signer(key);
          auto sigLen = signer.MaxSignatureLength();
          ByteVector result(sigLen, 0);
          sigLen =
              signer.SignMessage(prng, data.data(), data.size(), result.data());
          result.resize(sigLen);
          return result;
        });
  } catch (std::exception &ex) {
    return tl::unexpected(NestedError(ex.what(), loc));
  } catch (...) {
    return tl::unexpected(NestedError("Some uknown exception", loc));
  }
}
ECDSA256::PublicKey
MakePublicFromPrivateInternal(const ECDSA256::PrivateKey &privateKey,
                              CryptoPP::AutoSeededRandomPool &prng) {
  ECDSA256::PublicKey publicKey;
  privateKey.MakePublicKey(publicKey);
  bool isOk = publicKey.Validate(prng, 3);
  if (!isOk) {
    throw std::runtime_error("Cant validate created public key");
  }
  return publicKey;
}
std::pair<std::string, std::pair<std::string, std::string>>
Crypto::GenerateKeys() {
  CryptoPP::AutoSeededRandomPool prng;
  ECDSA256::PrivateKey privateKey;

  privateKey.Initialize(prng, ASN1::secp256k1());
  bool isOk = privateKey.Validate(prng, 3);
  if (!isOk) {
    throw std::runtime_error("Cant validate created private key");
  }
  const auto &privateExponent = privateKey.GetPrivateExponent();
  auto publicKey = MakePublicFromPrivateInternal(privateKey, prng);
  decltype(auto) publicPoint = publicKey.GetPublicElement();
  auto getStringFromInteger = [](const CryptoPP::Integer &val) -> std::string {
    std::ostringstream oss;
    oss << val;
    if (oss.bad()) {
      throw std::runtime_error("Cant convert key to string");
    }
    return oss.str();
  };
  std::pair<std::string, std::pair<std::string, std::string>> result;
  result.first = getStringFromInteger(privateExponent);
  result.second.first = getStringFromInteger(publicPoint.x);
  result.second.second = getStringFromInteger(publicPoint.y);
  return result;
}
std::pair<std::string, std::string>
Crypto::ConstructPublicKey(const std::string &privateKey) {
  CryptoPP::Integer privateExp(privateKey.c_str());
  CryptoPP::AutoSeededRandomPool prng;
  ECDSA256::PrivateKey privateKeyImported;
  privateKeyImported.Initialize(ASN1::secp256k1(), privateExp);
  bool isOk = privateKeyImported.Validate(prng, 3);
  if (!isOk) {
    throw std::runtime_error("Cant validate created private key");
  }
  auto publicKey = MakePublicFromPrivateInternal(privateKeyImported, prng);
  auto getStringFromInteger = [](const CryptoPP::Integer &val) -> std::string {
    std::ostringstream oss;
    oss << val;
    if (oss.bad()) {
      throw std::runtime_error("Cant convert key to string");
    }
    return oss.str();
  };
  std::pair<std::string, std::string> result;
  result.first = getStringFromInteger(publicKey.GetPublicElement().x);
  result.second = getStringFromInteger(publicKey.GetPublicElement().y);
  return result;
}
ECDSA256::PublicKey
GetInternalPublicKeyFormat(const PublicKeyOptimizedImpl &key) {
  return *reinterpret_cast<ECDSA256::PublicKey *>(key.GetImpl().get());
}
ECDSA256::PrivateKey
GetInternalPrivateKeyFormat(const PrivateKeyOptimizedImpl &key) {
  return *reinterpret_cast<ECDSA256::PrivateKey *>(key.GetImpl().get());
}
PublicKeyOptimizedImpl PublicKeyOptimizedImpl::FromClassicFormat(
    const std::pair<std::string, std::string> &publicKey) noexcept(false) {
  auto importedPublicKey = ImportPublicKey(publicKey, true);
  if (!importedPublicKey.has_value()) {
    throw std::runtime_error(importedPublicKey.error().GetFullErrorMsg());
  }
  std::shared_ptr<void> impl =
      std::make_shared<ECDSA256::PublicKey>(importedPublicKey.value());
  return {std::move(impl)};
}
std::pair<std::string, std::string>
PublicKeyOptimizedImpl::ToClassicFormat() const noexcept(false) {
  auto internalKeyFormat = GetInternalPublicKeyFormat(*this);
  const auto &publicElem = internalKeyFormat.GetPublicElement();
  std::pair<std::string, std::string> result;
  {
    std::ostringstream oss;
    oss << publicElem.x;
    if (oss.bad()) {
      throw std::runtime_error("Error while casting public x to string format");
    }
    result.first = oss.str();
  }
  {
    std::ostringstream oss;
    oss << publicElem.y;
    if (oss.bad()) {
      throw std::runtime_error("Error while casting public y to string format");
    }
    result.second = oss.str();
  }
  return result;
}
PrivateKeyOptimizedImpl PrivateKeyOptimizedImpl::FromClassicFormat(
    const std::string &key) noexcept(false) {
  auto importedPrivateKey = ImportPrivateKey(key, true);
  if (!importedPrivateKey.has_value()) {
    throw std::runtime_error(importedPrivateKey.error().GetFullErrorMsg());
  }
  std::shared_ptr<void> impl =
      std::make_shared<ECDSA256::PrivateKey>(importedPrivateKey.value());
  return {std::move(impl)};
}
[[nodiscard]] std::string PrivateKeyOptimizedImpl::ToClassicFormat() const
    noexcept(false) {
  auto internalKeyFormat = GetInternalPrivateKeyFormat(*this);
  const auto &privateElem = internalKeyFormat.GetPrivateExponent();
  std::string result;
  {
    std::ostringstream oss;
    oss << privateElem;
    if (oss.bad()) {
      throw std::runtime_error("Error while casting private to string format");
    }
    result = oss.str();
  }

  return result;
}
tl::expected<std::true_type, NestedError>
Crypto::TryToVerifyECDSA_CryptoPPOptimized(
    const ByteVector &signature, const ByteVector &blockData,
    const PublicKeyOptimizedImpl &publicKey) noexcept {
  auto loc = std::source_location::current();
  try {
    auto internalPublicKey = GetInternalPublicKeyFormat(publicKey);
    ECDSA256 ::Verifier verifier(internalPublicKey);
    auto isOk = verifier.VerifyMessage(
        (CryptoPP::byte *)(blockData.data()), blockData.size(),
        (CryptoPP::byte *)(signature.data()), signature.size());
    if (!isOk) {
      return tl::unexpected(NestedError("Cant verify message", loc));
    }
    return std::true_type{};
  } catch (std::exception &ex) {
    return tl::unexpected(NestedError(ex.what(), loc));
  } catch (...) {
    return tl::unexpected(NestedError("Some uknown exception", loc));
  }
}
tl::expected<Crypto::ByteVector, NestedError>
Crypto::TryToSignOptimized(const ByteVector &data,
                           const PrivateKeyOptimizedImpl &privateKey) {
  auto loc = std::source_location::current();

  try {
    auto privateKeyInternal = GetInternalPrivateKeyFormat(privateKey);
    CryptoPP::AutoSeededRandomPool prng;
    ECDSA256::Signer signer(privateKeyInternal);
    auto sigLen = signer.MaxSignatureLength();
    ByteVector result(sigLen, 0);
    sigLen = signer.SignMessage(prng, data.data(), data.size(), result.data());
    result.resize(sigLen);
    return result;
  } catch (std::exception &ex) {
    return tl::unexpected(NestedError(ex.what(), loc));
  } catch (...) {
    return tl::unexpected(NestedError("Some uknown exception", loc));
  }
}
Crypto::SHA256Hash Crypto::GenerateSHA256(const ByteVector &data) {
  SHA256Hash result;
  const CryptoPP::byte *rawData =
      reinterpret_cast<const CryptoPP::byte *>(data.data());
  const auto dataLen = data.size();
  CryptoPP::SHA256 hash;
  hash.Update(rawData, dataLen);
  static_assert(CryptoPP::SHA256::DIGESTSIZE == 32);
  hash.Final(result.data());
  return result;
}
} // namespace BlockChainCore
