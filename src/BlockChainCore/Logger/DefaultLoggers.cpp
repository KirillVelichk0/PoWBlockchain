//
// Created by houdini on 10.11.2023.
//
#include "../Predefs/predefs.h"
#include "DefaultLoggers.h"
namespace BlockChainCore {
    std::string ConstructDefaultLogCompiledSystemInfo() {
        boost::json::object sys_info;
        sys_info["OS"] = GetCurOs();
        sys_info["ARCH"] = GetCurArch();
        sys_info["ENDIAN"] = GetCurEndian();
        sys_info["COMP"] = GetCurComp();
        return boost::json::serialize(sys_info);
    }

    AddtitonalContextInfo ConstructWhatHappened(std::string_view whatHappened) noexcept {
        using namespace std::string_view_literals;
        return std::make_pair("what_happened"sv, whatHappened);
    }

    AddtitonalContextInfo ConstructExceptionType(std::string_view exceptionType) noexcept {
        using namespace std::string_view_literals;
        return std::make_pair("exception_type"sv, exceptionType);
    }
    void LogStartTrace(const std::source_location& loc){
        auto logPackage = ConstructTraceStartingLog(loc);
        Log(logPackage);
    }

    std::pair<AddtitonalContextInfo, AddtitonalContextInfo>
    ConstructExceptionAdditionalContext(std::string_view exceptionType) noexcept {
        using namespace std::string_view_literals;
        return std::make_pair(ConstructWhatHappened("Throw Exception"sv), ConstructExceptionType(exceptionType));
    }



    LogPackage ConstructTraceStartingLog(const std::source_location& location) noexcept {
        return ConstructDefaultLog(location, LogTypeEnum::Trace, "", ConstructWhatHappened("Starting_func"));
    }
}