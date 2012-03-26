/*
 * loudnessHistogram.cpp
 *
 *  Created on: Aug 26, 2011
 *      Author: Marc-Antoine ARNAUD
 *              MikrosImage R&D
 */

#include "loudnessHistogram.h"
#include <cmath>
#include <cstring>
#include <iostream>

namespace Loudness
{

LoudnessHistogram::LoudnessHistogram( float minValue, float maxValue, float step ) :
	_minValue      ( minValue ),
	_maxValue      ( maxValue ),
	_step          ( step ),
	_size          ( ( (_maxValue - _minValue) / _step ) + 1 ),
	_outOfScope    ( 0 ),
	_sumOfElements ( 0 )
{
	_histogram.resize( _size, 0 );
}

LoudnessHistogram::~LoudnessHistogram( )
{

}

void LoudnessHistogram::addValue( const float value )
{
	if( !( value > _minValue && value < _maxValue ) )
	{
		_outOfScope++;
		return;
	}
	// find associate index to the current value
	int index = std::floor( convertDbToIndex( value ) );

	_histogram.at( index )++;
	_sumOfElements++;
}

float LoudnessHistogram::integratedValue( const float fromValue, const float toValue )
{
	int fromIndex = std::max( 0,                      convertDbToIndex( fromValue ) );
	int toIndex   = std::min( (int)_histogram.size(), convertDbToIndex( toValue   ) );

	float sum = 0.0;
	int   countSegment = 0.0;

	for( int i = fromIndex; i< toIndex; i++ )
	{
		int weight = _histogram.at( i );

		float power = std::pow ( 10.f, convertIndexToDb( i ) / 10.f ) ;

		sum += weight * power;
		countSegment += weight;
	}

	return 10.0 * std::log10 ( sum / countSegment );
}


float LoudnessHistogram::foundMinPercentageFrom( float percentile, float fromValue, float toValue )
{
	int fromIndex = std::max( 0,                      convertDbToIndex( fromValue ) );
	int toIndex   = std::min( (int)_histogram.size(), convertDbToIndex( toValue   ) );

	int elementsFromThreashold = 0;
	for( int i = fromIndex; i < toIndex ; i++ )
	{
		elementsFromThreashold += _histogram.at( i );
	}


	int segmentToAssociatePercentile = 0.01f * percentile * elementsFromThreashold;

	int foundSegment = 0.0;
	int i = fromIndex;
	int correctIndex = 0;
	for( ; i < toIndex ; i++ )
	{
		foundSegment += _histogram.at( i );
		if( foundSegment > segmentToAssociatePercentile )
		{
			break;
		}
		else
		{
			correctIndex = i;
		}
	}

	return convertIndexToDb( correctIndex );
}

float LoudnessHistogram::foundMaxPercentageFrom( float percentile, float fromValue, float toValue )
{
	int fromIndex = std::max( 0,                      convertDbToIndex( fromValue ) );
	int toIndex   = std::min( (int)_histogram.size(), convertDbToIndex( toValue   ) );

	int elementsFromThreashold = 0;
	for( int i = fromIndex; i < toIndex ; i++ )
	{
		elementsFromThreashold += _histogram.at( i );
	}


	int segmentToAssociatePercentile = 0.01f * percentile * elementsFromThreashold;

	int foundSegment = 0.0;
	int i = fromIndex;
	for( ; ( i < toIndex ) && ( foundSegment < segmentToAssociatePercentile ) ; i++ )
	{
		foundSegment += _histogram.at( i );
	}

	return convertIndexToDb( i );
}

std::vector<int> LoudnessHistogram::getHistogram()
{
	return _histogram;
}

int LoudnessHistogram::convertDbToIndex( float value )
{
	return ( value - _minValue ) * _size / ( _maxValue - _minValue );
}

float LoudnessHistogram::convertIndexToDb( int index )
{
	return 1.f * index * ( _maxValue - _minValue ) / ( 1.0 * _size ) + _minValue;
}

}
