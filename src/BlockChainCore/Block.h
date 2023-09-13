//
// Created by houdini on 08.09.2023.
//

#ifndef BLOCKCHAINCORE_BLOCK_H
#define BLOCKCHAINCORE_BLOCK_H
#include <vector>
#include <string>
#include <sstream>
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
        //bool IsVerified(Signature) write this shit later

        auto GetHashInfo() const noexcept;
        [[nodiscard]]
        ByteVector GetBlockBytes() const noexcept;
        bool IsVerified(const bool(*verifier)(const ByteVector&, const ByteVector&, std::pair<std::string, std::string>&));
        bool IsVefified(const std::function<bool(const ByteVector&, const ByteVector&, std::pair<std::string, std::string>&)>& verifier);


    };


} // BlockChainCore

#endif //BLOCKCHAINCORE_BLOCK_H
