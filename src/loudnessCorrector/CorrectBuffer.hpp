#ifndef CORRECT_BUFFER_H
#define CORRECT_BUFFER_H

#include <vector>
#include "LookAheadLimiter.hpp"

namespace Loudness {
namespace tool {

size_t correctBuffer( float* data, const size_t samples, const size_t channelsInBuffer, const float gain )
{
	for( size_t c = 0; c < channelsInBuffer; c++ )
	{
		for( size_t i = 0; i < samples; i++ )
		{
			(*data) = (*data) * gain;
			data++;
		}
	}
	return samples;
}

size_t correctBuffer( std::vector<LookAheadLimiter*>& limiters, float* data, const size_t samples, const size_t channelsInBuffer, const float gain )
{
	float* inData  = data;
	float* outData = data;
	
	float sample;
	size_t count = 0;

	for( size_t i = 0; i < samples; i++ )
	{
		for( size_t c = 0; c < channelsInBuffer; c++ )
		{
			sample = (*inData) * gain;
			if( limiters.at(c)->process( sample ) )
			{
				(*outData) = sample;
				outData++;
				count++;
			}
			inData++;
		}
	}
	return count / channelsInBuffer;
}

size_t getLastData( std::vector<LookAheadLimiter*>& limiters, float* data, const size_t samples, const size_t channelsInBuffer, const float gain )
{
	float* inData  = data;
	float* outData = data;
	
	float sample;
	size_t count = 0;

	for( size_t i = 0; i < samples; i++ )
	{
		for( size_t c = 0; c < channelsInBuffer; c++ )
		{
			sample = (*inData) * gain;
			if( limiters.at(c)->getLastSamples( sample ) )
			{
				(*outData) = sample;
				outData++;
				count++;
			}
			inData++;
		}
	}
	return count / channelsInBuffer;
}

}
}

#endif
