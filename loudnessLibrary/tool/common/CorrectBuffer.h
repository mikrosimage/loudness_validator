#ifndef CORRECT_BUFFER_H
#define CORRECT_BUFFER_H

#include <vector>
#include <LookAheadLimiter.h>

void correctBuffer( float* data, const size_t samples, const size_t channelsInBuffer, const float gain )
{
	for( size_t c = 0; c < channelsInBuffer; c++ )
	{
		for( size_t i = 0; i < samples; i++ )
		{
			(*data) = (*data) * gain;
			data++;
		}
	}
}

void correctBuffer( std::vector<LookAheadLimiter*>& limiters, float* data, const size_t samples, const size_t channelsInBuffer, const float gain )
{
	float* inData  = data;
	float* outData = data;
	
	float sample;

	for( size_t i = 0; i < samples; i++ )
	{
		for( size_t c = 0; c < channelsInBuffer; c++ )
		{
			sample = (*inData) * gain;
			if( limiters.at(c)->process( sample ) )
			{
				(*outData) = sample;
				outData++;
			}
			inData++;
		}
	}
}

#endif
