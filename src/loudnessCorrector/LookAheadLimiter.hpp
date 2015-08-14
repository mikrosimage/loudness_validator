#ifndef _LOUDNESS_CORRECTOR_LOOK_AHEAD_LIMITER_HPP_
#define _LOUDNESS_CORRECTOR_LOOK_AHEAD_LIMITER_HPP_

#include <loudnessCommon/common.hpp>

#include "RollingMax.hpp"

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/rolling_sum.hpp>
#include <boost/circular_buffer.hpp>

#include <iostream>
#include <cmath>

using namespace boost::accumulators;

namespace Loudness {
namespace corrector {

class LoudnessExport LookAheadLimiter
{
public:
	LookAheadLimiter( const float lookAheadTime, const float sampleRate, const float threshold );
	~LookAheadLimiter();

	bool process( float &value );
	bool getLastSamples( float &value );

private:
	size_t signalCircularBufferSize;
	size_t maxCircularBufferSize;

	boost::circular_buffer<float> signal;

	RollingMax<float> accMax;

	accumulator_set<float, stats<tag::rolling_sum> > accShortTimeSum;
	accumulator_set<float, stats<tag::rolling_sum> > accShortTimeSum2;

	float threshold;
	size_t processTime;
	size_t lastSamples;
};

}
}

#endif
