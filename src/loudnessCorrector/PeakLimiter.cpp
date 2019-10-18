#include "PeakLimiter.hpp"

#include <math.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <algorithm>

namespace Loudness
{
namespace corrector
{

PeakLimiter::PeakLimiter(const float& attackMilliSec,
                         const float& releaseMilliSec,
                         const float& threshold,
                         const size_t& nbChannels,
                         const size_t& sampleRate)
    : _attackInMilliSec(attackMilliSec)
    , _releaseInMilliSec(releaseMilliSec)
    , _threshold(threshold)
    , _nbChannels(nbChannels)
    , _sampleRate(sampleRate)
    , _fadedGain(1.0f)
    , _smoothState(1.0)
    , _maxValueOfCurrentSection(0)
    , _sectionMaximumsMaxValue(0)
    , _sectionMaximumsMaxValueIndex(0)
    , _positionInSection(0)
    , _sectionIndex(0)
    , _outputSamplesCounter(0)
{
    // compute attack time in samples */
    _attackInSamples = (size_t)(_attackInMilliSec * _sampleRate / 1000);

    if (_attackInSamples < 1) // if attack time is too short
        _attackInSamples = 1;

    // length of buffer sections
    _sectionLength = (size_t) sqrt((float) _attackInSamples + 1);
    // sqrt(_attackInSamples+1) leads to the minimum of the number of maximum operators:
    //     nMaxOp = _sectionLength + (_attackInSamples+1)/_sectionLength

    _nbSections = (_attackInSamples + 1) / _sectionLength;
    if (_nbSections * _sectionLength < (_attackInSamples + 1))
        _nbSections++; /* create a full section for the last samples */

    // init maximum and delay buffers
    _maximums.init(_nbSections * _sectionLength);
    _sectionMaximums.init(_nbSections);
    _delayBuffer.init(_attackInSamples * _nbChannels);
    _sectionMaxIndexList = std::vector<size_t>(_nbSections, 0);

    // init parameters and states
    _temporalGainReductionFactor = (float) pow(0.1, 1.0 / (_attackInSamples + 1));
    _temporalGainRecoveringFactor = (float) pow(0.1, 1.0 / (_releaseInMilliSec * _sampleRate / 1000 + 1));
}

int PeakLimiter::apply(const float* samplesIn, float* samplesOut, const size_t& nSamples) {
    memcpy(samplesOut, samplesIn, nSamples * _nbChannels * sizeof(float));
    return applyInterlaced(samplesOut, nSamples);
}

int PeakLimiter::applyPlanar(const float** samplesIn, float** samplesOut, const size_t& nSamples) {
    const size_t bufferSize = nSamples * _nbChannels;
    float* interlacedSamplesOut = new float[bufferSize];

    size_t count = 0;
    for (size_t i = 0; i < nSamples; i++) {
        for (size_t j = 0; j < _nbChannels; j++) {
            interlacedSamplesOut[count++] = samplesIn[j][i];
        }
    }
    if(count != bufferSize) {
        throw std::runtime_error("Could not initialize output buffer.");
    }

    const int result = applyInterlaced(interlacedSamplesOut, nSamples);
    if(!result) { // if it succeeds
        size_t count = 0;
        for (size_t i = 0; i < nSamples; i++) {
            for (size_t j = 0; j < _nbChannels; j++) {
                samplesOut[j][i] = interlacedSamplesOut[count++];
            }
        }
        if(count != bufferSize) {
            throw std::runtime_error("Could not copy output buffer.");
        }
    }

    return result;
}

int PeakLimiter::applyInterlaced(float* samples, const size_t& nSamples) {
    _outputSamplesCounter = 0;
    const size_t totalNbSamples = nSamples * _nbChannels;
    size_t inputSamplesCounter = 0;
    size_t positionInDelayBuffer = _delayBuffer.getIndex();

    while(_outputSamplesCounter < totalNbSamples) {

        float* frame;
        if(inputSamplesCounter < totalNbSamples) {
            // get input frame while there is some
            frame = &samples[inputSamplesCounter];
        } else {
            // get buffer frame otherwise
            frame = _delayBuffer.getValues();
        }

        // compute peak and gain
        float maximum = getSectionMaximum(frame);
        computeGain(maximum);

        for (size_t c = 0; c < _nbChannels; ++c)
        {
            // get output value from delay buffer
            float outputValue = _delayBuffer.get();
            if(inputSamplesCounter < totalNbSamples) {
                // while we didn't reach the total nb of samples from input, fill the delay buffer
                const float sampleValue = samples[inputSamplesCounter];
                _delayBuffer.set(positionInDelayBuffer, sampleValue);
            }

            if(inputSamplesCounter >= _delayBuffer.getSize()) {
                // if the delay buffer has been completely filled once, we can start writing the output samples
                applyGain(outputValue);
                samples[_outputSamplesCounter] = outputValue;
                _outputSamplesCounter++;
            }

            inputSamplesCounter++;
            positionInDelayBuffer = _delayBuffer.incrementAndGetIndex();
        }
    }

    return 0;
}

void PeakLimiter::applyGain(float& value) {
    // Apply a gain to the previous delay value, and limit the value.
    value *= _smoothState;
    if (value > _threshold) {
        value = _threshold;
    }
    if (value < -_threshold) {
        value = -_threshold;
    }
}

float PeakLimiter::getSectionMaximum(float* frame) {
    // get maximum absolute sample value of all channels that are greater in absoulte value to threshold
    _maximums.set(_threshold);
    for (size_t j = 0; j < _nbChannels; j++) {
        _maximums.set(std::max(_maximums.get(), (float) fabs(frame[j])));
    }

    // search maximum in the current section
    if (_sectionMaxIndexList.at(_sectionMaximums.getIndex()) == _maximums.getIndex()) {
        // if we have just changed the sample containing the old maximum value
        // need to compute the maximum on the whole section
        _maxValueOfCurrentSection = _maximums.get(_sectionIndex);
        for (size_t j = 1; j < _sectionLength; j++) {
            size_t maximumsIndex = _sectionIndex + j;
            if (_maximums.get(maximumsIndex) > _maxValueOfCurrentSection) {
                _maxValueOfCurrentSection = _maximums.get(maximumsIndex);
                _sectionMaxIndexList[_sectionMaximums.getIndex()] = maximumsIndex;
            }
        }
    } else {
        // just need to compare the new value to the current maximum value
        if (_maximums.get() > _maxValueOfCurrentSection) {
            _maxValueOfCurrentSection = _maximums.get();
            _sectionMaxIndexList[_sectionMaximums.getIndex()] = _maximums.getIndex();
        }
    }

    // find maximum of slow (downsampled) max buffer
    float maximum = _sectionMaximumsMaxValue;
    if (_maxValueOfCurrentSection > maximum) {
        maximum = _maxValueOfCurrentSection;
    }

    _maximums.incrementAndGetIndex();
    _positionInSection++;
    return maximum;
}

void PeakLimiter::computeGain(const float& maximum) {
    // if the current section is finished, or end of _maximums is reached,
    // store the maximum of this section and open up a new one
    if ((_positionInSection >= _sectionLength) || (_maximums.getIndex() >= _attackInSamples + 1)) {
        _positionInSection = 0;

        // get maximum of current section and store it into downsampled buffer (and keep as previous max value)
        _sectionMaximums.set(_maxValueOfCurrentSection);
        float previousSectionMaxValue = _sectionMaximums.get();
        bool needToComputeMaxOfMaximumsOverAllSections = false;
         // if we reached the index of the max value of the dowsampled buffer (max of maximums)
        if (_sectionMaximumsMaxValueIndex == _sectionMaximums.getIndex()) {
            needToComputeMaxOfMaximumsOverAllSections = true;
        }
        if (_sectionMaximumsMaxValueIndex == _sectionMaximums.incrementAndGetIndex()) {
            // if we now reached the index of the max value of the dowsampled buffer (max of maximums)
            needToComputeMaxOfMaximumsOverAllSections = true;
        }
         // set _maxValueOfCurrentSection with corresponding value in downsampled buffer
        _maxValueOfCurrentSection = _sectionMaximums.get();
        // zero out the value representing the new section
        _sectionMaximums.set(0.0f);

        // compute the maximum over all the section
        if (needToComputeMaxOfMaximumsOverAllSections) {
            _sectionMaximumsMaxValue = 0;
             // for each section in downsampled buffer, check who's max
            for (size_t j = 0; j < _nbSections; j++) {
                if (_sectionMaximums.get(j) > _sectionMaximumsMaxValue) {
                    _sectionMaximumsMaxValue = _sectionMaximums.get(j);
                    _sectionMaximumsMaxValueIndex = j;
                }
            }
        } else if (previousSectionMaxValue > _sectionMaximumsMaxValue) {
            _sectionMaximumsMaxValue = previousSectionMaxValue;
            _sectionMaximumsMaxValueIndex = _sectionMaximums.getIndex();
        }

        // go to next section in max buffer (in samples)
        _sectionIndex += _sectionLength;
    }

     // if we reached the end of the max buffer (in samples), reset index
    if (_maximums.getIndex() == 0) {
        _sectionIndex = 0;
    }

    // compute needed gain
    float gain = 0;
    if (maximum > _threshold) {
        gain = _threshold / maximum;
    } else {
        gain = 1;
    }

    // avoid overshoot
    if (gain < _smoothState) { // if gain less than something
        _fadedGain = std::min(_fadedGain, (gain - 0.1f * (float) _smoothState) * 1.11111111f);
    } else {
        _fadedGain = gain;
    }

    // smoothing gain
    if (_fadedGain < _smoothState) {
        // attack case
        _smoothState = _temporalGainReductionFactor * (_smoothState - _fadedGain) + _fadedGain;
        // avoid overshoot
        if (gain > _smoothState) {
            _smoothState = gain;
        }
    } else {
        // release case
        _smoothState = _temporalGainRecoveringFactor * (_smoothState - _fadedGain) + _fadedGain;
    }
}

float PeakLimiter::getMaxGainReduction() {
    // get maximum gain reduction of last processed block
    return -20 * (float) log10(_smoothState);
}

}
}
