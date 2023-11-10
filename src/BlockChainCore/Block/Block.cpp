//
// Created by houdini on 08.09.2023.
//

#include "Block.h"
#include "../Crypto/Crypto.h"
#include <boost/random.hpp>
#include <boost/random/random_device.hpp>
namespace BlockChainCore {
    Block::Block(const BlockHashInfo& hashInfo, const UnixTime& timestamp, const std::pair<std::string, std::string>& minedBy,
          const BigNums::mpz_int& ledgerId, const BlockConsensusInfo& consensusInfo,
          const BlockContainedData& contData) : hashInfo(hashInfo), timestamp(timestamp),
                                                minedBy(minedBy), ledgerId(ledgerId), consensusInfo(consensusInfo), containedData(contData)
    {}
    Block::Block(BlockHashInfo&& hashInfo, const UnixTime& timestamp, std::pair<std::string, std::string>&& minedBy,
                 const BigNums::mpz_int& ledgerId, const BlockConsensusInfo& consensusInfo,
                 BlockContainedData&& contData) : hashInfo(std::move(hashInfo)), timestamp(timestamp),
                                                  minedBy(std::move(minedBy)), ledgerId(ledgerId),
                                                  consensusInfo(consensusInfo),
                                                  containedData(std::move(contData))
    {}
    auto Block::GetHashInfo() const noexcept {
        return this->hashInfo;
    }
    Block::Block(const BlockChain& currentBlockChain) noexcept(false){
        Block lastBlock = currentBlockChain.at(currentBlockChain.size() - 1);
        this->hashInfo.prevSignedHash = lastBlock.GetHashInfo().prevSignedHash;
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

    auto Block::GetTransactionId() const noexcept{
        return this->ledgerId;
    }
    void Block::SetCurHash(const ByteVector& curHash){
        this->hashInfo.curSignedHash = curHash;
    }
    void Block::SetCurHash(ByteVector&& curHash){
        this->hashInfo.curSignedHash = std::move(curHash);
    }
    void Block::SetPrevHash(const ByteVector& prevHash){
        this->hashInfo.prevSignedHash = prevHash;
    }
    void Block::SetPrevHash(ByteVector&& prevHash){
        this->hashInfo.prevSignedHash = std::move(prevHash);
    }
    void Block::SetTransactionId(const BigNums::mpz_int& ledgerId){
        this->ledgerId = ledgerId;
    }
    void Block::SetTransactionId(BigNums::mpz_int&& ledgerId){
        this->ledgerId = std::move(ledgerId);
    }
    auto Block::GetTimestamp() const noexcept{
        return this->timestamp;
    }
    void Block::SetTimestamp(const UnixTime& timestamp){
        this->timestamp = timestamp;
    }
    auto Block::GetMinedBy() const noexcept{
        return this->minedBy;
    }
    void Block::SetMinedBy(const std::pair<std::string, std::string>& minedBy){
        this->minedBy = minedBy;
    }
    void Block::SetMinedBy(std::pair<std::string, std::string>&& minedBy){
        this->minedBy = std::move(minedBy);
    }
    auto Block::GetConsensusInfo() const noexcept{
        return this->consensusInfo;
    }
    void Block::SetLuck(const BigNums::mpq_rational& luck){
        this->consensusInfo.luck = luck;
    }
    void Block::SetMiningPoint(const BigNums::mpz_int& miningPoint){
        this->consensusInfo.miningPoint = miningPoint;
    }
    auto Block::GetContainedData() const noexcept{
        return this->containedData;
    }
    void Block::SetContainedData(const BlockContainedData& contData){
        this->containedData = contData;
    }
    void Block::SetContainedData(BlockContainedData&& contData){
        this->containedData = std::move(contData);
    }
    ByteVector Block::SerializeForHashing() const{
        std::ostringstream oss;
        TextOArchive_ForSign oar{oss};
        oar << *this;
        const auto& strData = oss.str();
        ByteVector result(strData.size());
        std::memcpy(result.data(), strData.data(), strData.size());
        return result;
    }
    ByteVector Block::GetBlockBytes() const{
        std::stringstream ss;
        boost::archive::text_oarchive oar{ss};
        oar << *this;
        ss.seekg(0, std::ios::beg);
        auto bof = ss.tellg();
        ss.seekg(0, std::ios::end);
        auto stream_size = std::size_t(ss.tellg() - bof);
        ss.seekg(0, std::ios::beg);
        // make your vector long enough
        std::vector<unsigned char> v(stream_size);
        // read directly in
        ss.read((char*)v.data(), std::streamsize(v.size()));
        return v;
    }

    bool Block::TryWriteToStream(std::ostream& outStream) noexcept{
        try{
            boost::archive::text_oarchive oar{outStream};
            oar << *this;
            return true;
        } catch (boost::archive::archive_exception& ex){
            //write loggers
            return false;
        } catch (std::runtime_error& er){
            //write loggers
            return false;
        }
        catch (std::exception& ex){
            //write loggers
            return false;
        } catch (...){
            //write loggers
            return false;
        }
    }
    std::optional<Block> Block::GetFromStream(std::istream& inputStream) noexcept{
        try{
            Block block;
            boost::archive::text_iarchive iar{inputStream};
            iar >> block;
            return {std::move(block)};
        }catch (boost::archive::archive_exception& ex){
            //write loggers
            return {};
        } catch (std::runtime_error& er){
            //write loggers
            return {};
        }
        catch (std::exception& ex){
            //write loggers
            return {};
        } catch (...){
            //write loggers
            return {};
        }
    }



} // BlockChainCore