//
// Created by houdini on 25.10.2023.
//

#ifndef BLOCKCHAINCORE_ILOGGER_H
#define BLOCKCHAINCORE_ILOGGER_H

#include <cstdint>
#include <string>
#include <string_view>
#include <memory.h>
#include <boost/json.hpp>
#include <boost/stacktrace.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
namespace BlockChainCore {
/*! Перечисление, отображающие возможные уровни логов. Не все они обязаны обрабатываться вашим логгером,
* Однако разновидности даны все.
*/
    enum class LogTypeEnum {
        Trace, Debug, Info, Warn, Error, Fatal
    };

/*! \brief Структура, характеризующая отдельную запись в логах.
 * \details Структура LogPackage содержит следующий набор полей: \n
 * logType - отвечает за уровень записи \n
 * systemInfo - любая дополнительная информация о системе (timestamp вынесен отдельно) \n
 * timestamp - временная метка в формате Posix. Характеризует время, в которое был сгенерирован лог \n
 * message - сообщение об ошибке \n
 * context - любой дополнительный контекст об ошибке \n
 * sourceType - тип источника в формате строки. Самое щекотливое с точки зрения формата поле.
 * По умолчанию все логи от ядра генерируются с sourceType BlockChainCore.
 * Возможно, что данное поле вообще будет игнорироваться.
 * Однако, так как ваш логгер может работать не только с логами ядра, а ядро вообще ничего не должно знать о том, куда и как
 * оно логирует, данное поле было добавлено для возможности дополнительной кастомизации маршрутизации логов
 */
    struct LogPackage {
        LogTypeEnum logType;
        std::string_view systemInfo;
        boost::posix_time::ptime timestamp;
        std::string_view message;
        std::string_view context;
        std::string_view sourceType;
    };

    using AddtitonalContextInfo = std::pair<std::string_view, std::string_view>;

    std::string ConstructDefaultLogCompiledSystemInfo();

    namespace{

        template < std::same_as<std::pair<std::string_view, std::string_view>>... ContextInfo>
        constexpr void InitSourceLogContext(boost::json::object& context, ContextInfo... additionalContext){
            //прости господь мою грешную душу за такие костыли
            std::initializer_list<int>{(context[additionalContext.first]= additionalContext.second,0)...};
        }
    }

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

/*! Интерфейс, с помощью которого осуществляется логирование.
 * Важно! Так как существуют различные виды логгеров по наличию или отсутствию перехвата ошибок и прочего, данный интерфейс
 * отвечает лишь за доставку данных до логгера. Следует учитывать, что экземпляр данного класса будет создан на саммых
 * ранних стадиях начала функционирования ядра и в дальнейшем редактированию не подлежит. Со всеми аварийными ситуациями
 * должен справляться сам механизм логирования. \n
 * Важно! Под капотом используется shared_ptr. В общем, нужно грамотно определить, как деструктор влияет на время жизни объекта.
 * (например, ресурсами объекта владеет go-шный логгер) \n
 * Для обеспечения потокобезопасности Logger не должен менять свои состояния во время работы, или же должен быть защищен
 * примитивами синхронизации.
 */
    class ILogger {
    public:
        virtual ~ILogger() = 0;

        virtual void Log(const LogPackage &log) = 0;
    };


    //! Логгер выключен, чилим.
    class DisabledLogger final: public ILogger{
    public:
        DisabledLogger() = default;
        ~DisabledLogger() override;
        void Log(const LogPackage &log) override;
    };


    /*! Запускается единожды в самом начале работы программы. НЕ потоко-безопасен!
     * Скорее всего, будет вызываться не явно, а через C api. (Использовать эту фукнцию, только если вы работаете с плюсами).
     * Можно вызвать только один раз. Последующие вызовы функция игнорирует.
     * @param logger - абстракция к некоторому реальному логгеру на произвольной платформе
     */
    void SetupLogger(std::shared_ptr<ILogger> logger);


    /*!
     * Класс для тестирования логгера. Использует RAII, подсовывая переданный логгер в Singleton и возвращая DisabledLogger
     * после уничтожения.
     * Так как используется синглтон, следует быть аккуратным при тестах при запуске тестов в параллельном режиме.
     * */
    class LoggerTester{
    public:
        LoggerTester(std::shared_ptr<ILogger> logger);
        ~LoggerTester();
    };




    /*! Функция, с помощью которой осуществляется логирование. Данная функция просто вызывает установленный логгер.
     * Если он потокобезопасен, то данная функция тоже.
     * @param log- набор информации о логе. Тип LogPackage.
     */
    void Log(const LogPackage &log);
}




#endif //BLOCKCHAINCORE_ILOGGER_H
