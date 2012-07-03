#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/rolling_sum.hpp>
#include <rolling_max.hpp>

#include <iostream>
using namespace boost::accumulators;

class LookAheadLimiter
{
public:
	LookAheadLimiter( float lookAheadTime, float inputGain, float outputGain, float releaseTime, float sampleRate, float threshold );
	~LookAheadLimiter();
	
	float process( float value );
	
private:
	size_t signalCircularBufferSize;
	size_t maxCircularBufferSize;
	
	float* signalCircularBuffer;
	float* maxCircularBuffer;
	
	float* signalPtr;
	float* maxPtr;
	
	accumulator_set<float, stats<tag::rolling_max> > accMax;
	
	accumulator_set<float, stats<tag::rolling_sum> > accShortTimeSum;
	accumulator_set<float, stats<tag::rolling_sum> > accShortTimeSum2;
	
	float inputGain;
	float outputGain;
	float threshold;
};

/**
  * lookAheadTime [ ms ]
  * inputGain [ dB ]
  * outputGain [ dB ]
  * releaseTime [ ms ]
  */
LookAheadLimiter::LookAheadLimiter( float lookAheadTime, float inputGain, float outputGain, float releaseTime, float sampleRate, float threshold ) :
	signalCircularBufferSize ( sampleRate * lookAheadTime * 0.0001 ),
	maxCircularBufferSize ( signalCircularBufferSize * 0.5 ),
	accMax (tag::rolling_window::window_size = signalCircularBufferSize ),
	accShortTimeSum (tag::rolling_window::window_size = maxCircularBufferSize ),
	accShortTimeSum2 (tag::rolling_window::window_size = maxCircularBufferSize ),
	inputGain ( inputGain ),
	outputGain ( outputGain ),
	threshold ( threshold )
{
	
	signalCircularBuffer = new float[ signalCircularBufferSize ];
	maxCircularBuffer = new float[ maxCircularBufferSize ];
	
	signalPtr = signalCircularBuffer;
	maxPtr = maxCircularBuffer;
}


LookAheadLimiter::~LookAheadLimiter( )
{
	delete[] signalCircularBuffer;
	delete[] maxCircularBuffer;
}


float LookAheadLimiter::process( float value )
{
	*signalPtr = value;
	accMax( *signalPtr );
	
	signalPtr++;
	if( signalPtr > ( signalCircularBuffer + signalCircularBufferSize ))
	{
		signalPtr = signalCircularBuffer;
	}
	
	// found max value on signalCiucularBuffer
	float peak = inputGain * rolling_max( accMax );
	
	float gainReduction = ( peak > threshold ) ? peak - threshold : 1.0 ;
	
	// gainReduction -= 1.0; // for better processing stability
	
	accShortTimeSum( gainReduction );
	
	float g = rolling_sum( accShortTimeSum ) / maxCircularBufferSize;
	accShortTimeSum2 ( g );
	
	float gainWithoutRelease = rolling_sum( accShortTimeSum ) / maxCircularBufferSize;
	
	float gainWithRelease = gainWithoutRelease ;
	
	// gain = ( 1 - gainWithRelease)
	
	gainWithRelease *= outputGain;
	
	//std::cout << gainWithRelease << std::endl;
	
	return *signalPtr / gainWithRelease;
}
