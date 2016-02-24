#ifndef _LOUDNESS_ANALYSER_PROCESS_HPP_
#define _LOUDNESS_ANALYSER_PROCESS_HPP_

#include <loudnessCommon/common.hpp>
#include "Loudness.hpp"
#include "Filter.hpp"
#include "Histogram.hpp"
#include "TruePeakMeter.hpp"

#include <vector>
#include <cmath>

namespace Loudness
{
namespace analyser
{

struct LoudnessLevels;

class Process
{
public:
    Process(float absoluteThresholdValue, float relativeThresholdValue);
    ~Process();

    void init(const int numberOfChannels, const float frequencySampling, const bool enableOptimization = true);
    void reset();
    void process(size_t nbSamples, float* inputData[]);

    void setUpsamplingFrequencyForTruePeak(const size_t frequency);

    float getMaxLoudnessMomentary() const { return s_momentaryLoudness.getMaxLoudnessValue(); }

    float getMaxLoudnessShortTerm() const { return s_shortTermLoudness.getMaxLoudnessValue(); }
    float getMinLoudnessShortTerm() const { return s_shortTermLoudness.getMinLoudnessValue(); }

    float getIntegrated()
    {
        float _integratedLoudness, _integratedThreshold;
        s_momentaryLoudness.processIntegrationValues(_integratedLoudness, _integratedThreshold);
        return _integratedLoudness;
    }

    float getIntegratedThreshold()
    {
        float _integratedLoudness, _integratedThreshold;
        s_momentaryLoudness.processIntegrationValues(_integratedLoudness, _integratedThreshold);
        return _integratedThreshold;
    }

    float getRangeMin()
    {
        s_shortTermLoudness.processRangeValues();
        return s_shortTermLoudness.getMinRange();
    }

    float getRangeMax()
    {
        s_shortTermLoudness.processRangeValues();
        return s_shortTermLoudness.getMaxRange();
    }

    float getRangeThreshold()
    {
        s_shortTermLoudness.processRangeValues();
        return s_shortTermLoudness.getThresholdRange();
    }

    float getTruePeakValue() const { return _truePeakValue; }
    float getTruePeakValueInDb() const { return 20.0 * std::log10(_truePeakValue); }

    const std::vector<float> getShortTermValues() { return s_shortTermLoudness.getTemporalValues(); }

    // values in dB
    const std::vector<float> getTruePeakValues() { return _vectorOfTruePeakValue; }

    const std::vector<int> getHistogramShortTerm() { return s_shortTermLoudness.getHistogram(); }

    float getCorrectionGain(const LoudnessLevels& levels, const bool isShortProgram, const bool limiterIsEnable)
    {
        return s_measureLoudness.getCorrectionGain(levels, isShortProgram, getTruePeakValueInDb(), limiterIsEnable);
    }

private:
    // process on a bloc of 50ms, compute the loudness value, and found the TruePeak on the buffer
    float detectProcess(const size_t nbSamples, float& truePeakValue);

    size_t _numberOfChannels; // Number of channels, 2 or 5.
    float _frequencySampling; // Sample rate.
    size_t _fragmentSize;     // Fragments size, 1/20 second.
    size_t _fragmentCount;    // Number of samples remaining in current fragment.
    float _fragmentPower;     // Power accumulated for current fragment.
    int _writeIndex;          // Write index into _frpwr

    float _tmpTruePeakValue;                   // TruePeak on window size (for example on 500ms )
    int _countTruePeakPeriod;                  // TruePeak counter
    std::vector<float> _vectorOfTruePeakValue; // temporal TruePeak on window size
    float _truePeakValue;                      // TruePeak on Program

    float* _inputPointerData[MAX_CHANNELS];
    // pre-filters
    Filter _filters[MAX_CHANNELS];

    // TruePeakMeter
    TruePeakMeter _truePeakMeter[MAX_CHANNELS];

    Loudness s_measureLoudness;

    Loudness s_shortTermLoudness;
    Loudness s_momentaryLoudness;

    // Default channel gains.
    static float _channelGain[MAX_CHANNELS];
};
}
}

#endif
