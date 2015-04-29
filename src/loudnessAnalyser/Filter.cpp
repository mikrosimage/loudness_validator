
#include "Filter.hpp"

//#define PRINT_FILTERS_COEFFICIENTS

#ifdef PRINT_FILTERS_COEFFICIENTS
#include <iostream>
#include <iomanip>

#define WIDTH_DOUBLE_PRINT 20
#endif

#include <cmath>


namespace Loudness
{

/// Pre-filter constants

#define FC_PRE_FILTER 1681.974450955533
#define VH_PRE_FILTER    1.584864701130855   // quasi 4dB
#define VB_PRE_FILTER    1.258720930232562   // quasi sqrt(Vh)
#define VL_PRE_FILTER    1.0
#define  Q_PRE_FILTER    0.7071752369554196

/// RLB filter constants

#define FC_RLB_FILTER   38.13547087602444
#define VH_RLB_FILTER    1.0                 // optional gain factor
#define VB_RLB_FILTER    0.0
#define VL_RLB_FILTER    0.0
#define  Q_RLB_FILTER    0.5003270373238773


void Filter::initializeFilterCoefficients ( const float& frequencySampling )
{
	double preFilterOmega       = std::tan( M_PI * FC_PRE_FILTER / frequencySampling );
	double preFilterOmega2      = preFilterOmega * preFilterOmega; // omega ^ 2
	double preFilterDenominator = preFilterOmega2 + preFilterOmega / Q_PRE_FILTER + 1;

	_preA0 = 1.0; // normalized value
	_preA1 = 2 * ( preFilterOmega2 - 1 ) / preFilterDenominator;
	_preA2 =     ( preFilterOmega2 - preFilterOmega / Q_PRE_FILTER + 1 ) / preFilterDenominator;

	_preB0 =     ( VL_PRE_FILTER * preFilterOmega2 + VB_PRE_FILTER * preFilterOmega / Q_PRE_FILTER + VH_PRE_FILTER ) / preFilterDenominator;
	_preB1 = 2 * ( VL_PRE_FILTER * preFilterOmega2                                                 - VH_PRE_FILTER ) / preFilterDenominator;
	_preB2 =     ( VL_PRE_FILTER * preFilterOmega2 - VB_PRE_FILTER * preFilterOmega / Q_PRE_FILTER + VH_PRE_FILTER ) / preFilterDenominator;


	double rlbFilterOmega                 = std::tan( M_PI * FC_RLB_FILTER / frequencySampling );
	double rlbFilterOmega2                = rlbFilterOmega * rlbFilterOmega; // omega ^ 2
	double rlbFilterDenominator           = rlbFilterOmega2 + rlbFilterOmega / Q_RLB_FILTER + 1;
	double rlbFilterNormalizedDenominator = VL_RLB_FILTER * rlbFilterOmega2 + VB_RLB_FILTER * rlbFilterOmega / Q_RLB_FILTER + VH_RLB_FILTER;

	_rlbA0 = 1.0; // normalized value
	_rlbA1 = 2 * ( rlbFilterOmega2 - 1 ) / rlbFilterDenominator;
	_rlbA2 =     ( rlbFilterOmega2 - rlbFilterOmega / Q_RLB_FILTER + 1 ) / rlbFilterDenominator;

	_rlbB0 = 1.0; // normalized value
	_rlbB1 = 2 * ( VL_RLB_FILTER * rlbFilterOmega2                                                 - VH_RLB_FILTER ) / rlbFilterNormalizedDenominator;
	_rlbB2 =     ( VL_RLB_FILTER * rlbFilterOmega2 - VB_RLB_FILTER * rlbFilterOmega / Q_RLB_FILTER + VH_RLB_FILTER ) / rlbFilterNormalizedDenominator;

#ifdef PRINT_FILTERS_COEFFICIENTS
	std::cout << "Pre-Filter coefficients" << std::setprecision (16) << std::endl;
	std::cout << "omega = " << preFilterOmega << std::endl;
	std::cout << "omega2 = " << preFilterOmega2 << std::endl;
	std::cout << "a0 = " << std::setw(WIDTH_DOUBLE_PRINT) << _preA0 << "\ta1 = " << std::setw(WIDTH_DOUBLE_PRINT) << _preA1 << "\ta2 = " << std::setw(WIDTH_DOUBLE_PRINT) << _preA2 << std::endl;
	std::cout << "b0 = " << std::setw(WIDTH_DOUBLE_PRINT) << _preB0 << "\tb1 = " << std::setw(WIDTH_DOUBLE_PRINT) << _preB1 << "\tb2 = " << std::setw(WIDTH_DOUBLE_PRINT) << _preB2 << std::endl << std::endl;

	std::cout << "RLB Filter coefficients" << std::endl;
	std::cout << "omega = " << rlbFilterOmega << std::endl;
	std::cout << "a0 = " << std::setw(WIDTH_DOUBLE_PRINT) << _rlbA0 << "\ta1 = " << std::setw(WIDTH_DOUBLE_PRINT) << _rlbA1 << "\ta2 = " << std::setw(WIDTH_DOUBLE_PRINT) << _rlbA2 << std::endl;
	std::cout << "b0 = " << std::setw(WIDTH_DOUBLE_PRINT) << _rlbB0 << "\tb1 = " << std::setw(WIDTH_DOUBLE_PRINT) << _rlbB1 << "\tb2 = " << std::setw(WIDTH_DOUBLE_PRINT) << _rlbB2 << std::endl << std::endl;
#endif
}

float Filter::processSample( const float& sample )
{
	// process sampler in the filter
	double x = sample - _preA1 * _z1 - _preA2 * _z2 ;
	double y = _preB0 * x + _preB1 * _z1 + _preB2 * _z2 - _rlbA1* _z3 - _rlbA2 * _z4;

	double filteredChannel = _rlbB0 * y + _rlbB1 * _z3 + _rlbB2 * _z4;

	// update memory values
	_z2 = _z1;
	_z1 = x;

	_z4 = _z3;
	_z3 = y;

	// return the new sampling value
	return filteredChannel;
}

}
