
#include "LoudnessAnalyser.hpp"
#include "Process.hpp"

#include <iostream>
#include <iomanip>
#include <limits>
#include <vector>

namespace Loudness {
namespace analyser {

LoudnessAnalyser::LoudnessAnalyser( LoudnessLevels& levels ) :
	p_process ( new Process( levels.absoluteThresholdValue, levels.relativeThresholdValue ) ),
	s_levels  ( levels )
{
}

void LoudnessAnalyser::initAndStart( const size_t channels, const size_t frequency, const bool enableOptimization )
{
	s_durationInSamples = 0;
	s_frequency         = frequency;
	p_process->init( channels , frequency, enableOptimization );
}

void LoudnessAnalyser::setUpsamplingFrequencyForTruePeak( const size_t frequency )
{
	p_process->setUpsamplingFrequencyForTruePeak( frequency );
}

void LoudnessAnalyser::processSamples( float** samplesData, const size_t nbSamples )
{
	s_durationInSamples += nbSamples;
	p_process->process( nbSamples, samplesData );
}

bool LoudnessAnalyser::isShortProgram( )
{
	return ( s_durationInSamples < s_frequency * 120 ) ;
}

EStandard LoudnessAnalyser::getStandard( )
{
	return s_levels.standard;
}

double LoudnessAnalyser::getIntegratedLoudness( )
{
	return p_process->getIntegrated();
}

double LoudnessAnalyser::getIntegratedRange( )
{
	return p_process->getRangeMax() - p_process->getRangeMin();
}

double LoudnessAnalyser::getMaxShortTermLoudness( )
{
	return p_process->getMaxLoudnessShortTerm();
}

double LoudnessAnalyser::getMinShortTermLoudness( )
{
	return p_process->getMinLoudnessShortTerm();
}

double LoudnessAnalyser::getMomentaryLoudness( )
{
	return p_process->getMaxLoudnessMomentary();
}

double LoudnessAnalyser::getTruePeakValue( )
{
	return p_process->getTruePeakValue();
}

double LoudnessAnalyser::getTruePeakInDbTP( )
{
	return p_process->getTruePeakValueInDb();
}

void LoudnessAnalyser::printPloudValues( )
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

std::vector<float> LoudnessAnalyser::getTruePeakValues    ( )
{
	return p_process->getTruePeakValues();
}

std::vector<float> LoudnessAnalyser::getShortTermValues    ( )
{
	return p_process->getShortTermValues();
}

std::vector<int> LoudnessAnalyser::getShortTermHistogram ( )
{
	return p_process->getHistogramShortTerm();
}

ELoudnessResult LoudnessAnalyser::isValidProgram( )
{
	size_t invalidState = 0; // if a not valid result is present
	size_t notIllegalState = 0; // if a not valid but not illegal result is present

	// integrated loudness
	switch( isIntegratedLoudnessValid() )
	{
		case eNotValidResult: invalidState++; break;
		case eNotValidResultButNotIllegal: notIllegalState++; break;
		default: break;
	}

	// LRA
	switch( isIntegratedLoudnessRangeValid() )
	{
		case eNotValidResult: invalidState++; break;
		case eNotValidResultButNotIllegal: notIllegalState++; break;
		default: break;
	}

	// max short-term
	switch( isMaxShortTermLoudnessValid() )
	{
		case eNotValidResult: invalidState++; break;
		case eNotValidResultButNotIllegal: notIllegalState++; break;
		default: break;
	}

	// True Peak
	switch( isTruePeakValid() )
	{
		case eNotValidResult: invalidState++; break;
		case eNotValidResultButNotIllegal: notIllegalState++; break;
		default: break;
	}

	if( invalidState == 0 && notIllegalState == 0 )
		return eValidResult;
	
	if( invalidState == 0 )
		return eNotValidResultButNotIllegal;
	
	return eNotValidResult;
}

ELoudnessResult LoudnessAnalyser::isIntegratedLoudnessValid ( )
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

ELoudnessResult LoudnessAnalyser::isIntegratedLoudnessRangeValid ( )
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

ELoudnessResult LoudnessAnalyser::isMaxShortTermLoudnessValid ( )
{
	if( isShortProgram() ) // short program
	{
		if( std::isnan( s_levels.shortTermLoudnessShortProgramMaxValue ) )
			return eNoImportance;
		if( ( p_process->getMaxLoudnessShortTerm() - s_levels.programLoudnessShortProgramMaxValue ) > s_levels.shortTermLoudnessShortProgramMaxValue )
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

ELoudnessResult LoudnessAnalyser::isMinShortTermLoudnessValid ( )
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

ELoudnessResult LoudnessAnalyser::isMomentaryLoudnessValid ( )
{
	return eNoImportance;
}

ELoudnessResult LoudnessAnalyser::isTruePeakValid ( )
{
	if( std::isnan( s_levels.truePeakMaxValue ) )
		return eNoImportance;
	if( p_process->getTruePeakValueInDb() < s_levels.truePeakMaxValue )
		return eValidResult;
	else
		return eNotValidResult;
}

float LoudnessAnalyser::getCorrectionGain( const bool limiterIsEnable )
{
	return p_process->getCorrectionGain( s_levels, isShortProgram(), limiterIsEnable );
}


}
}
