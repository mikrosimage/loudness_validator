
#ifndef _LOUDNESS_ANALYSER_HISTOGRAM_HPP_
#define _LOUDNESS_ANALYSER_HISTOGRAM_HPP_

#include <vector>

namespace Loudness
{

class Histogram
{
public:

	Histogram  ( const float minValue, const float maxValue, const float step );
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
	float foundMinPercentageFrom( const float percentile, const float fromValue, const float toValue );

	/**
	 * found value for percentile value, take the upper value near the percentile
	 * @param percentile percentile (in %) of the level to found
	 * @param fromValue process from this value (in dB/LU)
	 * @param toValue process to this value (in dB/LU)
	 * @return return the level (in dB/LU) of the percentile
	 */
	float foundMaxPercentageFrom( const float percentile, const float fromValue, const float toValue );

	std::vector<int> getHistogram();
	
	void applyGain( const float gainInDb );

private:
	int   convertDbToIndex( const float value );
	float convertIndexToDb( const int index );

	const float      _minValue;
	const float      _maxValue;
	const float      _step;

	std::size_t      _size;
	std::size_t      _outOfScope;
	std::size_t      _sumOfElements;
	std::vector<int> _histogram;

};

}
#endif
