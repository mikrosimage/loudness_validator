/*
 * loudnessLibrary.cpp
 *
 *  Created on: Aug 26, 2011
 *      Author: Marc-Antoine ARNAUD
 *              MikrosImage R&D
 */

#include "loudnessLibrary.h"
#include "process.h"
#include "common.h"

#include <iostream>
#include <iomanip>
#include <limits>
#include <vector>

namespace Loudness
{

LoudnessLibrary::LoudnessLibrary( LoudnessLevels& levels ) :
	p_process ( new Process( levels.absoluteThresholdValue, levels.relativeThresholdValue ) ),
	s_levels  ( levels )
{
}

void LoudnessLibrary::initAndStart ( const size_t& channels, const size_t& frequency )
{
	s_durationInSamples = 0;
	s_frequency         = frequency;
	p_process->init( channels , frequency );
}

void LoudnessLibrary::setUpsamplingFrequencyForTruePeak  ( const size_t& frequency )
{
	p_process->setUpsamplingFrequencyForTruePeak( frequency );
}

void LoudnessLibrary::processSamples          ( float** samplesData, const size_t& samples )
{
	s_durationInSamples += samples;
	p_process->process( samples, samplesData );
}

bool LoudnessLibrary::isShortProgram( )
{
  return ( s_durationInSamples < s_frequency * 120 ) ;
}

double LoudnessLibrary::getIntegratedLoudness( )
{
	return p_process->getIntegrated();
}

double LoudnessLibrary::getIntegratedRange( )
{
	return p_process->getRangeMax() - p_process->getRangeMin();
}

double LoudnessLibrary::getMaxShortTermLoudness( )
{
	return p_process->getMaxLoudnessShortTerm();
}

double LoudnessLibrary::getMinShortTermLoudness( )
{
	return p_process->getMinLoudnessShortTerm();
}

double LoudnessLibrary::getMomentaryLoudness( )
{
	return p_process->getMaxLoudnessMomentary();
}

double LoudnessLibrary::getTruePeakValue( )
{
	return p_process->getTruePeakValue();
}

double LoudnessLibrary::getTruePeakInDbTP( )
{
	return p_process->getTruePeakValueInDb();
}

void LoudnessLibrary::printPloudValues        ( )
{
	std::cout.precision(1);
	std::cout.setf(std::ios::fixed,std::ios::floatfield);
	std::cout << "Integrated (Program Loudness) = " << p_process->getIntegrated()                                 << " LUFS" << std::endl;
	std::cout << "       Integrated range (LRA) = " << p_process->getRangeMax() - p_process->getRangeMin() << " LU"   << std::endl;
	std::cout << "                max Momentary = " << p_process->getMaxLoudnessMomentary()                       << " LUFS" << std::endl;
	std::cout << "               max Short-Term = " << p_process->getMaxLoudnessShortTerm()                       << " LUFS" << std::endl;
	std::cout << "               min Short-Term = " << p_process->getMinLoudnessShortTerm()                       << " LUFS" << std::endl;
	std::cout.precision(6);
	std::cout << "                    true peak = " << p_process->getTruePeakValue() << " ( " ;
	std::cout.precision(1);
	std::cout << p_process->getTruePeakValueInDb() << " dBFS )" << std::endl;
	std::cout << std::endl;
	std::cout << "         Integrated threshold = " << p_process->getIntegratedThreshold()                        << " LUFS" << std::endl;
	std::cout << "              Range threshold = " << p_process->getRangeThreshold()                             << " LUFS" << std::endl;
	
	std::cout << "                    range min = " << p_process->getRangeMin()                                   << " LUFS" << std::endl;
	std::cout << "                    range max = " << p_process->getRangeMax()                                   << " LUFS" << std::endl;
	std::cout << std::endl;
}

std::vector<float> LoudnessLibrary::getTruePeakValues    ( )
{
	return p_process->getTruePeakValues();
}

std::vector<float> LoudnessLibrary::getShortTermValues    ( )
{
	return p_process->getShortTermValues();
}

std::vector<int>   LoudnessLibrary::getShortTermHistogram ( )
{
	return p_process->getHistogramShortTerm();
}

ELoudnessResult    LoudnessLibrary::isIntegratedLoudnessValid ( )
{
	float roundedValue = std::floor( p_process->getIntegrated() * 10.0 ) / 10.0 ;
	if( isShortProgram() ) // short program
	{
		if( roundedValue > s_levels.programLoudnessShortProgramMaxValue || roundedValue < s_levels.programLoudnessShortProgramMinValue )
			return eNotValidResult;
		else
			return eValidResult;
	}
	else // long program
	{
		if ( std::isnan( s_levels.programLoudnessLongProgramMaxValue ) && std::isnan( s_levels.programLoudnessLongProgramMinValue ) )
			return eNoImportance;
		if(  roundedValue > s_levels.programLoudnessLongProgramMaxValue || roundedValue < s_levels.programLoudnessLongProgramMinValue  )
			return eNotValidResult;
		else
			return eValidResult;
	}
}

ELoudnessResult    LoudnessLibrary::isIntegratedLoudnessRangeValid ( )
{
	float loudnessRange = p_process->getRangeMax() - p_process->getRangeMin();
	if( isShortProgram() ) // short program
	{
		return eNoImportance;
	}
	else // long program
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
}

ELoudnessResult    LoudnessLibrary::isMaxShortTermLoudnessValid ( )
{
	if( isShortProgram() ) // short program
	{
		if( std::isnan( s_levels.shortTermLoudnessShortProgramMaxValue ) )
			return eNoImportance;
		if( ( p_process->getMaxLoudnessShortTerm() - s_levels.programLoudnessLongProgramMaxValue ) > s_levels.shortTermLoudnessShortProgramMaxValue )
			return eNotValidResult;
		else
			return eValidResult;
	}
	else // long program
	{
		if( std::isnan( s_levels.shortTermLoudnessLongProgramMaxValue ) )
			return eNoImportance;
		if( ( p_process->getMaxLoudnessShortTerm() - s_levels.programLoudnessLongProgramMaxValue ) > s_levels.shortTermLoudnessLongProgramMaxValue )
			return eNotValidResultButNotIllegal;
		else
			return eValidResult;
	}
}

ELoudnessResult    LoudnessLibrary::isMinShortTermLoudnessValid ( )
{
	if( isShortProgram() ) // short program
	{
		return eNoImportance;
	}
	else // long program
	{
		if( std::isnan( s_levels.shortTermLoudnessLongProgramMinValue ) )
			return eNoImportance;
		if( ( p_process->getMinLoudnessShortTerm() - s_levels.programLoudnessLongProgramMaxValue ) < s_levels.shortTermLoudnessLongProgramMinValue )
			return eNotValidResultButNotIllegal;
		else
			return eValidResult;
	}
}

ELoudnessResult    LoudnessLibrary::isMomentaryLoudnessValid ( )
{
	return eNoImportance;
}

ELoudnessResult    LoudnessLibrary::isTruePeakValid ( )
{
	if( std::isnan( s_levels.truePeakMaxValue ) )
		return eNoImportance;
	if( p_process->getTruePeakValueInDb() < s_levels.truePeakMaxValue )
		return eValidResult;
	else
		return eNotValidResult;
}

float LoudnessLibrary::getCorrectionGain( )
{
	return p_process->getCorrectionGain( s_levels, isShortProgram() );
}


}
