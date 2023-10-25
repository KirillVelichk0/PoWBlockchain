//
// Created by houdini on 25.10.2023.
//


#ifndef BLOCKCHAINCORE_PREDEFS_H
#define BLOCKCHAINCORE_PREDEFS_H
#include <boost/predef.h>
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
consteval std::string_view GetCurArch(){
    return "X86_64";
}
#elif
consteval std::string_view GetCurArch(){
    return "Unknown";
}
#endif

#endif //BLOCKCHAINCORE_PREDEFS_H
