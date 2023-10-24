//
// Created by houdini on 08.09.2023.
//

#ifndef BLOCKCHAINCORE_BLOCK_H
#define BLOCKCHAINCORE_BLOCK_H
#include <vector>
#include <string>
#include <sstream>
#include <ios>
#include <optional>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include <type_traits>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "ContainedData.h"
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/multiprecision/gmp.hpp>
#include <functional>
namespace BlockChainCore {
class Block;
using BlockChain = std::vector<Block>;
using ByteVector = std::vector<unsigned char>;
using UnixTime = boost::posix_time::ptime;
namespace BigNums = boost::multiprecision;


    /*!
     * Данный класс полностью повторяет функционал класса boost::archive::text_oarchive.
     * Его единственная задача - обеспечить дополнительный сценарий сериализации для верификации блока.
     * Достигается это путем перегрузки шаблона serialize в структуре BlockHashInfo.
     * В случае необходимости расширения функционала переделать под text_oarchive_impl в соответствии с рекомендациями разработчиков.
     */
    class TextOArchive_ForSign: public boost::archive::text_oarchive{
        using boost::archive::text_oarchive::text_oarchive;
    };

//! \brief Информация о хэше блока
//! \details Информация о хэше блока. Хранит подписанный хэш предыдущей и текущей записи.
    struct BlockHashInfo{
        friend class boost::serialization::access;
        ByteVector prevSignedHash;
        ByteVector curSignedHash;
    private:
        /*! Версия сериализатора, сохраняющая всю информацию о хеше блока, в том числе и о текущем подписанном хеше.
         * Используется для последующего хранения данных о блоке.
        */
        template <typename Archive>
        void serialize(Archive &ar, const unsigned int version) {
            ar & prevSignedHash;
            ar & curSignedHash;
        }
        template <>
        void serialize<TextOArchive_ForSign>(TextOArchive_ForSign& ar, const unsigned int version){
            ar & prevSignedHash;
        }

    };


    //! Информация, необходимая алгоритму консенсуса
    struct BlockConsensusInfo{
        friend class boost::serialization::access;
        BigNums::mpz_int miningPoint = 0; //для алгоритма консенсуса
        BigNums::mpq_rational luck = 0.0; //для алгоритма консенсуса
    private:
        template <typename Archive>
        void save(Archive &ar, const unsigned int version) const{
            {
                std::ostringstream oss;
                oss << this->miningPoint;
                const std::string& miningStr = oss.str();
                ar & miningStr;
            }
            {
                std::ostringstream oss;
                oss << this->luck;
                const std::string& luckStr = oss.str();
                ar & luckStr;
            }
        }

        template<class Archive>
        void load(Archive &ar, const unsigned int version){
            std::string miningPointStr;
            std::string luckStr;
            ar & miningPointStr;
            ar & luckStr;
            {
                std::istringstream iss{miningPointStr};
                iss.exceptions(std::ios::failbit);
                iss >> this->miningPoint;
            }
            {
                std::istringstream iss{luckStr};
                iss.exceptions(std::ios::failbit);
                iss >> this->luck;
            }
        }
        BOOST_SERIALIZATION_SPLIT_MEMBER();
    };
//! \brief Класс, хранящий информацию о блоке
//! \details Класс, хранящий информацию о блоке. По сути, его основная задача - обеспечение сериализации данных.
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
        friend class boost::serialization::access;
        Block(const BlockHashInfo& hashInfo, const UnixTime& timestamp, const std::pair<std::string, std::string>& minedBy,
                       const BigNums::mpz_int& ledgerId, const BlockConsensusInfo& consensusInfo,
                       const ContainedData& containedData);
        Block(BlockHashInfo&& hashInfo, const UnixTime& timestamp, std::pair<std::string, std::string>&& minedBy,
                       const BigNums::mpz_int& ledgerId, const BlockConsensusInfo& consensusInfo,
                       ContainedData&& containedData);
        //! Частично конструирует блок, цепляя его к переданной ветке блокчейна
        static Block ConstructFromChain(const BlockChain& currentBlockChain) noexcept(false);
        //! Инициализирует начальный блок.
        static Block init();
        [[nodiscard]]
        auto GetHashInfo() const noexcept;

        //! Осуществляет сериализацию блока для подсчета его хеша.
        [[nodiscard]]
        ByteVector SerializeForHashing() const;
        [[nodiscard]]
        ByteVector GetBlockBytes() const;
        //! Шаблонная фукнция, осуществляющая сравнение посчитанного хеша с текущими данными
       template <class CallableVerifier>
               bool IsValid(CallableVerifier&& callable)
               requires std::invocable<CallableVerifier, const ByteVector&, const ByteVector&, const std::pair<std::string, std::string>&> &&
                        std::same_as<bool, std::decay_t<decltype(callable(this->hashInfo.curSignedHash,
                                                                          this->SerializeForHashing(), this->minedBy))>>{
                            //write logger
                           return callable(this->hashInfo.curSignedHash, this->SerializeForHashing(), this->minedBy);
                       }
       [[nodiscard]]
       auto GetTransactionId() const noexcept;
       void SetCurHash(const ByteVector& curHash);
       void SetCurHash(ByteVector&& curHash);
       void SetPrevHash(const ByteVector& prevHash);
       void SetPrevHash(ByteVector&& prevHash);
       void SetTransactionId(const BigNums::mpz_int& ledgerId);
       void SetTransactionId(BigNums::mpz_int&& ledgerId);
       [[nodiscard]]
       auto GetTimestamp() const noexcept;
       void SetTimestamp(const UnixTime& timestamp);
       [[nodiscard]]
       auto GetMinedBy() const noexcept;
       void SetMinedBy(const std::pair<std::string, std::string>& minedBy);
       void SetMinedBy(std::pair<std::string, std::string>&& minedBy);
       [[nodiscard]]
       auto GetConsensusInfo() const noexcept;
       void SetLuck(const BigNums::mpq_rational& luck);
       void SetMiningPoint(const BigNums::mpz_int& miningPoint);
       [[nodiscard]]
       auto GetContainedData() const noexcept;
       void SetContainedData(const ContainedData& contData);
       void SetContainedData(ContainedData&& contData);
    private:
        template <typename Archive>
        void save(Archive &ar, const unsigned int version) const{
            ar & this->hashInfo;
            {
                std::ostringstream oss;
                oss << this->timestamp;
                auto timestampStr = oss.str();
                ar & timestampStr;
            }
            ar & this->minedBy.first;
            ar & this->minedBy.second;
            {
                std::ostringstream oss;
                oss << this->ledgerId;
                auto ledgerIdStr = oss.str();
                ar & ledgerIdStr;
            }
            ar & this->consensusInfo;
            ar & this->containedData;
        }

        template<class Archive>
        void load(Archive &ar, const unsigned int version){
            ar & this->hashInfo;
            {
                std::string containedTimestamp;
                ar & containedTimestamp;
                std::istringstream iss{containedTimestamp};
                iss.exceptions(std::ios::failbit);
                iss >> this->timestamp;
            }
            ar & this->minedBy.first;
            ar & this->minedBy.second;
            {
                std::string ledgerIdStr;
                ar & ledgerIdStr;
                std::istringstream iss{ledgerIdStr};
                iss.exceptions(std::ios::failbit);
                iss >> this->ledgerId;
            }
            ar & this->consensusInfo;
            ar & this->containedData;
        }
        BOOST_SERIALIZATION_SPLIT_MEMBER();
    public:
        [[nodiscard]]
        bool TryWriteToStream(std::ostream& outStream) noexcept;
        [[nodiscard]]
        static std::optional<Block> GetFromStream(std::istream& inputStream) noexcept;

    };


} // BlockChainCore

#endif //BLOCKCHAINCORE_BLOCK_H
