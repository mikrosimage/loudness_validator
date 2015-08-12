#ifndef _LOUDNESS_ANALYSER_UTIL_HARDWARE_DETECTION_HPP_
#define _LOUDNESS_ANALYSER_UTIL_HARDWARE_DETECTION_HPP_

namespace Loudness {
namespace common {
namespace details {

#if defined (_MSC_VER) || defined (__INTEL_COMPILER)
  #include <intrin.h>
#endif

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

}


class HardwareDetection
{
public:
	enum EVendor {
		eVendorUnknow = 0,
		eVendorIntel,
		eVendorAmd
	};

	enum ESimd {
		eSimdUnknown = 0,
		eSimdNone,
		eSimdMMX,
		eSimdExtendedMMX,
		eSimd3DNow,
		eSimd3DNow2, //3DNowProfessional 3DNow+
		eSimdSSE,
		eSimdSSE2,
		eSimdSSE3,
		eSimdSSSE3,
		eSimdSSE41,
		eSimdSSE42,
		eSimdSSE4a
	};

public:
	HardwareDetection()
	{}

	EVendor getCpuVendor()
	{
	    EVendor vendor = eVendorUnknow;
	    details::cpuid( 0, eax, ebx, ecx, edx );
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

	bool hasSimdMMX()
	{
		details::cpuid( 1, eax, ebx, ecx, edx );
		return ( edx & CPUID_FLAG_MMX );
	}

	bool hasSimdExtendedMMX()
	{
		details::cpuid( 1, eax, ebx, ecx, edx );
		return ( edx & CPUID_FLAG_EXT_MMX );
	}

	bool hasSimdSSE()
	{
		details::cpuid( 1, eax, ebx, ecx, edx );
		return ( edx & CPUID_FLAG_SSE );
	}

	bool hasSimdSSE2()
	{
		details::cpuid( 1, eax, ebx, ecx, edx );
		return ( edx & CPUID_FLAG_SSE2 );
	}

	bool hasSimdSSE3()
	{
		details::cpuid( 1, eax, ebx, ecx, edx );
		return ( ecx & CPUID_FLAG_SSE3 );
	}

	bool hasSimdSSSE3()
	{
		details::cpuid( 1, eax, ebx, ecx, edx );
		return ( ecx & CPUID_FLAG_SSSE3 );
	}

	bool hasSimdSSE4_1()
	{
		details::cpuid( 1, eax, ebx, ecx, edx );
		return ( ecx & CPUID_FLAG_SSE41);
	}

	bool hasSimdSSE4_2()
	{
		details::cpuid( 1, eax, ebx, ecx, edx );
		return ( ecx & CPUID_FLAG_SSE42 );
	}

	bool hasSimdSSE4a()
	{
		if( ! ( getCpuVendor() == eVendorAmd ) )
			return false;
		details::cpuid( 1, eax, ebx, ecx, edx );
		return ( ecx & CPUID_FLAG_SSE4A );
	}

	bool hasSimdAVX()
	{
		details::cpuid( 1, eax, ebx, ecx, edx );
		return ( ecx & CPUID_FLAG_AVX );
	}

	bool hasSimdAVX2()
	{
		details::cpuid( 2, eax, ebx, ecx, edx );
		return ( ecx & CPUID_FLAG_AVX2 );
	}

	bool hasHyperThreading()
	{
		details::cpuid( 1, eax, ebx, ecx, edx );
		return ( edx & CPUID_FLAG_HTT );
	}

private:
	unsigned long eax, ebx, ecx, edx;
};

}
}

#endif
