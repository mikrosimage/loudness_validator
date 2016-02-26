#include "Loudness.hpp"
#include <numeric>
#include <cmath>

//#define LOUD_CONSTANT -0.6976f
#define LOUD_CONSTANT -0.691f

namespace Loudness
{
namespace analyser
{

Loudness::Loudness(ELoudnessType loudnessType, float absoluteThresholdValue, float relativeThresholdValue,
                   float minHistrogramValue, float maxHistrogramValue, float stepHistrogramValue)
    : _minLoudness(200.f)
    , _maxLoudness(-200.f)
    , _absoluteThreshold(absoluteThresholdValue)
    , _relativeThreshold(relativeThresholdValue)
    , _numberOfFragments(0)
    , _loudnessType(loudnessType)
    , _histogram(minHistrogramValue, maxHistrogramValue, stepHistrogramValue)
    , _rollingSum(tag::rolling_window::window_size = (_loudnessType == eCorrectionLoudness)
                                                         ? 8
                                                         : (_loudnessType == eShortTermLoudness) ? 60 :
                                                                                                 /*eMomentaryLoudness*/ 8)
{
}

Loudness::~Loudness()
{
}

void Loudness::reset()
{
    _minLoudness = 200.f;
    _maxLoudness = -200.f;
    _numberOfFragments = 0;
    _temporalValues.clear();
    //_rollingSum =  RollSum();
    _histogram.reset();
}

void Loudness::addFragment(const float powerValue)
{
    float sum;
    float currentLoudness;
    _rollingSum(powerValue);

    sum = ::boost::accumulators::rolling_sum(_rollingSum);

    currentLoudness = LOUD_CONSTANT + 10.0 * std::log10(sum / ((_loudnessType == eShortTermLoudness) ? 60.0 : 8.0));

    _numberOfFragments++;

    switch(_loudnessType)
    {
        case eCorrectionLoudness:
        {
            if(_numberOfFragments != 2)
                break;
            _histogram.addValue(currentLoudness);
            // std::cout << currentLoudness << std::endl;
            _numberOfFragments = 0;
            break;
        }
        case eMomentaryLoudness:
        {
            if(_numberOfFragments != 2)
                break;
            _histogram.addValue(currentLoudness);
            _numberOfFragments = 0;
            break;
        }
        case eShortTermLoudness:
        {
            if(_numberOfFragments != 10)
                break;
            _histogram.addValue(currentLoudness);
            _temporalValues.push_back(currentLoudness);
            _numberOfFragments = 0;
            break;
        }
    }

    _maxLoudness = std::max(_maxLoudness, currentLoudness);
    _minLoudness = std::min(_minLoudness, currentLoudness);
}

void Loudness::processIntegrationValues(float& integratedLoudness, float& integratedThreshold)
{
    if(_loudnessType == eShortTermLoudness)
        // we process only with momentary histogram.
        return;

    integratedThreshold = _histogram.integratedValue(_absoluteThreshold, 5.0) + _relativeThreshold;

    integratedLoudness = _histogram.integratedValue(integratedThreshold, 5.0);
}

void Loudness::processRangeValues()
{
    if(_loudnessType != eShortTermLoudness)
        // we process only with short-term histogram.
        return;

    float relativeThresholdForLRA = -20.f;

    _thresholdRange = _histogram.integratedValue(_absoluteThreshold, 5.0) + relativeThresholdForLRA;

    // found between threshold and 5.0 LU the 10 percentiles of the distribution
    _minRange = _histogram.foundMinPercentageFrom(10.0, _thresholdRange, 5.0);

    // found between threshold and 5.0 LU the 10 percentiles of the distribution
    _maxRange = _histogram.foundMaxPercentageFrom(95.0, _thresholdRange, 5.0);
}

float Loudness::getCorrectionGain(const LoudnessLevels& levels, const bool isShortProgram, const float truePeakValue,
                                  const bool limiterIsEnable)
{
    float integratedThreshold;
    float integratedLoudness;
    float correctionGain;
    float idealCorrectionGain;
    float minCorrectionGain;
    // float maxCorrectionGain;
    float targetLevel =
        isShortProgram ? levels.programLoudnessShortProgramTargetLevel : levels.programLoudnessLongProgramTargetLevel;
    float minTargetLevel =
        isShortProgram ? levels.programLoudnessShortProgramTargetMaxLevel : levels.programLoudnessLongProgramTargetMaxLevel;
    // float maxTargetLevel = isShortProgram ? levels.programLoudnessShortProgramTargetMinLevel :
    // levels.programLoudnessLongProgramTargetMinLevel;

    integratedThreshold = _histogram.integratedValue(_absoluteThreshold, 5.0) + _relativeThreshold;
    integratedLoudness = _histogram.integratedValue(integratedThreshold, 5.0);

    idealCorrectionGain = targetLevel - integratedLoudness;
    minCorrectionGain = minTargetLevel - integratedLoudness;
    // maxCorrectionGain   = maxTargetLevel - integratedLoudness;

    if(limiterIsEnable)
        return std::pow(10, (idealCorrectionGain) / 20);

    float maxTruePeakGainCorrection = levels.truePeakTargetLevel - truePeakValue;

    float diff = maxTruePeakGainCorrection - idealCorrectionGain;

    /*
    std::cout << "| min gain correction:       " << minCorrectionGain << " dB = " << std::pow ( 10, ( minCorrectionGain ) /
    20 ) << std::endl;
    std::cout << "| ideal gain correction:     " << idealCorrectionGain << " dB = " << std::pow ( 10, ( idealCorrectionGain )
    / 20 ) << std::endl;
    std::cout << "| True Peak gain correction: " << maxTruePeakGainCorrection << " dB = " << std::pow ( 10, (
    maxTruePeakGainCorrection ) / 20 ) << std::endl;

    std::cout << "| diff:                      " << diff << std::endl;*/

    if(diff > 0) // the correction gain is valid to correct the program
    {
        correctionGain = idealCorrectionGain;
    }
    else // it can't be decided here
    {
        if(minCorrectionGain > maxTruePeakGainCorrection)
        {
            correctionGain = maxTruePeakGainCorrection;
        }
        else
        {
            correctionGain = LOUDNESS_NAN;
        }
    }
    /*
    _histogram.applyGain( correctionGain );
    integratedThreshold = _histogram.integratedValue( _absoluteThreshold, 5.0 ) + _relativeThreshold;
    integratedLoudness  = _histogram.integratedValue( integratedThreshold, 5.0 );*/

    // std::cout << "| => applying correction: " << correctionGain << " dB = " << std::pow ( 10, ( correctionGain ) / 20 ) <<
    // " | newest Program Loudness:" << integratedLoudness << " LUFS" << std::endl;
    return std::pow(10, (correctionGain) / 20);
}
}
}
