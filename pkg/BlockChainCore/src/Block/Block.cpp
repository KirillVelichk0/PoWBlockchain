//
// Created by houdini on 08.09.2023.
//
#include "Block.h"
#include "../Crypto/Crypto.h"
#include "../proto/gen/BlockExternal.pb.h"
#include <algorithm>
#include <bit>
#include <boost/random.hpp>
#include <boost/random/random_device.hpp>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <exception>
#include <fmt/core.h>
#include <fmt/format.h>
#include <limits>
#include <source_location>
#include <sstream>
#include <strstream>
#include <type_traits>
namespace BlockChainCore {
static_assert(std::numeric_limits<double>::is_iec559);
void ChangeToLittleEndian(std::uint64_t &num) {
  constexpr bool isBigEndian = std::endian::native == std::endian::big;
  constexpr bool isLittleEndian = std::endian::native == std::endian::little;
  static_assert(sizeof(double) == 8);
  static_assert(isBigEndian || isLittleEndian);
  if constexpr (isBigEndian) {
    unsigned char *interpr = (unsigned char *)&num;
    std::swap(interpr[0], interpr[7]);
    std::swap(interpr[1], interpr[6]);
    std::swap(interpr[2], interpr[5]);
    std::swap(interpr[3], interpr[4]);
  }
}
void ChangeToLittleEndian(std::int64_t &num) {
  constexpr bool isBigEndian = std::endian::native == std::endian::big;
  constexpr bool isLittleEndian = std::endian::native == std::endian::little;
  static_assert(sizeof(double) == 8);
  static_assert(isBigEndian || isLittleEndian);
  if constexpr (isBigEndian) {
    unsigned char *interpr = (unsigned char *)&num;
    std::swap(interpr[0], interpr[7]);
    std::swap(interpr[1], interpr[6]);
    std::swap(interpr[2], interpr[5]);
    std::swap(interpr[3], interpr[4]);
  }
}
std::uint64_t GetHashAbleReprOfDouble(double val) {
  std::uint64_t result;
  std::memcpy(&result, &val, 8);
  ChangeToLittleEndian(result);
  return result;
}
Block::Block(const BlockHashInfo &hashInfo, const UnixTime &timestamp,
             const std::pair<std::int64_t, std::int64_t> &minedBy,
             const std::uint64_t &ledgerId,
             const BlockConsensusInfo &consensusInfo,
             const ByteVector &contData)
    : hashInfo(hashInfo), timestamp(timestamp), minedBy(minedBy),
      ledgerId(ledgerId), consensusInfo(consensusInfo),
      containedData(contData) {}
Block::Block(BlockHashInfo &&hashInfo, const UnixTime &timestamp,
             std::pair<std::int64_t, std::int64_t> &&minedBy,
             const std::uint64_t &ledgerId,
             const BlockConsensusInfo &consensusInfo, ByteVector &&contData)
    : hashInfo(std::move(hashInfo)), timestamp(timestamp),
      minedBy(std::move(minedBy)), ledgerId(ledgerId),
      consensusInfo(consensusInfo), containedData(std::move(contData)) {}
auto Block::GetHashInfo() const noexcept { return this->hashInfo; }
tl::expected<Block, NestedError> Block::PrepareBlock(const Block &lastBlock) {
  auto loc = std::source_location::current();
  try {
    Block result;
    result.hashInfo.prevSignedHash = lastBlock.GetHashInfo().curSignedHash;
    result.ledgerId = lastBlock.ledgerId + 1;
    boost::random::random_device gen;
    boost::uniform_real<double> distr(0, 1000000);
    result.consensusInfo.luck = distr(gen);
    return result;
  } catch (std::exception &ex) {
    return tl::unexpected(NestedError(ex.what(), loc));
  } catch (...) {
    return tl::unexpected(NestedError("Uknown exception catched", loc));
  }
}
Block Block::init() { return Block{}; }

auto Block::GetTransactionId() const noexcept { return this->ledgerId; }
void Block::SetCurHash(const ByteVector &curHash) {
  this->hashInfo.curSignedHash = curHash;
}
void Block::SetCurHash(ByteVector &&curHash) {
  this->hashInfo.curSignedHash = std::move(curHash);
}
void Block::SetPrevHash(const ByteVector &prevHash) {
  this->hashInfo.prevSignedHash = prevHash;
}
void Block::SetPrevHash(ByteVector &&prevHash) {
  this->hashInfo.prevSignedHash = std::move(prevHash);
}
void Block::SetTransactionId(const std::uint64_t &ledgerId) {
  this->ledgerId = ledgerId;
}
void Block::SetTransactionId(std::uint64_t &&ledgerId) {
  this->ledgerId = std::move(ledgerId);
}
auto Block::GetTimestamp() const noexcept { return this->timestamp; }
void Block::SetTimestamp(const UnixTime &timestamp) {
  this->timestamp = timestamp;
}
auto Block::GetMinedBy() const noexcept { return this->minedBy; }
void Block::SetMinedBy(const std::pair<std::int64_t, std::int64_t> &minedBy) {
  this->minedBy = minedBy;
}
void Block::SetMinedBy(std::pair<std::int64_t, std::int64_t> &&minedBy) {
  this->minedBy = std::move(minedBy);
}
auto Block::GetConsensusInfo() const noexcept { return this->consensusInfo; }
void Block::SetLuck(double luck) { this->consensusInfo.luck = luck; }
void Block::SetMiningPoint(std::uint64_t miningPoint) {
  this->consensusInfo.miningPoint = miningPoint;
}
auto Block::GetContainedData() const noexcept { return this->containedData; }
void Block::SetContainedData(const ByteVector &contData) {
  this->containedData = contData;
}
void Block::SetContainedData(ByteVector &&contData) {
  this->containedData = std::move(contData);
}
std::uint64_t Block::GetHashingBlockSize() const noexcept {
  std::uint64_t result = 0;
  result += this->hashInfo.prevSignedHash.size();
  result += 16; // minedBy
  result += 16; // BlockConsensusInfo
  static_assert(sizeof(boost::posix_time::time_duration::sec_type) == 8);
  result += 8; // timestamp
  std::ostringstream oss;
  oss << this->ledgerId;
  result += oss.str().size();
  result += this->containedData.size();
  return result;
}
ByteVector Block::SerializeForHashing() const {
  auto size = this->GetHashingBlockSize();
  char *buf;
  buf = new char[size];
  std::strstream ss(buf, size);
  // пока все операции не пройдут успешно, деаллоцируем размер буфера и удаляем
  // его
  ss.freeze(false);
  ss.write((const char *)this->hashInfo.prevSignedHash.data(),
           this->hashInfo.prevSignedHash.size());
  auto minedByFirstPart = this->minedBy.first;
  auto minedBySecondPart = this->minedBy.second;
  ChangeToLittleEndian(minedByFirstPart);
  ChangeToLittleEndian(minedBySecondPart);
  ss.write((char *)&minedByFirstPart, 8);
  ss.write((char *)&minedBySecondPart, 8);
  auto points = this->consensusInfo.miningPoint;
  ChangeToLittleEndian(points);
  auto luckHashAble = GetHashAbleReprOfDouble(this->consensusInfo.luck);
  ss.write((char *)&points, 8);
  ss.write((char *)&luckHashAble, 8);
  UnixTime epoch(boost::gregorian::date(1970, 1, 1));
  // если вдруг кто-то умудрится выставить время до 1970 года, то ловим UB.
  // Хы.
  std::uint64_t secondsSinseEpoch = (this->timestamp - epoch).total_seconds();
  ChangeToLittleEndian(secondsSinseEpoch);
  ss.write((char *)(&secondsSinseEpoch), 8);
  std::ostringstream oss;
  oss << this->ledgerId;
  ss.write(oss.str().data(), oss.str().size());
  ss.write((const char *)this->containedData.data(), containedData.size());
  ByteVector result(buf, buf + ss.tellg());
  // если вектор создан, то можно вызывать ss.freeze(), чтобы не удалять буфер,
  // т.к. он передан
  ss.freeze(true);
  return result;
}

tl::expected<std::true_type, NestedError>
Block::CheckBlockIsCryptValid() noexcept {
  auto loc = std::source_location::current();
  return Crypto::TryToVerifyECDSA_CryptoPP(this->hashInfo.curSignedHash,
                                           this->SerializeForHashing(),
                                           this->minedBy)
      .map_error([&loc, this](NestedError &&nested) {
        std::ostringstream oss;
        oss << this->ledgerId;
        return NestedError(
            fmt::format("Error with standart validator. ledgerId - {0}",
                        oss.str()),
            nested, loc);
      });
}
tl::expected<std::true_type, NestedError>
Block::CheckBlockIsCryptValid(CryptValidator validator) noexcept {
  auto loc = std::source_location::current();

  try {
    return validator(this->hashInfo.curSignedHash, this->SerializeForHashing(),
                     this->minedBy)
        .map_error([&loc, this](NestedError &&nested) -> NestedError {
          std::ostringstream oss;
          oss << this->ledgerId;
          return NestedError(
              fmt::format("Error with external validator. ledgerId - {0}",
                          oss.str()),
              nested, loc);
        });

  } catch (std::exception &ex) {
    std::ostringstream oss;
    oss << this->ledgerId;
    return tl::unexpected(
        NestedError(fmt::format("Catched exception in external validator. "
                                "ledgerId - {0}\nException: {1}",
                                oss.str(), ex.what()),
                    loc));
    return tl::unexpected(
        NestedError(ex.what(), std::source_location::current()));
  } catch (...) {
    std::ostringstream oss;
    oss << this->ledgerId;
    return tl::unexpected(NestedError(
        fmt::format("Catched unknown exception in external validator. "
                    "ledgerId - {0}\n",
                    oss.str()),
        loc));
  }
}
tl::expected<std::string, NestedError>
Block::ConvertToProto(Block &block) noexcept {
  try {
    block_external::v1::Block converted;
    {
      std::string curHashTrans('a', block.hashInfo.curSignedHash.size());
      std::memcpy((void *)curHashTrans.data(),
                  block.hashInfo.curSignedHash.data(), curHashTrans.size());
      converted.set_cur_hash(std::move(curHashTrans));
    }
    {
      std::string prevHashTrans('a', block.hashInfo.prevSignedHash.size());
      std::memcpy((void *)prevHashTrans.data(),
                  block.hashInfo.prevSignedHash.data(), prevHashTrans.size());
      converted.set_prev_hash(std::move(prevHashTrans));
    }
    {
      std::unique_ptr<google::protobuf::Timestamp> timestamp(
          new google::protobuf::Timestamp());
      UnixTime epoch(boost::gregorian::date(1970, 1, 1));
      // если вдруг кто-то умудрится выставить время до 1970 года, то ловим UB.
      // Хы.
      std::uint64_t secondsSinseEpoch =
          (block.timestamp - epoch).total_nanoseconds();
      timestamp->set_nanos(secondsSinseEpoch);
      converted.set_allocated_unix_timestamp(timestamp.release());
    }
    {
      std::unique_ptr<block_external::v1::Block::Key> minedBy(
          new block_external::v1::Block::Key());
      minedBy->set_x(block.minedBy.first);
      minedBy->set_y(block.minedBy.second);
      converted.set_allocated_mined_by(minedBy.release());
    }
    converted.set_ledger_id(block.ledgerId);
    converted.set_mining_points(block.consensusInfo.miningPoint);
    converted.set_luck(block.consensusInfo.luck);
    {
      std::string data('a', block.containedData.size());
      std::memcpy((void *)data.data(), block.containedData.data(), data.size());
      converted.set_contained_data(std::move(data));
    }
    std::string result;
    if (converted.SerializeToString(&result)) {
      return result;
    } else {
      return tl::unexpected(NestedError("protobuf cant serialize to string",
                                        std::source_location::current()));
    }
  } catch (std::exception &e) {
    return tl::unexpected(
        NestedError(e.what(), std::source_location::current()));
  } catch (...) {
    return tl::unexpected(
        NestedError("unexpected error", std::source_location::current()));
  }
}
tl::expected<Block, NestedError>
Block::CreateFromProto(const std::string &data) noexcept {
  try {
    block_external::v1::Block protoBlock;
    if (!protoBlock.ParseFromString(data)) {
      return tl::unexpected(NestedError("Cant parse proto from string",
                                        std::source_location::current()));
    }
    Block result;
    {
      auto &curHash = protoBlock.cur_hash();
      result.hashInfo.curSignedHash.resize(curHash.size());
      std::memcpy(result.hashInfo.curSignedHash.data(), curHash.data(),
                  curHash.size());
    }
    {
      auto &prevHash = protoBlock.prev_hash();
      result.hashInfo.prevSignedHash.resize(prevHash.size());
      std::memcpy(result.hashInfo.prevSignedHash.data(), prevHash.data(),
                  prevHash.size());
    }
    {
      auto nanos = protoBlock.unix_timestamp().nanos();
      result.timestamp = boost::posix_time::ptime(
          {1970, 1, 1}, boost::posix_time::time_duration(0, 0, 0, nanos));
    }
    {
      result.minedBy.first = protoBlock.mined_by().x();
      result.minedBy.second = protoBlock.mined_by().y();
    }
    result.ledgerId = protoBlock.ledger_id();
    result.consensusInfo.miningPoint = protoBlock.mining_points();
    result.consensusInfo.luck = protoBlock.luck();
    {
      result.containedData.resize(protoBlock.contained_data().size());
      std::memcpy(result.containedData.data(),
                  protoBlock.contained_data().data(),
                  result.containedData.size());
    }
    return result;
  } catch (std::exception &e) {
    return tl::unexpected(
        NestedError(e.what(), std::source_location::current()));
  } catch (...) {
    return tl::unexpected(
        NestedError("unexpected error", std::source_location::current()));
  }
}
} // namespace BlockChainCore
