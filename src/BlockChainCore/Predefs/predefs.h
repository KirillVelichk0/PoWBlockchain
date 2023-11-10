//
// Created by houdini on 25.10.2023.
//


#ifndef BLOCKCHAINCORE_PREDEFS_H
#define BLOCKCHAINCORE_PREDEFS_H
#include <boost/predef.h>
#include <string_view>
namespace BlockChainCore {
#if BOOST_ARCH_ARM
    consteval std::string_view GetCurArch(){
    return "ARM";
}
#elif BOOST_ARCH_ALPHA
    consteval std::string_view GetCurArch(){
        return "ALPHA";
    }
#elif BOOST_ARCH_X86_32
    consteval std::string_view GetCurArch(){
        return "X86_32";
    }
#elif BOOST_ARCH_X86_64

    consteval std::string_view GetCurArch() {
        return "X86_64";
    }

#elif
    consteval std::string_view GetCurArch(){
        return "Unknown";
    }
#endif

#if BOOST_COMP_CLANG
    consteval std::string_view GetCurComp(){
        return "clang";
    }
#elif BOOST_COMP_GNUC

    consteval std::string_view GetCurComp() {
        return "gnu-gcc";
    }

#elif BOOST_COMP_MSVC
    consteval std::string_view GetCurComp(){
        return "msvc";
    }
#else
    consteval std::string_view GetCurComp(){
        return "unknown";
    }
#endif
#if BOOST_OS_LINUX

    consteval std::string_view GetCurOs() {
        return BOOST_OS_LINUX_NAME;
    }

#elif BOOST_OS_MACOS
    consteval std::string_view GetCurOs(){
        return BOOST_OS_MACOS_NAME;
    }
#elif BOOST_OS_BSD
    consteval std::string_view GetCurOs(){
        return BOOST_OS_BSD_NAME;
    }
#elif BOOST_OS_WINDOWS
    consteval std::string_view GetCurOs(){
        return BOOST_OS_WINDOWS_NAME;
    }
#else
    consteval std::string_view GetCurOs(){
        return Uknown;
    }
#endif

#if BOOST_ENDIAN_BIG_BYTE
    consteval std::string_view GetCurEndian(){
        return "BigByte";
    }
#elif BOOST_ENDIAN_BIG_WORD
    consteval std::string_view GetCurEndian(){
        return "BigWord";
    }
#elif BOOST_ENDIAN_LITTLE_BYTE

    consteval std::string_view GetCurEndian() {
        return "LittleByte";
    }

#elif BOOST_ENDIAN_LITTLE_WORD
    consteval std::string_view GetCurEndian(){
        return "LittleWord";
    }
#else
    consteval std::string_view GetCurEndian(){
        return "Uknown";
    }
#endif
}
#endif //BLOCKCHAINCORE_PREDEFS_H
