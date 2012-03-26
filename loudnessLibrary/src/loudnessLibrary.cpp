/*
 * loudnessLibrary.cpp
 *
 *  Created on: Aug 26, 2011
 *      Author: Marc-Antoine ARNAUD
 *              MikrosImage R&D
 */

#include "loudnessLibrary.h"
#include "loudnessProcess.h"

#include <iostream>
#include <iomanip>
#include <limits>
#include <vector>

namespace Loudness
{

LoudnessLibrary::LoudnessLibrary( LoudnessLevels levels ) :
	p_LoudnessProcess ( new LoudnessProcess( levels.absoluteThresholdValue, levels.relativeThresholdValue ) ),
	s_levels         ( levels )
{
}

void LoudnessLibrary::initAndStart ( const size_t& channels, const size_t& frequency )
{
	s_durationInSamples = 0;
	s_frequency         = frequency;
	p_LoudnessProcess->init( channels , frequency );
}

void LoudnessLibrary::setUpsamplingFrequencyForTruePeak  ( const size_t& frequency )
{
	p_LoudnessProcess->setUpsamplingFrequencyForTruePeak( frequency );
}

void LoudnessLibrary::processSamples          ( float** samplesData, const size_t& samples )
{
	s_durationInSamples += samples;
	p_LoudnessProcess->process( samples, samplesData );
}

void LoudnessLibrary::getIsShortProgram       ( bool& isShortProgram )
{
  isShortProgram = ( s_durationInSamples < s_frequency * 120 ) ;
}

void LoudnessLibrary::getIntegratedLoudness   ( double& integratedLoudnessValue )
{
	integratedLoudnessValue = p_LoudnessProcess->getIntegrated();
}

void LoudnessLibrary::getIntegratedRange      ( double& integratedRange )
{
	integratedRange         = p_LoudnessProcess->getRangeMax() - p_LoudnessProcess->getRangeMin();
}

void LoudnessLibrary::getMaxShortTermLoudness ( double& maxShortTermLoudnessValue )
{
	maxShortTermLoudnessValue  = p_LoudnessProcess->getMaxLoudnessShortTerm();
}

void LoudnessLibrary::getMinShortTermLoudness ( double& minShortTermLoudnessValue )
{
	minShortTermLoudnessValue  = p_LoudnessProcess->getMinLoudnessShortTerm();
}

void LoudnessLibrary::getMomentaryLoudness    ( double& momentaryLoudnessValue)
{
	momentaryLoudnessValue  = p_LoudnessProcess->getMaxLoudnessMomentary();
}

void LoudnessLibrary::getTruePeakValue        ( double& truePeakValue )
{
	truePeakValue           = p_LoudnessProcess->getTruePeakValue();
}

void LoudnessLibrary::getTruePeakInDbTP       ( double& truePeakValueInDb )
{
	truePeakValueInDb       = p_LoudnessProcess->getTruePeakValueInDb();
}

void LoudnessLibrary::printPloudValues        ( )
{
	std::cout.precision(1);
	std::cout.setf(std::ios::fixed,std::ios::floatfield);
	std::cout << "        I = " << p_LoudnessProcess->getIntegrated()                                 << " LUFS" << std::endl;
	std::cout << "  I range = " << p_LoudnessProcess->getRangeMax() - p_LoudnessProcess->getRangeMin() << " LU"   << std::endl;
	std::cout << "threshold = " << p_LoudnessProcess->getIntegratedThreshold()                        << " LUFS" << std::endl;
	std::cout << "    max M = " << p_LoudnessProcess->getMaxLoudnessMomentary()                       << " LUFS" << std::endl;
	std::cout << "    max S = " << p_LoudnessProcess->getMaxLoudnessShortTerm()                       << " LUFS" << std::endl;
	std::cout << "    min S = " << p_LoudnessProcess->getMinLoudnessShortTerm()                       << " LUFS" << std::endl;

	std::cout << "range th. = " << p_LoudnessProcess->getRangeThreshold()                             << " LUFS" << std::endl;
	std::cout << "range min = " << p_LoudnessProcess->getRangeMin()                                   << " LUFS" << std::endl;
	std::cout << "range max = " << p_LoudnessProcess->getRangeMax()                                   << " LUFS" << std::endl;
	std::cout.precision(6);
	std::cout << "true peak = " << p_LoudnessProcess->getTruePeakValue() << " ( " ;
	std::cout.precision(1);
	std::cout << p_LoudnessProcess->getTruePeakValueInDb() << " dBFS )" << std::endl;
}

std::vector<float> LoudnessLibrary::getTruePeakValues    ( )
{
	return p_LoudnessProcess->getTruePeakValues();
}

std::vector<float> LoudnessLibrary::getShortTermValues    ( )
{
	return p_LoudnessProcess->getShortTermValues();
}

std::vector<int> LoudnessLibrary::getShortTermHistogram ( )
{
	return p_LoudnessProcess->getHistogramShortTerm();
}

ELoudnessResult     LoudnessLibrary::isIntegratedLoudnessValid ( )
{
	float roundedValue = std::floor( p_LoudnessProcess->getIntegrated() * 10.0 ) / 10.0 ;
	if( s_durationInSamples > s_frequency * 120 ) // long program
	{
		//if( ! std::isnan( programLoudnessLongProgramMaxValue ) )
		//    return eNoImportance;
		if ( std::isnan( s_levels.programLoudnessLongProgramMaxValue ) && std::isnan( s_levels.programLoudnessLongProgramMinValue ) )
			return eNoImportance;
		if(  roundedValue > s_levels.programLoudnessLongProgramMaxValue )
			return eNotValidResult;
		else
		{
			if( roundedValue < s_levels.programLoudnessLongProgramMinValue )
				return eNotValidResult;
			else
				return eValidResult;
		}
	}
	else // short program
	{
		if( roundedValue > s_levels.programLoudnessShortProgramMaxValue )
			return eNotValidResult;
		else
			return eValidResult;
	}
}

ELoudnessResult    LoudnessLibrary::isIntegratedLoudnessRangeValid ( )
{
	float loudnessRange = p_LoudnessProcess->getRangeMax() - p_LoudnessProcess->getRangeMin();
	if( s_durationInSamples > s_frequency * 120 ) // long program
	{
		if ( std::isnan( s_levels.maximalLoudnessRange ) && std::isnan( s_levels.minimalLoudnessRange ) )
			return eNoImportance;
		if( ! std::isnan( s_levels.maximalLoudnessRange ) && loudnessRange > s_levels.maximalLoudnessRange )
			return eNotValidResult;
		else
		{
			if( ! std::isnan( s_levels.minimalLoudnessRange ) && loudnessRange < s_levels.minimalLoudnessRange )
				return eNotValidResultButNotIllegal;
			else
				return eValidResult;
		}
	}
	else // short program
	{
		return eNoImportance;
	}
}

ELoudnessResult    LoudnessLibrary::isMaxShortTermLoudnessValid ( )
{

	if( s_durationInSamples > s_frequency * 120 ) // long program
	{
		if( std::isnan( s_levels.shortTermLoudnessLongProgramMaxValue ) )
			return eNoImportance;
		if( ( p_LoudnessProcess->getMaxLoudnessShortTerm() - s_levels.programLoudnessLongProgramMaxValue ) > s_levels.shortTermLoudnessLongProgramMaxValue )
			return eNotValidResultButNotIllegal;
		else
			return eValidResult;
	}
	else // short program
	{
		if( std::isnan( s_levels.shortTermLoudnessShortProgramMaxValue ) )
			return eNoImportance;
		if( ( p_LoudnessProcess->getMaxLoudnessShortTerm() - s_levels.programLoudnessLongProgramMaxValue ) > s_levels.shortTermLoudnessShortProgramMaxValue )
			return eNotValidResult;
		else
			return eValidResult;
	}
}

ELoudnessResult    LoudnessLibrary::isMinShortTermLoudnessValid ( )
{
	if( s_durationInSamples > s_frequency * 120 ) // long program
	{
		if( std::isnan( s_levels.shortTermLoudnessLongProgramMinValue ) )
			return eNoImportance;
		if( ( p_LoudnessProcess->getMinLoudnessShortTerm() - s_levels.programLoudnessLongProgramMaxValue ) < s_levels.shortTermLoudnessLongProgramMinValue )
			return eNotValidResultButNotIllegal;
		else
			return eValidResult;
	}
	else // short program
	{
		return eNoImportance;
	}
}

ELoudnessResult    LoudnessLibrary::isMomentaryLoudnessValid ( )
{
	return eValidResult;
}

ELoudnessResult    LoudnessLibrary::isTruePeakValid ( )
{
	if( std::isnan( s_levels.truePeakMaxValue ) )
		return eNoImportance;
	if( p_LoudnessProcess->getTruePeakValueInDb() < s_levels.truePeakMaxValue )
		return eValidResult;
	else
		return eNotValidResult;
}

}
