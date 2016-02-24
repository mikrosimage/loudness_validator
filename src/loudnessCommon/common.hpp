#ifndef _LOUDNESS_COMMON_COMMON_HPP_
#define _LOUDNESS_COMMON_COMMON_HPP_

#define MAX_CHANNELS 5
#define FRAGMENT_SIZE 64

#include "system.hpp"

#include <iostream>

#if defined(__WINDOWS__)
#define LoudnessExport __declspec(dllexport)
#elif defined(__GNUC__) // Add compiler definition here...
#if __GNUC__ - 0 > 3 || (__GNUC__ - 0 == 3 && __GNUC_MINOR__ - 0 > 2)
#define LoudnessExport __attribute__((visibility("default")))
#else
#define LoudnessExport
#warning                                                                                                                    \
    "LoudnessExport not set because of a too old gcc version. The plug-in may not compile with the option -fvisible=hidden."
#endif
#else
#error "LoudnessExport not defined for this compiler..."
#endif

#ifndef PLOUD_NO_COUT
/**
 * @param[in] ... : all parameters with an operator << defined
 * @brief terminal display
 **/
#define PLOUD_COUT(...) ::std::cout << __VA_ARGS__ << ::std::endl
#define PLOUD_CERR(...) ::std::cerr << __VA_ARGS__ << ::std::endl

#define PLOUD_COUT_X(N, ...)                                                                                                \
    for(std::size_t i = 0; i < N; ++i)                                                                                      \
    {                                                                                                                       \
        ::std::cout << __VA_ARGS__;                                                                                         \
    }                                                                                                                       \
    ::std::cout << ::std::endl

#define PLOUD_VAR(a) #a << ": " << a
#define PLOUD_VAR_ENDL(a) #a << ":" << ::std::endl << a

#else
#define PLOUD_COUT(...)
#define PLOUD_CERR(...)
#define PLOUD_COUT_X(N, ...)
#define PLOUD_VAR(a)
#define PLOUD_VAR_ENDL(a)
#endif

#define PLOUD_COUT_VAR(a) PLOUD_COUT(PLOUD_VAR(a))

#endif
