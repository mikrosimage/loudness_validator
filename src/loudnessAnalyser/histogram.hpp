/*
 * loudnessHistogram.h
 *
 *  Created on: Aug 26, 2011
 *      Author: Marc-Antoine ARNAUD
 *              MikrosImage R&D
 */

#ifndef EBU_R128_HISTOGRAM_H
#define EBU_R128_HISTOGRAM_H

#include <vector>

namespace Loudness
{

class Histogram
{
public:

	Histogram  ( float minValue, float maxValue, float step );
	~Histogram ( );

	void reset();
	
	void addValue( const float value );

	float integratedValue( const float fromValue, const float toValue );

	/**
	 * found value for percentile value, take the lowest value near the percentile
	 * @param percentile percentile (in %) of the level to found
	 * @param fromValue process from this value (in dB/LU)
	 * @param toValue process to this value (in dB/LU)
	 * @return return the level (in dB/LU) of the percentile
	 */
	float foundMinPercentageFrom( float percentile, float fromValue, float toValue );

	/**
	 * found value for percentile value, take the upper value near the percentile
	 * @param percentile percentile (in %) of the level to found
	 * @param fromValue process from this value (in dB/LU)
	 * @param toValue process to this value (in dB/LU)
	 * @return return the level (in dB/LU) of the percentile
	 */
	float foundMaxPercentageFrom( float percentile, float fromValue, float toValue );

	std::vector<int> getHistogram();
	
	void applyGain( float gainInDb );

private:
	int   convertDbToIndex( float value );
	float convertIndexToDb( int index );

	const float      _minValue;
	const float      _maxValue;
	const float      _step;

	std::size_t      _size;
	std::size_t      _outOfScope;
	std::size_t      _sumOfElements;
	std::vector<int> _histogram;

};

}
#endif // EBU_R128_HISTOGRAM_H
