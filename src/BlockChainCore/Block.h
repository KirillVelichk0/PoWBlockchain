//
// Created by houdini on 08.09.2023.
//

#ifndef BLOCKCHAINCORE_BLOCK_H
#define BLOCKCHAINCORE_BLOCK_H
#include <vector>
#include <string>
#include <sstream>
#include <type_traits>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "ContainedData.h"
#include <boost/multiprecision/gmp.hpp>
#include <functional>
namespace BlockChainCore {
class Block;
using BlockChain = std::vector<Block>;
using ByteVector = std::vector<unsigned char>;
using UnixTime = boost::posix_time::ptime;
namespace BigNums = boost::multiprecision;
    struct BlockHashInfo{
        ByteVector prevHash;
        ByteVector curHash;
    };
    struct BlockConsensusInfo{
        BigNums::mpz_int miningPoint = 0; //для алгоритма консенсуса
        BigNums::mpq_rational luck = 0.0; //для алгоритма консенсуса
    };
    class Block {
    private:
        BlockHashInfo hashInfo;
        UnixTime timestamp;
        std::pair<std::string, std::string> minedBy; //два числа в тексовом виде, разделенные точкой
        BigNums::mpz_int ledgerId = 1; //для идентификации в БД
        BlockConsensusInfo consensusInfo;
        ContainedData containedData;
        Block() = default;
        explicit Block(const BlockChain& currentBlockChain) noexcept(false);
    public:
        Block(const BlockHashInfo& hashInfo, const UnixTime& timestamp, const std::pair<std::string, std::string>& minedBy,
                       const BigNums::mpz_int& ledgerId, const BlockConsensusInfo& consensusInfo,
                       const ContainedData& containedData);
        Block(BlockHashInfo&& hashInfo, const UnixTime& timestamp, std::pair<std::string, std::string>&& minedBy,
                       const BigNums::mpz_int& ledgerId, const BlockConsensusInfo& consensusInfo,
                       ContainedData&& containedData);
        static Block ConstructFromChain(const BlockChain& currentBlockChain) noexcept(false);
        static Block init();
        [[nodiscard]]
        auto GetHashInfo() const noexcept;
        [[nodiscard]]
        ByteVector GetBlockBytes() const noexcept;
       template <class Callable>
               bool IsValid(Callable&& callable)
               requires std::invocable<Callable, const ByteVector&, const ByteVector&, const std::pair<std::string, std::string>&> &&
                       std::same_as<bool, std::decay_t<decltype(callable(this->hashInfo.curHash, this->GetBlockBytes(), this->minedBy))>>{
                           return callable(this->hashInfo.curHash, this->GetBlockBytes(), this->minedBy);
                       }
       [[nodiscard]]
       auto GetTransactionLedger() const noexcept;
       void SetCurHash(const ByteVector& curHash);
       void SetCurHash(ByteVector&& curHash);
       void SetPrevHash(const ByteVector& prevHash);
       void SetPrevHash(ByteVector&& prevHash);
       void SetTransactionLedger(const BigNums::mpz_int& ledgerId);
        void SetTransactionLedger(BigNums::mpz_int&& ledgerId);



    };


} // BlockChainCore

#endif //BLOCKCHAINCORE_BLOCK_H
