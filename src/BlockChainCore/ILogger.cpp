//
// Created by houdini on 25.10.2023.
//

#include "ILogger.h"
#include <boost/json.hpp>
#include <boost/stacktrace.hpp>
#include "predefs.h"
namespace BlockChainCore {





    namespace {
        /*! Функция, которая реализует синглтон для осуществляения логгирования.
         *  Данная функция при нормальном сценарии использования не должна вызываться никем, кроме непосредственно фукнций ядра.
         *  Данный синглтон инициализируется ядром до начала нормального функционирования самого ядра вызовом функции SetupCore.
         *
         */
        std::shared_ptr<ILogger> &GetLoggerInternal(){
            static std::shared_ptr<ILogger> logger;
            return logger;
        }
    }
    void __SetupLogger(){} //write later

    void Log(const LogPackage &log){
        auto& logger = GetLoggerInternal();
        logger->Log(log);
    }
    DisabledLogger::~DisabledLogger(){
        //все вырублено, чилим
    }
    void DisabledLogger::Log(const LogPackage &log){
        //все вырублено, чилим
    }

    std::string ConstructDefaultLogContext(std::string_view currentStacktrace){
        boost::json::object context;
        context["stacktrace"] = currentStacktrace;
        context["context_creator"] = "ConstructDefaultLogContext";
        return boost::json::serialize(context);
    }



    std::string ConstractDefaultLogSystemInfo(){
        boost::json::object context;
        return boost::json::serialize(context);
    }
    LogPackage ConstructDefaultLog(std::string_view sourceType, LogTypeEnum logType, std::string_view message){
        LogPackage result;
        result.sourceType = sourceType;
        result.logType = logType;
        result.message = message;
        result.timestamp = boost::posix_time::second_clock::local_time();
        std::string stacktrace = to_string(boost::stacktrace::stacktrace());
        result.context = ConstructDefaultLogContext(stacktrace);
    }

}
