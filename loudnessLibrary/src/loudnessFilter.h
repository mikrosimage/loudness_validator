/*
 * loudnessFilter.h
 *
 *  Created on: Aug 26, 2011
 *      Author: Marc-Antoine ARNAUD
 *              MikrosImage R&D
 */

#ifndef LOUDNESS_FILTER_H
#define LOUDNESS_FILTER_H

namespace Loudness{

class LoudnessFilter
{
public:
	void reset ( ) { _z1 = _z2 = _z3 = _z4 = 0; }

	void initializeFilterCoefficients ( const float& frequencySampling );

	float processSample( const float& sample );

private:
	float _z1, _z2, _z3, _z4;

	// Pre-filters coefficients, depending on sampling frequency
	double  _preA0, _preA1, _preA2;
	double  _preB0, _preB1, _preB2;
	// RLB-filters coefficients, depending on sampling frequency
	double  _rlbA0, _rlbA1, _rlbA2;
	double  _rlbB0, _rlbB1, _rlbB2;
};


}

#endif // EBU_R128_FILTER_H
