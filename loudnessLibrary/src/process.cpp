/*
 * loudnessProcess.cpp
 *
 *  Created on: Aug 26, 2011
 *      Author: Marc-Antoine ARNAUD
 *              MikrosImage R&D
 */

#include "process.h"
#include "common.h"

namespace Loudness{

float Process::_channelGain [MAX_CHANNELS] = { 1.0f, 1.0f, 1.0f, 1.41f, 1.41f };

Process::Process( float absoluteThresholdValue, float relativeThresholdValue ) :
	s_measureLoudness   ( eCorrectionLoudness, absoluteThresholdValue, relativeThresholdValue, -200, 20, 0.01 ),
	s_shortTermLoudness ( eShortTermLoudness, absoluteThresholdValue, relativeThresholdValue ),
	s_momentaryLoudness ( eMomentaryLoudness, absoluteThresholdValue, relativeThresholdValue )
{
}

Process::~Process()
{
}

void Process::init ( const int numberOfChannels, const float frequencySampling )
{
	_numberOfChannels  = numberOfChannels;
	_frequencySampling = frequencySampling;
	_fragmentSize      = (int) frequencySampling / 20;

	for( int i=0; i < _numberOfChannels; i++)
	{
		_filters[i].initializeFilterCoefficients ( _frequencySampling );
		_truePeakMeter[i].initialize             ( _frequencySampling );
	}

	reset();
}

void Process::reset ( )
{
	_fragmentCount          = _fragmentSize;
	_fragmentPower          = 1e-30f;
	_writeIndex             = 0;
	_truePeakValue          = 0;

	_countTruePeakPeriod  = 0;

	_vectorOfTruePeakValue.clear();
	
	for (int c = 0; c < _numberOfChannels; c++)
		_filters [c].reset ();
	
	s_measureLoudness.reset();
	s_shortTermLoudness.reset();
	s_momentaryLoudness.reset();
}

void Process::setUpsamplingFrequencyForTruePeak ( const size_t frequency )
{
	for( int i=0; i< _numberOfChannels; i++)
	{
		_truePeakMeter[i].setUpsamplingFrequencyInHz( frequency );
	}
}

void Process::process ( int numberOfFrames, float *inputData [] )
{
	int i, samplesForOneBloc;

	for (i = 0; i < _numberOfChannels; i++)
		_inputPointerData [i] = inputData [i];

	while ( numberOfFrames )
	{
		samplesForOneBloc = (_fragmentCount < numberOfFrames) ? _fragmentCount : numberOfFrames;

		_fragmentPower += detectProcess ( samplesForOneBloc, _tmpTruePeakValue );

		//PLOUD_COUT_VAR( _fragmentPower );
		
		_fragmentCount -= samplesForOneBloc;
		if ( _fragmentCount == 0 )
		{
			_countTruePeakPeriod++;
			if( _countTruePeakPeriod == 10 )
			{
				_vectorOfTruePeakValue.push_back( 20.0 * std::log10( _tmpTruePeakValue ) );
				_truePeakValue = std::max( _truePeakValue, _tmpTruePeakValue );
				_tmpTruePeakValue = 0.0;
				_countTruePeakPeriod = 0;
				for (int channel = 0; channel < _numberOfChannels; channel++)
					_truePeakMeter[channel].resetMaxValue();
			}

			s_momentaryLoudness.addFragment( _fragmentPower / _fragmentSize );
			s_shortTermLoudness.addFragment( _fragmentPower / _fragmentSize );
			s_measureLoudness  .addFragment( _fragmentPower / _fragmentSize );

			_fragmentCount = _fragmentSize;
			_fragmentPower = 1e-30f;
			_writeIndex &= 63;
		}

		for ( i = 0; i < _numberOfChannels; i++ )
		  _inputPointerData [i] += samplesForOneBloc;

		numberOfFrames -= samplesForOneBloc;
	}
}

float Process::detectProcess ( const int numberOfFrames, float& truePeakValue )
{
	// process on a bloc of 50ms, compute the loudness value, and the found the TruePeak on the buffer
	int   channel, frame;
	float sumOfChannelPower, sumOfWeightedPowerChannels;
	float filteredChannel;
	float *sample;

	truePeakValue = 0.0; // reset the TruePeak to be sure to take the max value after.

	sumOfWeightedPowerChannels = 0;
	for (channel = 0; channel < _numberOfChannels; channel++)
	{
		sample = _inputPointerData [channel];
		sumOfChannelPower = 0;

		for ( frame = 0; frame < numberOfFrames; frame++ )
		{
			// process filtering value
			filteredChannel = _filters      [channel].processSample( sample[frame] );

			// process the true peak value (with inter-samples)
			truePeakValue  = std::max( truePeakValue, _truePeakMeter[channel].processSample( sample[frame] ) );

			// process power value of the filtered value
			sumOfChannelPower += filteredChannel * filteredChannel;
		}

		// weight each channel (1.41 for surround channels, 1 for others, 2 for mono channel)
		if ( _numberOfChannels == 1 )
			sumOfWeightedPowerChannels = 2 * sumOfChannelPower;
		else
			sumOfWeightedPowerChannels += _channelGain [channel] * sumOfChannelPower;

	}
	return sumOfWeightedPowerChannels;
}

}
