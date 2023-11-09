//
// Created by houdini on 25.10.2023.
//

#include "ILogger.h"
#include "predefs.h"
namespace BlockChainCore {





    namespace {
        /*! Функция, которая реализует синглтон для осуществляения логгирования.
         *  Данная функция при нормальном сценарии использования не должна вызываться никем, кроме непосредственно фукнций ядра.
         *  Данный синглтон инициализируется ядром до начала нормального функционирования самого ядра вызовом функции SetupCore.
         *
         */
        std::shared_ptr<ILogger> &GetLoggerInternal(){
            static std::shared_ptr<ILogger> logger = std::make_shared<DisabledLogger>();
            return logger;
        }
    }
    void SetupLogger(std::shared_ptr<ILogger> logger){
        static bool wasSetted = false;
        if(!wasSetted) {
            GetLoggerInternal() = logger;
            wasSetted = true;
        }
    }

    LoggerTester::LoggerTester(std::shared_ptr<ILogger> logger){
        GetLoggerInternal() = logger;
    }
    LoggerTester::~LoggerTester(){
        GetLoggerInternal() = std::make_shared<DisabledLogger>();
    }


    void Log(const LogPackage &log){
        auto logger = GetLoggerInternal();
        logger->Log(log);
    }
    DisabledLogger::~DisabledLogger(){
        //все вырублено, чилим
    }
    void DisabledLogger::Log(const LogPackage &log){
        //все вырублено, чилим
    }





    std::string ConstructDefaultLogCompiledSystemInfo(){
        boost::json::object sys_info;
        sys_info["OS"] = GetCurOs();
        sys_info["ARCH"] = GetCurArch();
        sys_info["ENDIAN"] = GetCurEndian();
        sys_info["COMP"] = GetCurComp();
        return boost::json::serialize(sys_info);
    }
    AddtitonalContextInfo ConstructWhatHappened(std::string_view whatHappened) noexcept{
        using namespace std::string_view_literals;
        return std::make_pair("what_happened"sv, whatHappened);
    }
    AddtitonalContextInfo  ConstructExceptionType(std::string_view exceptionType) noexcept{
        using namespace std::string_view_literals;
        return std::make_pair("exception_type"sv, exceptionType);
    }
    std::pair<AddtitonalContextInfo, AddtitonalContextInfo> ConstructExceptionAdditionalContext(std::string_view exceptionType) noexcept{
        using namespace std::string_view_literals;
        return std::make_pair(ConstructWhatHappened("Throw Exception"sv), ConstructExceptionType(exceptionType));
    }


}
