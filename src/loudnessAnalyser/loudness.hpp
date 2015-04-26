/*
 * loudnessLoudness.h
 *
 *  Created on: Aug 26, 2011
 *      Author: Marc-Antoine ARNAUD
 *              MikrosImage R&D
 */

#ifndef EBU_R128_LOUDNESS_H
#define EBU_R128_LOUDNESS_H

#include <vector>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/rolling_mean.hpp>
#include <boost/accumulators/statistics/rolling_mean.hpp>

#include "histogram.hpp"
#include "loudnessLibrary.hpp"

using namespace boost::accumulators;

namespace Loudness{

enum ELoudnessType{
	eCorrectionLoudness = 0,
	eShortTermLoudness,
	eMomentaryLoudness
};


class Loudness
{
	typedef accumulator_set<float, stats<tag::rolling_sum> > RollSum;
public:
	Loudness( ELoudnessType loudnessType, float absoluteThresholdValue, float relativeThresholdValue,  float minHistrogramValue = -70.0, float maxHistrogramValue = 5.0, float stepHistrogramValue = 0.01 );
	~Loudness();

	/**
	 * reset histogram data
	 */
	void reset();
	
	/**
	 * add fragment value
	 * a fragment is a loudness value on a windows of 50ms
	 */
	void addFragment( const float powerValue );

	/**
	 * process Intregration values on Momentary Loudness type
	 */
	void processIntegrationValues( float& integratedLoudness, float& integratedThreshold );

	void processRangeValues();
	
	float getCorrectionGain( const LoudnessLevels &levels, const bool isShortProgram, const float truePeakValue, bool limiterIsEnable );

	std::vector<int>   getHistogram     () { return _histogram.getHistogram(); }
	std::vector<float> getTemporalValues() { return _temporalValues; }

	float getMinLoudnessValue() const  { return _minLoudness; }
	float getMaxLoudnessValue() const  { return _maxLoudness; }

	float getMinRange()       const    { return _minRange; }
	float getMaxRange()       const    { return _maxRange; }
	float getThresholdRange() const    { return _thresholdRange; }
	float getLoudnessRange()  const    { return _minRange - _maxRange; }
	
private:
	float              _minLoudness;        ///< minimum loudness value found for loudness type
	float              _maxLoudness;        ///< maximum loudness value found for loudness type

	float              _absoluteThreshold;  ///< absolute threshold (defined by norm: tipycaly -70.0 LUFS)
	float              _relativeThreshold;  ///< relative threshold (defined by norm: tipycaly -10.0 LU)
	
	float              _thresholdRange;     ///< relative threshold determined for the current sound

	float              _minRange;           ///<
	float              _maxRange;           ///<

	int                _numberOfFragments;  ///< number of fragments added

	size_t             _counterOfFragments;
	
	ELoudnessType      _loudnessType;       ///< type of loudness compute with this class
	Histogram          _histogram;          ///< the associate histogram of the meseaure

	RollSum            _rollingSum;         ///< rolling sum on fragments

	std::vector<float> _temporalValues;     ///< use to return ShortTerm values
};

}

#endif
