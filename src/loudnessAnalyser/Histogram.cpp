
#include "Histogram.hpp"
#include <cmath>
#include "common.hpp"

namespace Loudness
{

Histogram::Histogram( const float minValue, const float maxValue, const float step ) :
	_minValue      ( minValue ),
	_maxValue      ( maxValue ),
	_step          ( step ),
	_size          ( ( (_maxValue - _minValue) / _step ) + 1 ),
	_outOfScope    ( 0 ),
	_sumOfElements ( 0 )
{
	_histogram.resize( _size, 0 );
}

Histogram::~Histogram( )
{

}

void Histogram::reset( )
{
	_outOfScope    = 0;
	_sumOfElements = 0;
	_histogram.clear();
	_histogram.resize( _size, 0 );
	/*
	PLOUD_COUT_VAR( _minValue );
	PLOUD_COUT_VAR( _maxValue );
	PLOUD_COUT_VAR( _step );
	PLOUD_COUT_VAR( _size );
	PLOUD_COUT_VAR( _outOfScope );
	PLOUD_COUT_VAR( _sumOfElements );
	PLOUD_COUT_VAR( _histogram.size() );*/
}

void Histogram::addValue( const float value )
{
	//PLOUD_COUT_VAR(value);
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

float Histogram::integratedValue( const float fromValue, const float toValue )
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


float Histogram::foundMinPercentageFrom( const float percentile, const float fromValue, const float toValue )
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

float Histogram::foundMaxPercentageFrom( const float percentile, const float fromValue, const float toValue )
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

std::vector<int> Histogram::getHistogram()
{
	return _histogram;
}

void Histogram::applyGain( const float gainInDb )
{
	std::vector<int> histogram;
	histogram.resize( _size, 0 );
	for( size_t i=0; i < _histogram.size(); i++ )
	{
		float value = convertIndexToDb( i );
		value += gainInDb;
		if( value > _minValue && value < _maxValue )
		{
			histogram.at( convertDbToIndex( value ) ) = _histogram.at(i);
		}
	}
	for( size_t i=0; i < _histogram.size(); i++ )
	{
		_histogram.at( i ) = histogram.at(i);
	}
}

int Histogram::convertDbToIndex( const float value )
{
	return ( value - _minValue ) * _size / ( _maxValue - _minValue );
}

float Histogram::convertIndexToDb( const int index )
{
	return 1.f * index * ( _maxValue - _minValue ) / ( 1.0 * _size ) + _minValue;
}

}
