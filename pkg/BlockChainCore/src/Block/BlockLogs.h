//
// Created by houdini on 10.11.2023.
//

#ifndef BLOCKCHAINCORE_BLOCKLOGS_H
#define BLOCKCHAINCORE_BLOCKLOGS_H
#include <boost/multiprecision/gmp.hpp>
#include <source_location>
#include "../Logger/DefaultLoggers.h"
namespace BlockChainCore{
    class BlockLogs {
    public:

        BlockLogs() = delete;
        static void LogIsValidInfo(const boost::multiprecision::mpz_int& ledgerId, const std::source_location& loc = std::source_location::current());

        template<ExceptionConcept ExceptionType>
        static void LogBlockException(ExceptionType& ex, std::string_view exType,  const std::source_location& loc = std::source_location::current()){
            decltype(auto) context = ConstructExceptionAdditionalContext(exType);
            auto log = ConstructDefaultLog(loc, LogTypeEnum::Warn, ex.what(), context.first, context.second);
            Log(log);
        }
        static void LogBlockUnknownException(std::string_view message,  const std::source_location& loc = std::source_location::current());
    };
}



#endif //BLOCKCHAINCORE_BLOCKLOGS_H
