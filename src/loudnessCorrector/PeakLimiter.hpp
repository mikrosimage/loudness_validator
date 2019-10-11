#ifndef _LOUDNESS_CORRECTOR_PEAK_LIMITER_HPP_
#define _LOUDNESS_CORRECTOR_PEAK_LIMITER_HPP_

#include <iostream>
#include <vector>
#include <string.h>

namespace Loudness
{
namespace corrector
{

class SampleBuffer {
public:
    SampleBuffer()
        : _size(0)
        , _index(0)
        , _values(NULL)
    {}

    SampleBuffer(const size_t& length)
        : SampleBuffer()
    {
        init(length);
    }

    void init(const size_t& length) {
        _size = length;
        _index = 0;
        _values = new float[_size]();
        memset(_values, 0, sizeof(float) * _size);
    }

    ~SampleBuffer() { if(_values) { delete[] _values; } }

    size_t getSize() const { return _size; }
    size_t getIndex() const { return _index; }
    size_t incrementAndGetIndex() {
        if (++_index >= _size) {
            _index = 0;
        }
        return _index;
    }

    float* getValues(const size_t& index) { return &_values[index]; }
    float* getValues() { return getValues(_index); }

    void setIndex(const size_t& index) {
        _index = index;
    }

    float get(const size_t& index) {
        return _values[index];
    }

    float get() {
        return get(_index);
    }

    void set(const size_t& index, const float& value) {
        _values[index] = value;
    }

    void set(const float& value) {
        set(_index, value);
    }

private:
    size_t _size;
    size_t _index;
    float* _values;
    std::string _name;
};

class PeakLimiter {

public:

    PeakLimiter(const float& attackMilliSec,  /// attack/lookahead time in milliseconds
                const float& releaseMilliSec, /// release time in milliseconds
                const float& threshold,       /// limiting threshold
                const size_t& nbChannels,     /// number of channels
                const size_t& sampleRate);    /// sampling rate in Hz

    /// Apply limiter to interlaced buffer
    int apply(const float* samplesIn, float* samplesOut, const size_t& nSamplesPerChannel);
    /// Apply limiter to planar buffer
    int applyPlanar(const float** samplesIn, float** samplesOut, const size_t& nSamplesPerChannel);

    /// Get delay (i.e. attack) in samples
    size_t getDelay() const { return _attackInSamples; }

    /// Get attack in milliseconds
    float getAttack() const { return _attackInMilliSec; }

    /// Get sample rate
    size_t getSampleRate() const { return _sampleRate; }

    /// Get release in milliseconds
    float getRelease() const { return _releaseInMilliSec; }

    /// Get (linear) threshold
    float getThreshold() const { return _threshold; }

    /// Get maximum gain reduction in last processed block in dB
    float getMaxGainReduction();

private:
    int applyInterlaced(float* samples, const size_t& nSamplesPerChannel);

    float getSectionMaximum(float* frame);

    void computeGain(const float& maximum);
    void applyGain(float& value);

private:

    float _temporalGainReductionFactor;
    float _temporalGainRecoveringFactor;

    size_t _attackInSamples;
    float _attackInMilliSec;
    float _releaseInMilliSec;

    float _threshold;

    size_t _nbChannels;
    size_t _sampleRate;

    float _fadedGain;
    float _smoothState;

    // Maximum sample values buffer
    SampleBuffer _maximums;
    float _maxValueOfCurrentSection;

    // Maximum values per section buffer
    SampleBuffer _sectionMaximums; // downsampled
    float _sectionMaximumsMaxValue;
    size_t _sectionMaximumsMaxValueIndex;

    // Delay values buffer
    SampleBuffer _delayBuffer;

    size_t _sectionLength;
    size_t _nbSections; // number of sections in max. buffer
    size_t _positionInSection; // index into max. buffer current section (in samples)

    std::vector<size_t> _sectionMaxIndexList;
    size_t _sectionIndex; // position of section in max. buffer (in samples)

    size_t _outputSamplesCounter;
};

}
}

#endif /* _LOUDNESS_CORRECTOR_PEAK_LIMITER_HPP_ */
