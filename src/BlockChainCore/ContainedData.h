//
// Created by houdini on 13.09.2023.
//

#ifndef BLOCKCHAINCORE_CONTAINEDDATA_H
#define BLOCKCHAINCORE_CONTAINEDDATA_H
#include <boost/serialization/serialization.hpp>
namespace BlockChainCore {

    class ContainedData {
    public:
        friend class boost::serialization::access;
    private:
        template <typename Archive>
        void serialize(Archive &ar, const unsigned int version){}
    };

} // BlockChainCore

#endif //BLOCKCHAINCORE_CONTAINEDDATA_H
