/*
 * loudnessTruePeakMeter.cpp
 *
 *  Created on: Aug 26, 2011
 *      Author: Marc-Antoine ARNAUD
 *              MikrosImage R&D
 */

#include "loudnessTruePeakMeter.h"

#include <iostream>
#include <cmath>
#include <algorithm>

namespace Loudness{


LoudnessTruePeakMeter::LoudnessTruePeakMeter():
	_z1                  ( 0 ),
	_z2                  ( 0 ),
	_z3                  ( 0 ),
	_z4                  ( 0 ),
	_maxValue            ( 0 ),
	_maxSignal           ( 0 ),
	_frequencySampling   ( 0 ),
	_upsamplingFrequency ( 192000 )
{
}

void LoudnessTruePeakMeter::initialize( const int frequencySampling )
{
	_frequencySampling = frequencySampling;
	_factor = _upsamplingFrequency / _frequencySampling;

	_coefficients.clear();
	_historySamples.clear();

	// process coefficients for i= -FILTER_SIZE/2 to i = FILTER_SIZE/2
	// but impulse response is symetric [ H(-i) = H(i) ]
	for( int i = FILTER_SIZE * 0.5 - 0.5; i > 0; --i )
	{
		_coefficients.push_back( _factor * std::sin( M_PI * i / _factor ) / ( M_PI * i ) );
		_historySamples.push_back( 0.0 );
	}
	// for 0, the sinc is equel to 1
	_coefficients.push_back( 1.0 );
	_historySamples.push_back( 0.0 );

	// process positive index
	for( int i = 1.0; i < FILTER_SIZE * 0.5; ++i )
	{
		_coefficients.push_back( _factor * std::sin( M_PI * i / _factor ) / ( M_PI * i ) );
		_historySamples.push_back( 0.0 );
	}

}

float LoudnessTruePeakMeter::processSample( const double& sample )
{
	_historySamples.erase( _historySamples.begin() );
	_historySamples.push_back( sample );

	for( int interSampleIdx = 0; interSampleIdx < _factor; interSampleIdx++ )
	{
		double tmpValue = 0.0;

		int historySampleIndex = 0;
		for( int iter = 0; iter + interSampleIdx < FILTER_SIZE; iter += _factor, historySampleIndex++ )
		{
			tmpValue += _coefficients.at( interSampleIdx + iter ) * _historySamples.at( _historySamples.size() - historySampleIndex - 1 );
			//std::cout << "detail :  " << sample << "\t" << _coefficients.at( interSampleIdx + iter ) << "\t" << _historySamples.at( _historySamples.size() - historySampleIndex - 1 ) <<"\t";
			//std::cout << "+= " << _coefficients.at( interSampleIdx + iter ) * _historySamples.at( _historySamples.size() - historySampleIndex - 1 ) << std::endl;
		}
		//std::cout << "\t\t\t\t\t\t" << interSampleIdx << " \t " << tmpValue << std::endl;
		_maxValue = std::max ( _maxValue, std::abs( tmpValue ) );
	}
	_maxValue = std::max ( _maxValue, std::abs( sample ) );
	_maxSignal = std::max ( _maxSignal, std::abs( sample ) );

	return _maxValue;
}

}
