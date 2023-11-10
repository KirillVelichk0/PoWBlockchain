//
// Created by houdini on 10.11.2023.
//
#include "BlockLogs.h"
#include "../Logger/DefaultLoggers.h"
namespace BlockChainCore{
    void BlockLogs::LogIsValidInfo(const boost::multiprecision::mpz_int& ledgerId,  const std::source_location& loc){
        std::ostringstream oss;
        oss << ledgerId;
        std::string blockId = oss.str();
        std::string message = "Starting validation check of block with id: " + blockId;
        auto log = ConstructDefaultLog(loc, LogTypeEnum::Info,
                                       message, ConstructWhatHappened("Block validating info log"));
        Log(log);
    }
}
