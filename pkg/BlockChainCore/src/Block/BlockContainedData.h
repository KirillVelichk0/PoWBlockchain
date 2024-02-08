//
// Created by houdini on 13.09.2023.
//

#ifndef BLOCKCHAINCORE_BLOCKCONTAINEDDATA_H
#define BLOCKCHAINCORE_BLOCKCONTAINEDDATA_H
#include "../Logger/DefaultLoggers.h"
#include <boost/serialization/serialization.hpp>
#include <vector>
namespace BlockChainCore {
using ByteVector = std::vector<unsigned char>;
/*! \brief Класс для хранения хранящихся в блоке данных в произвольном формате.
 * \details Единственное, за что отвечает класс BlockContainedData - это
 * хранения и сериализация данных. Данные, хранимые в нем, абсолютно
 * произвольны. За валидацию хранимых данных отвечают внешние ресурсы, никакого
 * отношения к ядру абсолютно не имеющие.
 */
class BlockContainedData {
public:
  friend class boost::serialization::access;

private:
  ByteVector data;
  template <typename Archive>
  void serialize(Archive &ar, const unsigned int version) {
    LogStartTrace();
    ar & data;
  }

public:
  [[nodiscard]] ByteVector &GetData() noexcept;
};

} // namespace BlockChainCore

#endif // BLOCKCHAINCORE_BLOCKCONTAINEDDATA_H
