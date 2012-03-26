#include "loudnessLoudness.h"
#include <numeric>
#include <cmath>
#include <iostream>

namespace Loudness{


LoudnessLoudness::LoudnessLoudness( ELoudnessType loudnessType, float absoluteThresholdValue, float relativeThresholdValue ) :
	_minLoudness       (  200.f ),
	_maxLoudness       ( -200.f ),
	_absoluteThreshold ( absoluteThresholdValue ),
	_relativeThreshold ( relativeThresholdValue ),
	_numberOfFragments ( 0 ),
	_loudnessType      ( loudnessType ),
	_histogram         ( -70.0, 5.0, 0.01 ),
	_rollingSum        ( tag::rolling_window::window_size =
			     (loudnessType == eIntegratedLoudness) ? 1 :
			     (loudnessType == eShortTermLoudness ) ? 60 :
			     /*eMomentaryLoudness*/  8 )
{
}

LoudnessLoudness::~LoudnessLoudness()
{

}

void LoudnessLoudness::addFragment ( const float powerValue )
{
	float sum;
	float currentLoudness;
	_rollingSum( powerValue );

	sum = ::boost::accumulators::rolling_sum( _rollingSum );

	currentLoudness = -0.6976f + 10.0 * std::log10 ( sum / ( ( _loudnessType == eShortTermLoudness ) ? 60.0 : 8.0 )   );

	_numberOfFragments++;

	switch( _loudnessType )
	{
		case eMomentaryLoudness:
			if( _numberOfFragments != 2 )
				break;
			_histogram.addValue( currentLoudness );
			_numberOfFragments = 0;
			break;
		case eShortTermLoudness:
			if( _numberOfFragments != 10 )
				break;
			_histogram.addValue( currentLoudness );
			_temporalValues.push_back( currentLoudness );
			_numberOfFragments = 0;
			break;
		default:
			break;
	}

	_maxLoudness = std::max( _maxLoudness, currentLoudness );
	_minLoudness = std::min( _minLoudness, currentLoudness );
}


void LoudnessLoudness::processIntegrationValues( float& integratedLoudness, float& integratedThreshold )
{
	if ( _loudnessType != eMomentaryLoudness )
		// we process only with momentary histogram.
		return;

	integratedThreshold = _histogram.integratedValue( _absoluteThreshold, 5.0 ) + _relativeThreshold;

	integratedLoudness = _histogram.integratedValue( integratedThreshold, 5.0 );
}

void LoudnessLoudness::processRangeValues()
{
	if ( _loudnessType != eShortTermLoudness )
		// we process only with short-term histogram.
		return;

	float relativeThresholdForLRA = -20.f;

	_thresholdRange = _histogram.integratedValue( _absoluteThreshold, 5.0 ) + relativeThresholdForLRA;

	// found between threshold and 5.0 LU the 10 percentiles of the distribution
	_minRange = _histogram.foundMinPercentageFrom( 10.0, _thresholdRange, 5.0 );

	// found between threshold and 5.0 LU the 10 percentiles of the distribution
	_maxRange = _histogram.foundMaxPercentageFrom( 95.0, _thresholdRange, 5.0 );
}

}
