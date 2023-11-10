//
// Created by houdini on 10.11.2023.
//

#ifndef BLOCKCHAINCORE_BLOCKLOGS_H
#define BLOCKCHAINCORE_BLOCKLOGS_H
#include <boost/multiprecision/gmp.hpp>
#include <source_location>
namespace BlockChainCore{
    class BlockLogs {
    public:

        BlockLogs() = delete;
        static void LogIsValidInfo(const boost::multiprecision::mpz_int& ledgerId, const std::source_location& loc = std::source_location::current());
    };
}



#endif //BLOCKCHAINCORE_BLOCKLOGS_H
