#ifndef _LOUDNESS_ANALYSER_UTIL_HARDWARE_DETECTION_HPP_
#define _LOUDNESS_ANALYSER_UTIL_HARDWARE_DETECTION_HPP_

namespace Loudness {
namespace utils {

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
	HardwareDetection();

	EVendor getCpuVendor();

	bool hasSimdMMX();
	bool hasSimdExtendedMMX();
	bool hasSimdSSE();
	bool hasSimdSSE2();
	bool hasSimdSSE3();
	bool hasSimdSSSE3();
	bool hasSimdSSE4_1();
	bool hasSimdSSE4_2();
	bool hasSimdSSE4a();

	bool hasSimdAVX();
	bool hasSimdAVX2();

	bool hasHyperThreading();

private:
	unsigned long eax, ebx, ecx, edx;
};

}
}

#endif
