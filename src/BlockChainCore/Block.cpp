//
// Created by houdini on 08.09.2023.
//

#include "Block.h"
#include <boost/random.hpp>
#include <boost/random/random_device.hpp>
#include <boost/multiprecision/random.hpp>
#include <ctime>
namespace BlockChainCore {
    Block::Block(const BlockHashInfo& hashInfo, const UnixTime& timestamp, const std::pair<std::string, std::string>& minedBy,
          const BigNums::mpz_int& ledgerId, const BlockConsensusInfo& consensusInfo,
          const ContainedData& contData) : hashInfo(hashInfo), timestamp(timestamp),
                                                        minedBy(minedBy), ledgerId(ledgerId), consensusInfo(consensusInfo), containedData(contData)
    {}
    Block::Block(BlockHashInfo&& hashInfo, const UnixTime& timestamp, std::pair<std::string, std::string>&& minedBy,
    const BigNums::mpz_int& ledgerId, const BlockConsensusInfo& consensusInfo,
            ContainedData&& contData) : hashInfo(std::move(hashInfo)), timestamp(timestamp),
    minedBy(std::move(minedBy)), ledgerId(ledgerId),
    consensusInfo(consensusInfo),
    containedData(std::move(contData))
    {}
    auto Block::GetHashInfo() const noexcept {
        return this->hashInfo;
    }
    Block::Block(const BlockChain& currentBlockChain) noexcept(false){
        Block lastBlock = currentBlockChain.at(currentBlockChain.size() - 1);
        this->hashInfo.prevHash = lastBlock.GetHashInfo().prevHash;
        this->ledgerId = lastBlock.ledgerId + 1;
        boost::random::random_device gen;
        boost::uniform_real<BigNums::mpq_rational> distr(0, 1000000);
        this->consensusInfo.luck = distr(gen);
    }
    Block Block::ConstructFromChain(const BlockChain& currentBlockChain) noexcept(false){
        return Block(currentBlockChain);
    }
    Block Block::init(){
        return Block{};
    }

    bool Block::IsVerified(const bool(*verifier)(const ByteVector&, const ByteVector&, std::pair<std::string, std::string>&)){
        return verifier(this->hashInfo.curHash, this->GetBlockBytes(), this->minedBy);
    }
    bool Block::IsVefified(const std::function<bool(const ByteVector&, const ByteVector&, std::pair<std::string, std::string>&)>& verifier){
        return verifier(this->hashInfo.curHash, this->GetBlockBytes(), this->minedBy);
    }


} // BlockChainCore