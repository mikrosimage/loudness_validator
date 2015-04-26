
#ifndef _LOUDNESS_ANALYSER_COMMON_HPP_
#define _LOUDNESS_ANALYSER_COMMON_HPP_

#include <iostream>

#ifndef PLOUD_NO_COUT
/**
 * @param[in] ... : all parameters with an operator << defined
 * @brief terminal display
 **/
 #define PLOUD_COUT( ... )  ::std::cout << __VA_ARGS__ << ::std::endl
 #define PLOUD_CERR( ... )  ::std::cerr << __VA_ARGS__ << ::std::endl

 #define PLOUD_COUT_X( N, ... ) \
	for( std::size_t i = 0; i < N; ++i ) { ::std::cout << __VA_ARGS__; } \
		::std::cout << ::std::endl

 #define PLOUD_VAR( a ) # a << ": " << a
 #define PLOUD_VAR_ENDL( a ) # a << ":" << ::std::endl << a

#else
 #define PLOUD_COUT(...)
 #define PLOUD_CERR(...)
 #define PLOUD_COUT_X( N, ... )
 #define PLOUD_VAR( a )
 #define PLOUD_VAR_ENDL( a )
#endif

#define PLOUD_COUT_VAR( a )  PLOUD_COUT( PLOUD_VAR( a ) )

#endif
