//
// Created by houdini on 10.11.2023.
//

#include "CryptoLogs_ImportPubKey.h"
#include "../Logger/ILogger.h"
#include "../Logger/DefaultLoggers.h"
namespace BlockChainCore{

    void CryptoLogs_ImportPubKey::LogStringStreamFail(const std::pair<std::string, std::string>& publicKey, const std::source_location& loc){
        using namespace std::string_literals;
        using namespace std::string_view_literals;
        std::string message = "There was a problem during the parsing of the public key."
                              " Cant validate public key. ss >> x >> y set failbit\n"
                              " Public key: "s + publicKey.first + " " + publicKey.second ;
        auto log = ConstructDefaultLog(loc, LogTypeEnum::Warn, message,
                                       ConstructWhatHappened("ss >> x >> y set failbit\n "
                                                             "ss has type std::istringstream"));
        Log(log);
    }
    void CryptoLogs_ImportPubKey::LogValidatingFail(const std::pair<std::string, std::string>& publicKey, const std::source_location& loc){
        using namespace std::string_literals;
        using namespace std::string_view_literals;
        std::string message = "There was a problem during the import of the public key."
                              " Cant validate public key. result.value().Validate(rng, 3) return false\n"
                              " Public key: "s + publicKey.first + " " + publicKey.second ;
        auto log = ConstructDefaultLog(loc, LogTypeEnum::Warn, message,
                                       ConstructWhatHappened("result.value().Validate(rng, 3) return false"));
        Log(log);
    }
    void CryptoLogs_ImportPubKey::LogCatchedUknownException(const std::pair<std::string, std::string>& publicKey, const std::source_location& loc){
        using namespace std::string_literals;
        using namespace std::string_view_literals;
        std::string message = "There was a problem during the import of the public key."
                              " CryptoPP is poorly documented, it`s throw unknown exception,"
                              " so it was not possible to get accurate information."
                              " Public key: "s + publicKey.first + " " + publicKey.second ;
        auto exceptionContext = ConstructExceptionAdditionalContext("Unknown"sv);
        auto log = ConstructDefaultLog(loc, LogTypeEnum::Warn, message,
                                       exceptionContext.first, exceptionContext.second);
        Log(log);
    }
    void CryptoLogs_TryToVerifyECDSA_CryptoPP::LogVerifyMessageFail(const ByteVector& signature, const ByteVector& blockData, const std::pair<std::string, std::string>& publicKey, const std::source_location& loc){
        using namespace std::string_literals;
        using namespace std::string_view_literals;
        //add url64 encoding fot signature and blockData
        std::string message = "There was a problem during verifying BlockData."
                              " verifier.VerifyMessage return false\n"
                              //" signature:\n "s + std::string((char*)signature.data()) +"\n"+
                              //" BlockData: \n"s + std::string((char*)signature.data())
                              " Public key: "s + publicKey.first + " " + publicKey.second ;
        auto log = ConstructDefaultLog(loc, LogTypeEnum::Warn, message,
                                       ConstructWhatHappened("verifier.VerifyMessage return false"));
        Log(log);
    }
    void CryptoLogs_TryToVerifyECDSA_CryptoPP::LogUknownExceptionCatched(const ByteVector& signature, const ByteVector& blockData, const std::pair<std::string, std::string>& publicKey, const std::source_location& loc){
        using namespace std::string_literals;
        using namespace std::string_view_literals;
        //add url64 encoding fot signature and blockData
        std::string message = "Catched unknown exception\n"
                              //" signature:\n "s + std::string((char*)signature.data()) +"\n"+
                              //" BlockData: \n"s + std::string((char*)signature.data())
                              " Public key: "s + publicKey.first + " " + publicKey.second ;
        auto log = ConstructDefaultLog(loc, LogTypeEnum::Warn, message,
                                       ConstructWhatHappened("verifier.VerifyMessage return false"));
        Log(log);
    }
}