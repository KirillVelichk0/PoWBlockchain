//
// Created by houdini on 10.11.2023.
//
#ifndef BLOCKCHAINCORE_DEFAULTLOGGERS_H
#define BLOCKCHAINCORE_DEFAULTLOGGERS_H
#include "ILogger.h"
#include <source_location>
namespace BlockChainCore {
    using AddtitonalContextInfo = std::pair<std::string_view, std::string_view>;

    template <class ExceptionType>
    concept ExceptionConcept = requires (ExceptionType ex){
        std::string_view(ex.what());
    };

    std::string ConstructDefaultLogCompiledSystemInfo();

    namespace{

        template < std::same_as<std::pair<std::string_view, std::string_view>>... ContextInfo>
        constexpr void InitSourceLogContext(boost::json::object& context, ContextInfo... additionalContext){
            //прости господь мою грешную душу за такие костыли. Аминь.
            std::initializer_list<int>{(context[additionalContext.first]= additionalContext.second,0)...};
        }
    }
    /*!
     * \brief Функция, автоматизирующая создание Trace лога.
     */
    LogPackage ConstructTraceStartingLog(const std::source_location& location) noexcept;
    /*!
     * \brief Функция, создающая объект контекста с ключом what_happened.
     */
    AddtitonalContextInfo ConstructWhatHappened(std::string_view whatHappened) noexcept;
    /*!
     * \brief Функция, создающая объект контекста с ключом exception_type.
     */
    AddtitonalContextInfo  ConstructExceptionType(std::string_view exceptionType) noexcept;
    /*!
     * \brief Функция, создающая объект контекста для типа исключения, помечая what_happened и exception_type.
     */
    std::pair<AddtitonalContextInfo, AddtitonalContextInfo> ConstructExceptionAdditionalContext(std::string_view exceptionType) noexcept;

    /*!
     * \brief Функция, автоматизирующая создание и логирование Trace лога.
     */
    void LogStartTrace(const std::source_location& loc = std::source_location::current());

    /*! \brief Функция, создающая дополнительный контекст для лога
     * \details Функция создает контекст в следующем формате:\n
     * stacktrace : текущий stacktrace \n
     * context_creator : ConstructDefaultLogContext
     * source_additional_context : сконструированный json из переданных объектов контекста
     * \param currentStacktrace : stacktrace, при котором была вызвана данная функция
     * \param contextInfo: variadic template переменная, представляющая собой множество пар ключ-значение
     */
    template <std::same_as<std::pair<std::string_view, std::string_view>>... ContextInfo>
    std::string ConstructDefaultLogContext(std::string_view currentStacktrace, ContextInfo... contextInfo){
        boost::json::object context;
        context["stacktrace"] = currentStacktrace;
        context["context_creator"] = "ConstructDefaultLogContext";
        boost::json::object additional_context;
        InitSourceLogContext(additional_context, contextInfo...);
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
    inline LogPackage ConstructDefaultLog(const std::source_location& location, LogTypeEnum logType, std::string_view message, ContextInfo... contextInfo){
        LogPackage result;
        result.sourceType = location.function_name();
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
