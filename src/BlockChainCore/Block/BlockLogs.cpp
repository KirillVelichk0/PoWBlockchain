//
// Created by houdini on 10.11.2023.
//
#include "BlockLogs.h"
#include <string_view>
namespace BlockChainCore{
    void BlockLogs::LogIsValidInfo(const boost::multiprecision::mpz_int& ledgerId,  const std::source_location& loc){
        using namespace std::string_view_literals;
        std::ostringstream oss;
        oss << ledgerId;
        std::string blockId = oss.str();
        std::string message = "Starting validation check of block with id: " + blockId;
        auto log = ConstructDefaultLog(loc, LogTypeEnum::Info,
                                       message, ConstructWhatHappened("Block validating info log"),
                                       std::make_pair("ledger_id"sv, std::string_view(blockId)));
        Log(log);
    }
    void BlockLogs::LogBlockUnknownException(std::string_view message, const std::source_location& loc){
        auto context = ConstructExceptionAdditionalContext("Unknown");
        auto log = ConstructDefaultLog(loc, LogTypeEnum::Warn, message, context.first, context.second);
        Log(log);
    }
}
