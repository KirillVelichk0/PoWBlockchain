//
// Created by houdini on 08.09.2023.
//
#include "Block.h"
#include "../Crypto/Crypto.h"
#include "../proto/gen/BlockExternal.pb.h"
#include <algorithm>
#include <bit>
#include <boost/date_time/posix_time/conversion.hpp>
#include <boost/random.hpp>
#include <boost/random/random_device.hpp>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <exception>
#include <fmt/core.h>
#include <fmt/format.h>
#include <iterator>
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
  std::memcpy(&result, &val, sizeof(result));
  ChangeToLittleEndian(result);
  return result;
}
Block::Block(const BlockHashInfo &hashInfo, const UnixTime &timestamp,
             const std::pair<std::string, std::string> &minedBy,
             const std::uint64_t &ledgerId,
             const BlockConsensusInfo &consensusInfo,
             const ByteVector &contData)
    : hashInfo(hashInfo), timestamp(timestamp), minedBy(minedBy),
      ledgerId(ledgerId), consensusInfo(consensusInfo),
      containedData(contData) {}
Block::Block(BlockHashInfo &&hashInfo, const UnixTime &timestamp,
             std::pair<std::string, std::string> &&minedBy,
             const std::uint64_t &ledgerId,
             const BlockConsensusInfo &consensusInfo, ByteVector &&contData)
    : hashInfo(std::move(hashInfo)), timestamp(timestamp),
      minedBy(std::move(minedBy)), ledgerId(ledgerId),
      consensusInfo(consensusInfo), containedData(std::move(contData)) {}
const BlockHashInfo &Block::GetHashInfo() const noexcept {
  return this->hashInfo;
}
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

const std::uint64_t &Block::GetTransactionId() const noexcept {
  return this->ledgerId;
}
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
void Block::SetHashInfo(const BlockHashInfo &hashInfo) {
  this->hashInfo = hashInfo;
}
void Block::SetHashInfo(BlockHashInfo &&hashInfo) noexcept {
  this->hashInfo = std::move(hashInfo);
}
const UnixTime &Block::GetTimestamp() const noexcept { return this->timestamp; }
void Block::SetTimestamp(const UnixTime &timestamp) {
  this->timestamp = timestamp;
}
const std::pair<std::string, std::string> &Block::GetMinedBy() const noexcept {
  return this->minedBy;
}
void Block::SetMinedBy(const std::pair<std::string, std::string> &minedBy) {
  this->minedBy = minedBy;
}
void Block::SetMinedBy(std::pair<std::string, std::string> &&minedBy) {
  this->minedBy = std::move(minedBy);
}
const BlockConsensusInfo &Block::GetConsensusInfo() const noexcept {
  return this->consensusInfo;
}
void Block::SetLuck(double luck) { this->consensusInfo.luck = luck; }
void Block::SetMiningPoint(std::uint64_t miningPoint) {
  this->consensusInfo.miningPoint = miningPoint;
}
const ByteVector &Block::GetContainedData() const noexcept {
  return this->containedData;
}
void Block::SetContainedData(const ByteVector &contData) {
  this->containedData = contData;
}
void Block::SetContainedData(ByteVector &&contData) {
  this->containedData = std::move(contData);
}
std::uint64_t Block::GetHashingBlockSize() const noexcept {
  std::uint64_t result = 0;
  result += this->hashInfo.prevSignedHash.size();
  result += this->minedBy.first.size() + this->minedBy.second.size(); // minedBy
  result += 16; // BlockConsensusInfo
  static_assert(sizeof(boost::posix_time::time_duration::sec_type) == 8);
  result += 8; // timestamp
  result += sizeof(this->ledgerId);
  result += this->containedData.size();
  return result;
}
ByteVector Block::SerializeForHashing() const {
  ByteVector result;
  result.reserve(this->GetHashingBlockSize());
  std::copy(this->hashInfo.prevSignedHash.begin(),
            this->hashInfo.prevSignedHash.end(), std::back_inserter(result));
  UnixTime epoch(boost::gregorian::date(1970, 1, 1));
  // если вдруг кто-то умудрится выставить время до 1970 года, то ловим UB.
  // Хы.
  std::uint64_t secondsSinseEpoch =
      (this->timestamp - epoch).total_nanoseconds();
  ChangeToLittleEndian(secondsSinseEpoch);
  std::copy(&secondsSinseEpoch, &secondsSinseEpoch + sizeof(secondsSinseEpoch),
            std::back_inserter(result));
  const auto &x = this->minedBy.first;
  const auto &y = this->minedBy.second;
  std::copy(x.begin(), x.end(), std::back_inserter(result));
  std::copy(y.begin(), y.end(), std::back_inserter(result));
  auto ledgerId = this->ledgerId;
  ChangeToLittleEndian(ledgerId);
  std::copy(&ledgerId, &ledgerId + sizeof(ledgerId),
            std::back_inserter(result));
  auto miningPoint = this->consensusInfo.miningPoint;
  ChangeToLittleEndian(miningPoint);
  std::copy(&miningPoint, &miningPoint + sizeof(miningPoint),
            std::back_inserter(result));
  auto luck = GetHashAbleReprOfDouble(this->consensusInfo.luck);
  std::copy(&luck, &luck + sizeof(luck), std::back_inserter(result));
  std::copy(this->containedData.begin(), this->containedData.end(),
            std::back_inserter(result));
  return result;
}

tl::expected<std::true_type, NestedError>
Block::CheckBlockIsCryptValid() noexcept {
  auto loc = std::source_location::current();
  return Crypto::TryToVerifyECDSA_CryptoPP(this->hashInfo.curSignedHash,
                                           this->SerializeForHashing(),
                                           this->minedBy, false)
      .map_error([&loc, this](NestedError &&nested) {
        std::ostringstream oss;
        oss << this->ledgerId;
        return NestedError(
            fmt::format("Error with standart validator. ledgerId - {0}",
                        oss.str()),
            std::move(nested), loc);
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
          return {fmt::format("Error with external validator. ledgerId - {0}",
                              oss.str()),
                  std::move(nested), loc};
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
block_external::v1::Block
ConstructProtoFromBlockImpl(Block &block) noexcept(false) {
  block_external::v1::Block converted;
  {
    std::string curHashTrans(block.GetHashInfo().curSignedHash.size(), 'a');
    std::memcpy((void *)curHashTrans.data(),
                block.GetHashInfo().curSignedHash.data(), curHashTrans.size());
    converted.set_cur_hash(std::move(curHashTrans));
  }
  {
    std::string prevHashTrans(block.GetHashInfo().prevSignedHash.size(), 'a');
    std::memcpy((void *)prevHashTrans.data(),
                block.GetHashInfo().prevSignedHash.data(),
                prevHashTrans.size());
    converted.set_prev_hash(std::move(prevHashTrans));
  }
  {
    UnixTime epoch(boost::gregorian::date(1970, 1, 1));
    // если вдруг кто-то умудрится выставить время до 1970 года, то ловим UB.
    // Хы.
    std::uint64_t nanoSecondsSinseEpoch =
        (block.GetTimestamp() - epoch).total_nanoseconds();
    converted.set_unix_timestamp(nanoSecondsSinseEpoch);
  }
  {
    std::unique_ptr<block_external::v1::Block::Key> minedBy =
        std::make_unique<block_external::v1::Block::Key>();
    minedBy->set_x(block.GetMinedBy().first);
    minedBy->set_y(block.GetMinedBy().second);
    converted.set_allocated_mined_by(minedBy.release());
  }
  converted.set_ledger_id(block.GetTransactionId());
  converted.set_mining_points(block.GetConsensusInfo().miningPoint);
  converted.set_luck(block.GetConsensusInfo().luck);
  {
    std::string data(block.GetContainedData().size(), 'a');
    std::memcpy((void *)data.data(), block.GetContainedData().data(),
                data.size());
    converted.set_contained_data(std::move(data));
  }
  return converted;
}
Block ConstructBlockFromProtoImpl(
    const block_external::v1::Block &protoBlock) noexcept(false) {
  auto result = Block::init();
  BlockHashInfo hashInfo;
  {
    const auto &curHash = protoBlock.cur_hash();
    hashInfo.curSignedHash.resize(curHash.size());
    std::memcpy(hashInfo.curSignedHash.data(), curHash.data(), curHash.size());
  }
  {
    const auto &prevHash = protoBlock.prev_hash();
    hashInfo.prevSignedHash.resize(prevHash.size());
    std::memcpy(hashInfo.prevSignedHash.data(), prevHash.data(),
                prevHash.size());
  }
  result.SetHashInfo(std::move(hashInfo));
  {
    auto nanos = protoBlock.unix_timestamp();
    // boost::posix_time::time_duration использует в качестве формата
    // наносекунд тип данных long, размер которого зависит от платформы
    static_assert(sizeof(long) == sizeof(std::uint64_t),
                  "boost::posix_time::time_duration использует не 64 бита");
    using time_point = std::chrono::system_clock::time_point;
    time_point uptime_timepoint{
        std::chrono::duration_cast<time_point::duration>(
            std::chrono::nanoseconds(nanos))};
    std::time_t timepoint =
        std::chrono::system_clock::to_time_t(uptime_timepoint);
    result.SetTimestamp(boost::posix_time::from_time_t(timepoint));
  }
  {
    result.SetMinedBy(
        std::make_pair(protoBlock.mined_by().x(), protoBlock.mined_by().y()));
  }
  result.SetTransactionId(protoBlock.ledger_id());
  result.SetMiningPoint(protoBlock.mining_points());
  result.SetLuck(protoBlock.luck());
  {
    ByteVector containedData(protoBlock.contained_data().size());
    std::memcpy(containedData.data(), protoBlock.contained_data().data(),
                containedData.size());
    result.SetContainedData(std::move(containedData));
  }
  return result;
}
tl::expected<std::string, NestedError>
Block::ConvertToProto(Block &block) noexcept {
  try {
    auto converted = ConstructProtoFromBlockImpl(block);
    std::string result;
    if (!converted.SerializeToString(&result)) {
      return tl::unexpected(NestedError("protobuf cant serialize to string",
                                        std::source_location::current()));
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
tl::expected<std::true_type, NestedError>
Block::ConvertToProto(Block &block, std::ostream &outputStream) noexcept {
  try {
    auto converted = ConstructProtoFromBlockImpl(block);
    if (!converted.SerializeToOstream(&outputStream)) {
      return tl::unexpected(NestedError("Cant serialize block to stream",
                                        std::source_location::current()));
    }
    return std::true_type{};
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
    return ConstructBlockFromProtoImpl(protoBlock);

  } catch (std::exception &e) {
    return tl::unexpected(
        NestedError(e.what(), std::source_location::current()));
  } catch (...) {
    return tl::unexpected(
        NestedError("unexpected error", std::source_location::current()));
  }
}
tl::expected<Block, NestedError>
Block::CreateFromProto(std::istream &inputStream) noexcept {
  try {
    block_external::v1::Block protoBlock;
    if (!protoBlock.ParseFromIstream(&inputStream)) {
      return tl::unexpected(NestedError("Cant parse proto from stream",
                                        std::source_location::current()));
    }
    return ConstructBlockFromProtoImpl(protoBlock);

  } catch (std::exception &e) {
    return tl::unexpected(
        NestedError(e.what(), std::source_location::current()));
  } catch (...) {
    return tl::unexpected(
        NestedError("unexpected error", std::source_location::current()));
  }
}
bool operator==(const Block &lhs, const Block &rhs) {
  return lhs.GetMinedBy() == rhs.GetMinedBy() &&
         lhs.GetHashInfo().curSignedHash == rhs.GetHashInfo().curSignedHash &&
         lhs.GetHashInfo().prevSignedHash == rhs.GetHashInfo().prevSignedHash &&
         lhs.GetTimestamp() == rhs.GetTimestamp() &&
         lhs.GetConsensusInfo().luck == rhs.GetConsensusInfo().luck &&
         lhs.GetConsensusInfo().miningPoint ==
             rhs.GetConsensusInfo().miningPoint &&
         lhs.GetTransactionId() == rhs.GetTransactionId() &&
         lhs.GetContainedData() == rhs.GetContainedData();
}

} // namespace BlockChainCore
