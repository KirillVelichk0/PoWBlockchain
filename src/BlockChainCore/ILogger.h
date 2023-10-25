//
// Created by houdini on 25.10.2023.
//

#ifndef BLOCKCHAINCORE_ILOGGER_H
#define BLOCKCHAINCORE_ILOGGER_H

#include <cstdint>
#include <string>
#include <string_view>
#include <memory.h>
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
    /*! \brief Функция, предлагающая стандартный конструктор лога.
     * \details Функция записывает переданные параметры, сама получает метку времени, записывает в контекст
     * текущий stacktrace (именно поэтому функция inline) и себя как создателя лога, собирает различную
     * информацию о системе
     */
    inline LogPackage ConstructDefaultLog(std::string_view sourceType, LogTypeEnum logType, std::string_view message);

/*! Интерфейс, с помощью которого осуществляется логирование.
 * Важно! Так как существуют различные виды логгеров по наличию или отсутствию перехвата ошибок и прочего, данный интерфейс
 * отвечает лишь за доставку данных до логгера. Следует учитывать, что экземпляр данного класса будет создан на саммых
 * ранних стадиях начала функционирования ядра и в дальнейшем редактированию не подлежит. Со всеми аварийными ситуациями
 * должен справляться сам механизм логирования.
 */
    class ILogger {
    public:
        virtual ~ILogger() = 0;

        virtual void Log(const LogPackage &log) = 0;
    };

    //! Логгер выключен, чилим.
    class DisabledLogger: public ILogger{
    public:
        ~DisabledLogger() override;
        void Log(const LogPackage &log) override;
    };


    //! Функция отвечающая за конфигурирование логгера с помощью конфига. Должна вызываться только функцией SetupCore. Не потокобезопасна.
    void __SetupLogger(); //Write later!
    //! Функция, с помощью которой осуществляется логирование. Является потокобезопасной при нормальном сценарии инициализации ядра.
    void Log(const LogPackage &log);
}




#endif //BLOCKCHAINCORE_ILOGGER_H
