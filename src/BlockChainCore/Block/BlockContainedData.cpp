//
// Created by houdini on 13.09.2023.
//

#include "BlockContainedData.h"

namespace BlockChainCore {
    ByteVector& BlockContainedData::GetData() noexcept{
        return this->data;
    }
} // BlockChainCore