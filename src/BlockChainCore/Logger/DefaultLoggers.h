//
// Created by houdini on 10.11.2023.
//
#ifndef BLOCKCHAINCORE_DEFAULTLOGGERS_H
#define BLOCKCHAINCORE_DEFAULTLOGGERS_H
#include "ILogger.h"
namespace BlockChainCore {
    using AddtitonalContextInfo = std::pair<std::string_view, std::string_view>;

    std::string ConstructDefaultLogCompiledSystemInfo();

    namespace{

        template < std::same_as<std::pair<std::string_view, std::string_view>>... ContextInfo>
        constexpr void InitSourceLogContext(boost::json::object& context, ContextInfo... additionalContext){
            //прости господь мою грешную душу за такие костыли
            std::initializer_list<int>{(context[additionalContext.first]= additionalContext.second,0)...};
        }
    }

    LogPackage ConstructTraceStartingLog(std::string_view sourceType) noexcept;
    AddtitonalContextInfo ConstructWhatHappened(std::string_view whatHappened) noexcept;
    AddtitonalContextInfo  ConstructExceptionType(std::string_view exceptionType) noexcept;
    std::pair<AddtitonalContextInfo, AddtitonalContextInfo> ConstructExceptionAdditionalContext(std::string_view exceptionType) noexcept;


    template <std::same_as<std::pair<std::string_view, std::string_view>>... ContextInfo>
    std::string ConstructDefaultLogContext(std::string_view currentStacktrace, ContextInfo... contextInfo){
        boost::json::object context;
        context["stacktrace"] = currentStacktrace;
        context["context_creator"] = "ConstructDefaultLogContext";
        boost::json::object additional_context;
        context["source_additional_context"] = additional_context;
        return boost::json::serialize(context);
    }

    /*! \brief Функция, предлагающая стандартный конструктор лога.
     * \details Функция записывает переданные параметры, сама получает метку времени, записывает в контекст
     * текущий stacktrace (именно поэтому функция inline) и себя как создателя лога, собирает различную
     * информацию о системе
     * \param sourceType - тип источника \n
     * \param logType - тип лога
     * \param message - сообщение самого лога
     * \param contextInfo - variadic перечисление из пар string_view. В дальнейшем
     * распаковывается и помещается внутри context/source_additional_context
     */
    template <std::same_as<std::pair<std::string_view, std::string_view>>... ContextInfo>
    inline LogPackage ConstructDefaultLog(std::string_view sourceType, LogTypeEnum logType, std::string_view message, ContextInfo... contextInfo){
        LogPackage result;
        result.sourceType = sourceType;
        result.logType = logType;
        result.message = message;
        result.timestamp = boost::posix_time::second_clock::local_time();
        std::string stacktrace = to_string(boost::stacktrace::stacktrace());
        result.context = ConstructDefaultLogContext(stacktrace, contextInfo...);
        result.systemInfo = ConstructDefaultLogCompiledSystemInfo();
        return result;
    }
}

#endif //BLOCKCHAINCORE_DEFAULTLOGGERS_H
