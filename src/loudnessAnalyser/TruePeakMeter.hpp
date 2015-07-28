#ifndef _LOUDNESS_ANALYSER_TRUE_PEAK_METER_HPP_
#define _LOUDNESS_ANALYSER_TRUE_PEAK_METER_HPP_

#include <vector>

namespace Loudness{

class TruePeakMeter
{
public:
	TruePeakMeter( bool enableOptimization = false );

	void initialize( const int frequencySampling );

	void enableOptimization( bool enable )
	{
		_enableOptimization = enable;
	}

	void reset()
	{
		_maxValue            = 0;
		_frequencySampling   = 0;
		_upsamplingFrequency = 192000.0;
	}

	void resetMaxValue()
	{
		_maxValue            = 0;
	}

	float processSample( const double& sample );

	float getTruePeakValue()
	{
		//std::cout << "max signal = " <<  _maxSignal << " = " << 20.0 * std::log10( _maxSignal )  << "dB\t max true peak " << _maxValue << " = " << 20.0 * std::log10( _maxValue ) << "dB" << std::endl;
		return _maxValue;
	}

	void setUpsamplingFrequencyInHz ( int frequency )
	{
		_upsamplingFrequency = frequency;
	}

private:
	static const int FILTER_SIZE = 125;

private:
	std::vector<float>  _historySamples;
	std::vector<float>  _coefficients;
	
	std::vector<float>  _orderedCoefficientsScale4[4];
	std::vector<float>  _orderedCoefficientsScale8[8];

	double              _maxValue;            /// maximum value of the upsampled signal
	double              _maxSignal;           /// maximum value of the signal
	double              _frequencySampling;   /// input frequency sampling
	double              _upsamplingFrequency; /// output frequency sampling
	double              _factor;              /// upsampling scale factor

	bool                _enableOptimization;
};

}

#endif
