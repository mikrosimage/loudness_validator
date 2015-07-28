
#include "HardwareDetection.hpp"

#if defined (_MSC_VER) || defined (__INTEL_COMPILER)
#include <intrin.h>
#endif

namespace Loudness {
namespace utils {

static inline void cpuid( int functionnumber, unsigned long& a, unsigned long& b, unsigned long& c, unsigned long& d )
{
#if defined (_MSC_VER) || defined (__INTEL_COMPILER) // Microsoft or Intel compiler, intrin.h included
        int output[4] = { -1 };
	__cpuidex(output, functionnumber, 0); // intrinsic function for CPUID
	a = output[0];
	b = output[1];
	c = output[2];
	d = output[3];
#elif defined(__GNUC__) // use inline assembly, Gnu/AT&T syntax
	__asm("cpuid" : "=a"(a),"=b"(b),"=c"(c),"=d"(d) : "a"(functionnumber),"c"(0) : );
#else // unknown platform. try inline assembly with masm/intel syntax
	unsigned long output[4];
	__asm {
		mov eax, functionnumber
		xor ecx, ecx
		cpuid;
		mov esi, output
		mov [esi],    eax
		mov [esi+4],  ebx
		mov [esi+8],  ecx
		mov [esi+12], edx
	}
	a = output[0];
	b = output[1];
	c = output[2];
	d = output[3];
#endif
}
//#define cpuid(in, a, b, c, d) asm("cpuid": "=a" (a), "=b" (b), "=c" (c), "=d" (d) : "a" (in));
/*
#define CPUID_VENDOR_AMD       "AuthenticAMD"
#define CPUID_VENDOR_INTEL     "GenuineIntel"
#define CPUID_VENDOR_VIA       "CentaurHauls"
#define CPUID_VENDOR_TRANSMETA "GenuineTMx86"
#define CPUID_VENDOR_CYRIX     "CyrixInstead"
#define CPUID_VENDOR_CENTAUR   "CentaurHauls"
#define CPUID_VENDOR_NEXGEN    "NexGenDriven"
#define CPUID_VENDOR_UMC       "UMC UMC UMC "
#define CPUID_VENDOR_SIS       "SiS SiS SiS "
#define CPUID_VENDOR_NSC       "Geode by NSC"
#define CPUID_VENDOR_RISE      "RiseRiseRise"
*/

#define CPUID_VENDOR_AMD   0x68747541
#define CPUID_VENDOR_INTEL 0x756e6547

#define CPUID_FLAG_AVX2    1 << 5  /* in ebx function 2*/
#define CPUID_FLAG_SSE3    1 << 0  /* in ecx function 1*/
#define CPUID_FLAG_SSE4A   1 << 6  /* in ecx function 1*/
#define CPUID_FLAG_SSSE3   1 << 9  /* in ecx function 1*/
#define CPUID_FLAG_SSE41   1 << 19 /* in ecx function 1*/
#define CPUID_FLAG_SSE42   1 << 20 /* in ecx function 1*/
#define CPUID_FLAG_AVX     1 << 28 /* in ecx function 1*/
#define CPUID_FLAG_MMX     1 << 23 /* in edx function 1*/
#define CPUID_FLAG_EXT_MMX 1 << 24 /* in edx function 1*/
#define CPUID_FLAG_SSE     1 << 25 /* in edx function 1*/
#define CPUID_FLAG_SSE2    1 << 26 /* in edx function 1*/
#define CPUID_FLAG_HTT     1 << 28 /* in edx function 1*/

HardwareDetection::HardwareDetection()
{
}

HardwareDetection::EVendor HardwareDetection::getCpuVendor()
{
	EVendor vendor = eVendorUnknow;
	cpuid( 0, eax, ebx, ecx, edx );
	switch(ebx)
	{
		case CPUID_VENDOR_INTEL:
			vendor = eVendorIntel;
			break;
		case CPUID_VENDOR_AMD:
			vendor = eVendorAmd;
			break;
		default:
			vendor = eVendorUnknow;
			break;
	}
	return vendor;
}

bool HardwareDetection::hasSimdMMX()
{
	cpuid( 1, eax, ebx, ecx, edx );
	return ( edx & CPUID_FLAG_MMX );
}

bool HardwareDetection::hasSimdExtendedMMX()
{
	cpuid( 1, eax, ebx, ecx, edx );
	return ( edx & CPUID_FLAG_EXT_MMX );
}

bool HardwareDetection::hasSimdSSE()
{
	cpuid( 1, eax, ebx, ecx, edx );
	return ( edx & CPUID_FLAG_SSE );
}

bool HardwareDetection::hasSimdSSE2()
{
	cpuid( 1, eax, ebx, ecx, edx );
	return ( edx & CPUID_FLAG_SSE2 );
}

bool HardwareDetection::hasSimdSSE3()
{
	cpuid( 1, eax, ebx, ecx, edx );
	return ( ecx & CPUID_FLAG_SSE3 );
}

bool HardwareDetection::hasSimdSSSE3()
{
	cpuid( 1, eax, ebx, ecx, edx );
	return ( ecx & CPUID_FLAG_SSSE3 );
}

bool HardwareDetection::hasSimdSSE4_1()
{
	cpuid( 1, eax, ebx, ecx, edx );
	return ( ecx & CPUID_FLAG_SSE41);
}

bool HardwareDetection::hasSimdSSE4_2()
{
	cpuid( 1, eax, ebx, ecx, edx );
	return ( ecx & CPUID_FLAG_SSE42 );
}

bool HardwareDetection::hasSimdSSE4a()
{
	if( ! ( getCpuVendor() == eVendorAmd ) )
		return false;
	cpuid( 1, eax, ebx, ecx, edx );
	return ( ecx & CPUID_FLAG_SSE4A );
}

bool HardwareDetection::hasSimdAVX()
{
	cpuid( 1, eax, ebx, ecx, edx );
	return ( ecx & CPUID_FLAG_AVX );
}

bool HardwareDetection::hasSimdAVX2()
{
	cpuid( 2, eax, ebx, ecx, edx );
	return ( ecx & CPUID_FLAG_AVX2 );
}

bool HardwareDetection::hasHyperThreading()
{
	cpuid( 1, eax, ebx, ecx, edx );
	return ( edx & CPUID_FLAG_HTT );
}

}
}
