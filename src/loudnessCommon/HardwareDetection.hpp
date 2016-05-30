#ifndef _LOUDNESS_COMMON_HARDWARE_DETECTION_HPP_
#define _LOUDNESS_COMMON_HARDWARE_DETECTION_HPP_

#if defined(_MSC_VER) || defined(__INTEL_COMPILER)
#include <intrin.h>
#endif

namespace Loudness
{
namespace common
{

namespace details
{

static const unsigned long cpuidVendorAmd = 0x68747541;
static const unsigned long cpuidVendorIntel = 0x756e6547;
static const unsigned long cpuidFlagAvx2 = 1 << 5;    /* in ebx function 2*/
static const unsigned long cpuidFlagSse3 = 1 << 0;    /* in ecx function 1*/
static const unsigned long cpuidFlagSse4a = 1 << 6;   /* in ecx function 1*/
static const unsigned long cpuidFlagSsse3 = 1 << 9;   /* in ecx function 1*/
static const unsigned long cpuidFlagSse41 = 1 << 19;  /* in ecx function 1*/
static const unsigned long cpuidFlagSse42 = 1 << 20;  /* in ecx function 1*/
static const unsigned long cpuidFlagAvx = 1 << 28;    /* in ecx function 1*/
static const unsigned long cpuidFlagMmx = 1 << 23;    /* in edx function 1*/
static const unsigned long cpuidFlagExtMmx = 1 << 24; /* in edx function 1*/
static const unsigned long cpuidFlagSse = 1 << 25;    /* in edx function 1*/
static const unsigned long cpuidFlagSse2 = 1 << 26;   /* in edx function 1*/
static const unsigned long cpuidFlagHtt = 1 << 28;    /* in edx function 1*/
}

class HardwareDetection
{
public:
    enum EVendor
    {
        eVendorUnknow = 0,
        eVendorIntel,
        eVendorAmd
    };

    enum ESimd
    {
        eSimdUnknown = 0,
        eSimdNone,
        eSimdMMX,
        eSimdExtendedMMX,
        eSimd3DNow,
        eSimd3DNow2, // 3DNowProfessional 3DNow+
        eSimdSSE,
        eSimdSSE2,
        eSimdSSE3,
        eSimdSSSE3,
        eSimdSSE41,
        eSimdSSE42,
        eSimdSSE4a
    };

private:
public:
    HardwareDetection() {}

    EVendor getCpuVendor()
    {
        EVendor vendor = eVendorUnknow;
        cpuid(0, eax, ebx, ecx, edx);
        switch(ebx)
        {
            case details::cpuidVendorIntel:
            {
                vendor = eVendorIntel;
                break;
            }
            case details::cpuidVendorAmd:
            {
                vendor = eVendorAmd;
                break;
            }
            default:
            {
                vendor = eVendorUnknow;
                break;
            }
        }
        return vendor;
    }

    bool hasSimdMMX()
    {
        cpuid(1, eax, ebx, ecx, edx);
        return (edx & details::cpuidFlagMmx);
    }

    bool hasSimdExtendedMMX()
    {
        cpuid(1, eax, ebx, ecx, edx);
        return (edx & details::cpuidFlagExtMmx);
    }

    bool hasSimdSSE()
    {
        cpuid(1, eax, ebx, ecx, edx);
        return (edx & details::cpuidFlagSse);
    }

    bool hasSimdSSE2()
    {
        cpuid(1, eax, ebx, ecx, edx);
        return (edx & details::cpuidFlagSse2);
    }

    bool hasSimdSSE3()
    {
        cpuid(1, eax, ebx, ecx, edx);
        return (ecx & details::cpuidFlagSse3);
    }

    bool hasSimdSSSE3()
    {
        cpuid(1, eax, ebx, ecx, edx);
        return (ecx & details::cpuidFlagSsse3);
    }

    bool hasSimdSSE4_1()
    {
        cpuid(1, eax, ebx, ecx, edx);
        return (ecx & details::cpuidFlagSse41);
    }

    bool hasSimdSSE4_2()
    {
        cpuid(1, eax, ebx, ecx, edx);
        return (ecx & details::cpuidFlagSse42);
    }

    bool hasSimdSSE4a()
    {
        if(!(getCpuVendor() == eVendorAmd))
            return false;
        cpuid(1, eax, ebx, ecx, edx);
        return (ecx & details::cpuidFlagSse4a);
    }

    bool hasSimdAVX()
    {
        cpuid(1, eax, ebx, ecx, edx);
        return (ecx & details::cpuidFlagAvx);
    }

    bool hasSimdAVX2()
    {
        cpuid(2, eax, ebx, ecx, edx);
        return (ecx & details::cpuidFlagAvx2);
    }

    bool hasHyperThreading()
    {
        cpuid(1, eax, ebx, ecx, edx);
        return (edx & details::cpuidFlagHtt);
    }

private:
    static inline void cpuid(int functionnumber, unsigned long& a, unsigned long& b, unsigned long& c, unsigned long& d)
    {
#if defined(_MSC_VER) || defined(__INTEL_COMPILER) // Microsoft or Intel compiler, intrin.h included
        int output[4] = {-1};
        __cpuidex(output, functionnumber, 0); // intrinsic function for CPUID
        a = output[0];
        b = output[1];
        c = output[2];
        d = output[3];
#elif defined(__GNUC__) // use inline assembly, Gnu/AT&T syntax
        __asm("cpuid" : "=a"(a), "=b"(b), "=c"(c), "=d"(d) : "a"(functionnumber), "c"(0) :);
#else                   // unknown platform. try inline assembly with masm/intel syntax
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

private:
    unsigned long eax, ebx, ecx, edx;
};
}
}

#endif
