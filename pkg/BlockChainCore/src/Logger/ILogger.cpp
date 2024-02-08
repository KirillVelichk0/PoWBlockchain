//
// Created by houdini on 25.10.2023.
//

#include "ILogger.h"
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








}
