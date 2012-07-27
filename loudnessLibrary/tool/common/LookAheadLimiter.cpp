#include "LookAheadLimiter.h"

/**
  * lookAheadTime [ ms ]
  * inputGain [ dB ]
  * releaseTime [ ms ]
  */
LookAheadLimiter::LookAheadLimiter( const float lookAheadTime, const float sampleRate, const float threshold ) :
	signalCircularBufferSize ( sampleRate * lookAheadTime * 0.0001 ),
	maxCircularBufferSize ( signalCircularBufferSize * 0.5 ),
	signal( signalCircularBufferSize ),
	accMax( signalCircularBufferSize ),
	accShortTimeSum (tag::rolling_window::window_size = maxCircularBufferSize ),
	accShortTimeSum2 (tag::rolling_window::window_size = maxCircularBufferSize ),
	threshold ( threshold ),
	processTime( 0 ),
	lastSamples( 0 )
{
}

LookAheadLimiter::~LookAheadLimiter( )
{
}


bool LookAheadLimiter::process( float& value )
{
	processTime++;
	signal.push_back( value );
	
	accMax( fabs( value ) );
	
	// found max value on signalCiucularBuffer
	float peak = accMax.getMax();
	
	float gainReduction = ( peak > threshold ) ? threshold / peak : 1.0;
	
	accShortTimeSum( gainReduction );
	
	float g = rolling_sum( accShortTimeSum ) / maxCircularBufferSize;
	accShortTimeSum2 ( g );
	
	float finalGain = rolling_sum( accShortTimeSum2 ) / maxCircularBufferSize;
	
	value = signal[0] * finalGain;
	
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
