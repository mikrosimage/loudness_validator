#ifndef LOOK_AHEAD_LIMITER_H
#define LOOK_AHEAD_LIMITER_H

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/rolling_sum.hpp>
#include <RollingMax.hpp>

#include <iostream>
#include <cmath>
using namespace boost::accumulators;

class LookAheadLimiter
{
public:
	LookAheadLimiter( const float lookAheadTime, const float sampleRate, const float threshold );
	~LookAheadLimiter();
	
	bool process( float &value );
	bool getLastSamples( float &value );
	
private:
	size_t signalCircularBufferSize;
	size_t maxCircularBufferSize;
	
	float* signalCircularBuffer;
	float* maxCircularBuffer;
	
	float* signalPtr;
	
	RollingMax<float> accMax;
	
	accumulator_set<float, stats<tag::rolling_sum> > accShortTimeSum;
	accumulator_set<float, stats<tag::rolling_sum> > accShortTimeSum2;
	
	float threshold;
	size_t processTime;
	size_t lastSamples;
};

/**
  * lookAheadTime [ ms ]
  * inputGain [ dB ]
  * releaseTime [ ms ]
  */
LookAheadLimiter::LookAheadLimiter( const float lookAheadTime, const float sampleRate, const float threshold ) :
	signalCircularBufferSize ( sampleRate * lookAheadTime * 0.0001 ),
	maxCircularBufferSize ( signalCircularBufferSize * 0.5 ),
	accMax( signalCircularBufferSize ),
	accShortTimeSum (tag::rolling_window::window_size = maxCircularBufferSize ),
	accShortTimeSum2 (tag::rolling_window::window_size = maxCircularBufferSize ),
	threshold ( threshold ),
	processTime( 0 ),
	lastSamples( 0 )
{
	signalCircularBuffer = new float[ signalCircularBufferSize ];
	maxCircularBuffer = new float[ maxCircularBufferSize ];
	
	for( size_t i=0; i< maxCircularBufferSize; i++ )
	{
		signalCircularBuffer[i] = 0.0;
		maxCircularBuffer[i] = 0.0;
	}
	
	signalPtr = signalCircularBuffer;
}


LookAheadLimiter::~LookAheadLimiter( )
{
	delete[] signalCircularBuffer;
	delete[] maxCircularBuffer;
}


bool LookAheadLimiter::process( float& value )
{
	processTime++;
	*signalPtr = value;
	accMax( fabs( value ) );
	
	signalPtr++;
	if( signalPtr > ( signalCircularBuffer + signalCircularBufferSize ) )
	{
		signalPtr = signalCircularBuffer;
	}
	
	// found max value on signalCiucularBuffer
	float peak = accMax.getMax();
	
	float gainReduction = ( peak > threshold ) ? threshold / peak : 1.0;
	
	accShortTimeSum( gainReduction );
	
	float g = rolling_sum( accShortTimeSum ) / maxCircularBufferSize;
	accShortTimeSum2 ( g );
	
	float finalGain = rolling_sum( accShortTimeSum ) / maxCircularBufferSize;
	
	value = *signalPtr * finalGain;
	
	if( processTime < signalCircularBufferSize )
		return false;
	
	return true;
}

bool LookAheadLimiter::getLastSamples( float &value )
{
	lastSamples++;
	value = 0.0;
	process( value );
	
	if( lastSamples > signalCircularBufferSize )
		return false;
	
	return true;
}

#endif
