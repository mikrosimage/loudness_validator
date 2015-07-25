
#include "TruePeakMeter.hpp"
#include "common.hpp"

#include <cmath>
#include <algorithm>

#ifdef USE_SSE2
#include <emmintrin.h>
#endif

namespace Loudness{


TruePeakMeter::TruePeakMeter()
	: _maxValue            ( 0 )
	, _maxSignal           ( 0 )
	, _frequencySampling   ( 0 )
	, _upsamplingFrequency ( 192000 )
{
}

void TruePeakMeter::initialize( const int frequencySampling )
{
	_frequencySampling = frequencySampling;
	_factor = _upsamplingFrequency / _frequencySampling;

	_coefficients.clear();
	_historySamples.clear();
	for( int i = 0; i < 4; i++ )
	{
		_orderedCoefficientsScale4[i].clear();
		_orderedCoefficientsScale8[i].clear();
		_orderedCoefficientsScale8[2*i+1].clear();
	}
	
	// process coefficients for i= -FILTER_SIZE/2 to i = FILTER_SIZE/2
	// but impulse response is symetric [ H(-i) = H(i) ]
	for( int i = FILTER_SIZE * 0.5 - 0.5; i > 0; --i )
	{
		_coefficients.push_back( _factor * std::sin( M_PI * i / _factor ) / ( M_PI * i ) );
	}
	// for 0, the sinc is equel to 1
	_coefficients.push_back( 1.0 );

	// process positive index
	for( int i = 1.0; i < FILTER_SIZE * 0.5; ++i )
	{
		_coefficients.push_back( _factor * std::sin( M_PI * i / _factor ) / ( M_PI * i ) );
	}
	
	for( size_t i = 0; i < FILTER_SIZE / _factor; i++ )
		_historySamples.push_back( 0.0 );

	// fill to process easly on SIMD (4 datas on the same time)
	size_t fill = 4 - (int)FILTER_SIZE % 4;
	for( size_t i = 0; i < fill; ++i )
	{
		_coefficients.push_back( 0.0 );
		_historySamples.push_back( 0.0 );
	}
	
	for( size_t i = 0; i < _coefficients.size(); i += 4 )
	{
		for( size_t n = 0; n < 4; n++ )
		{
			_orderedCoefficientsScale4[n].push_back( _coefficients.at( i + n ) );
		}
	}
	
	for( size_t i = 0; i < _coefficients.size(); i += 8 )
	{
		for( size_t n = 0; n < 8; n++ )
		{
			_orderedCoefficientsScale8[n].push_back( _coefficients.at( i + n ) );
		}
	}
}

float TruePeakMeter::processSample( const double& sample )
{
	_historySamples.erase( _historySamples.begin() );
	_historySamples.push_back( sample );

	for( int interSampleIdx = 0; interSampleIdx < _factor; interSampleIdx++ )
	{
		double tmpValue = 0.0;
#ifdef USE_SSE2
		
		if( _factor == 4.0 )
		{
			__m128 sum = _mm_set1_ps ( 0.0 );
			const size_t maxIter = _orderedCoefficientsScale4[interSampleIdx].size();
			const float* coefsPtr = &_orderedCoefficientsScale4[interSampleIdx][0];
			const float* histPtr = &_historySamples[0];
			for( size_t iter = 0; iter < maxIter; iter += 4 )
			{
				__m128 input = _mm_loadu_ps( (histPtr+iter) );
				__m128 coefs = _mm_loadu_ps( (coefsPtr+iter) );
			
				sum = _mm_add_ps( _mm_mul_ps( input, coefs ), sum );
			}
			float tmp[4];
			_mm_storeu_ps( tmp, sum );
			tmpValue = tmp[0] + tmp[1] + tmp[2] +tmp[3];
		}
		else
		{
			int historySampleIndex = 0;
			for( int iter = 0; iter + interSampleIdx < FILTER_SIZE; iter += _factor, historySampleIndex++ )
			{
				tmpValue += _coefficients.at( interSampleIdx + iter ) * _historySamples.at( _historySamples.size() - historySampleIndex - 1 );
			}
		}
#else
		int historySampleIndex = 0;
		for( int iter = 0; iter + interSampleIdx < FILTER_SIZE; iter += _factor, historySampleIndex++ )
		{
			tmpValue += _coefficients.at( interSampleIdx + iter ) * _historySamples.at( _historySamples.size() - historySampleIndex - 1 );
			//std::cout << "detail :  " << interSampleIdx + iter << "\t" << _historySamples.size() - historySampleIndex - 1 << std::endl;
			//std::cout << "detail :  " << sample << "\t" << _coefficients.at( interSampleIdx + iter ) << "\t" << _historySamples.at( _historySamples.size() - historySampleIndex - 1 ) <<"\t";
			//std::cout << "+= " << _coefficients.at( interSampleIdx + iter ) * _historySamples.at( _historySamples.size() - historySampleIndex - 1 ) << std::endl;
		}
#endif
		//std::cout << "\t\t\t\t\t\t" << interSampleIdx << " \t " << tmpValue << std::endl;
		_maxValue = std::max ( _maxValue, std::abs( tmpValue ) );
	}
	_maxValue = std::max ( _maxValue, std::abs( sample ) );
	_maxSignal = std::max ( _maxSignal, std::abs( sample ) );

	return _maxValue;
}

}
