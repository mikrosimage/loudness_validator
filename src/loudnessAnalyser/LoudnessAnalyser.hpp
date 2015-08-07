
#ifndef _LOUDNESS_ANALYSER_LOUDNESS_ANALYSER_HPP_
#define _LOUDNESS_ANALYSER_LOUDNESS_ANALYSER_HPP_

#include "common.hpp"

#include <cstdlib>
#include <vector>
#include <memory>
#include <limits>

namespace Loudness
{

#define LOUDNESS_NAN std::numeric_limits<float>::quiet_NaN()

enum LoudnessExport EStandard
{
	eStandardCST_R017 = 0,
	eStandardEBU_R128,
	eStandardATSC_A85,
	eStandardUnknown
};

struct LoudnessExport LoudnessLevels
{
	float programLoudnessLongProgramMaxValue;
	float programLoudnessLongProgramMinValue;
	float programLoudnessLongProgramTargetLevel;
	float programLoudnessLongProgramTargetMaxLevel;
	float programLoudnessLongProgramTargetMinLevel;
	
	float shortTermLoudnessLongProgramMaxValue;
	float shortTermLoudnessLongProgramMinValue;
	
	float programLoudnessShortProgramMaxValue;
	float programLoudnessShortProgramMinValue;
	float programLoudnessShortProgramTargetLevel;
	float programLoudnessShortProgramTargetMaxLevel;
	float programLoudnessShortProgramTargetMinLevel;
	
	float shortTermLoudnessShortProgramMaxValue;

	float truePeakMaxValue;
	float truePeakTargetLevel;

	float absoluteThresholdValue;
	float relativeThresholdValue;

	float maximalLoudnessRange;
	float minimalLoudnessRange;

	EStandard standard;
	
	// custom levels
	LoudnessLevels(
		float ProgramLoudnessLongProgramMaxValue,
		float ProgramLoudnessLongProgramMinValue,
		float ProgramLoudnessLongProgramTargetLevel,
		float ProgramLoudnessLongProgramTargetMaxLevel,
		float ProgramLoudnessLongProgramTargetMinLevel,
		float ShortTermLoudnessLongProgramMaxValue,
		float ShortTermLoudnessLongProgramMinValue,
		float ProgramLoudnessShortProgramMaxValue,
		float ProgramLoudnessShortProgramMinValue,
		float ProgramLoudnessShortProgramTargetLevel,
		float ProgramLoudnessShortProgramTargetMaxLevel,
		float ProgramLoudnessShortProgramTargetMinLevel,
		float ShortTermLoudnessShortProgramMaxValue,
		float TruePeakMaxValue,
		float TruePeakTargetLevel,
		float AbsoluteThresholdValue,
		float RelativeThresholdValue,
		float MaximalLoudnessRange,
		float MinimalLoudnessRange,
		EStandard Standard = eStandardUnknown ) :
	programLoudnessLongProgramMaxValue        ( ProgramLoudnessLongProgramMaxValue ),
	programLoudnessLongProgramMinValue        ( ProgramLoudnessLongProgramMinValue ),
	programLoudnessLongProgramTargetLevel     ( ProgramLoudnessLongProgramTargetLevel ),
	programLoudnessLongProgramTargetMaxLevel  ( ProgramLoudnessLongProgramTargetMaxLevel ),
	programLoudnessLongProgramTargetMinLevel  ( ProgramLoudnessLongProgramTargetMinLevel ),
	shortTermLoudnessLongProgramMaxValue      ( ShortTermLoudnessLongProgramMaxValue ),
	shortTermLoudnessLongProgramMinValue      ( ShortTermLoudnessLongProgramMinValue ),
	programLoudnessShortProgramMaxValue       ( ProgramLoudnessShortProgramMaxValue ),
	programLoudnessShortProgramMinValue       ( ProgramLoudnessShortProgramMinValue ),
	programLoudnessShortProgramTargetLevel    ( ProgramLoudnessShortProgramTargetLevel ),
	programLoudnessShortProgramTargetMaxLevel ( ProgramLoudnessShortProgramTargetMaxLevel ),
	programLoudnessShortProgramTargetMinLevel ( ProgramLoudnessShortProgramTargetMinLevel ),
	shortTermLoudnessShortProgramMaxValue     ( ShortTermLoudnessShortProgramMaxValue ),
	truePeakMaxValue                          ( TruePeakMaxValue ),
	truePeakTargetLevel                       ( TruePeakTargetLevel ),
	absoluteThresholdValue                    ( AbsoluteThresholdValue ),
	relativeThresholdValue                    ( RelativeThresholdValue ),
	maximalLoudnessRange                      ( MaximalLoudnessRange ),
	minimalLoudnessRange                      ( MinimalLoudnessRange ),
	standard                                  ( Standard )
	{
	}

	static LoudnessLevels Loudness_CST_R017( )
	{
		LoudnessLevels levels(
			-22.0,   // programLoudnessLongProgramMaxValue
			-24.0,   // programLoudnessLongProgramMinValue
			-23.0,   // programLoudnessLongProgramTargetLevel
			-22.5,   // programLoudnessLongProgramTargetMaxLevel
			-23.5,   // programLoudnessLongProgramTargetMinLevel
			LOUDNESS_NAN, // shortTermLoudnessLongProgramMaxValue
			LOUDNESS_NAN, // shortTermLoudnessLongProgramMinValue
			-23.0,   // programLoudnessShortProgramMaxValue
			-24.0,   // programLoudnessShortProgramMinValue
			-23.5,   // programLoudnessShortProgramTargetLevel
			-23.2,   // programLoudnessShortProgramTargetMaxLevel
			-23.8,   // programLoudnessShortProgramTargetMinLevel
			+ 3.0,   // shortTermLoudnessShortProgramMaxValue
			- 3.0,   // truePeakMaxValue
			- 4.0,   // truePeakTargetLevel
			-70.0,   // absoluteThresholdValue
			-10.0,   // relativeThresholdValue
			+20.0,   // maximalLoudnessRange
			+ 5.0,   // minimalLoudnessRange
			eStandardCST_R017 // standard
		);
		return levels;
	}

	static LoudnessLevels Loudness_EBU_R128( )
	{
		LoudnessLevels levels(
			-22.0,   // programLoudnessLongProgramMaxValue
			-24.0,   // programLoudnessLongProgramMinValue
			-23.0,   // programLoudnessLongProgramTargetLevel
			-22.5,   // programLoudnessLongProgramTargetMaxLevel
			-23.5,   // programLoudnessLongProgramTargetMinLevel
			LOUDNESS_NAN, // shortTermLoudnessLongProgramMaxValue
			LOUDNESS_NAN, // shortTermLoudnessLongProgramMinValue
			-22.0,   // programLoudnessShortProgramMaxValue
			-24.0,   // programLoudnessShortProgramMinValue
			-23.0,   // programLoudnessShortProgramTargetLevel
			-22.5,   // programLoudnessShortProgramTargetMaxLevel
			-23.5,   // programLoudnessShortProgramTargetMinLevel
			LOUDNESS_NAN, // shortTermLoudnessShortProgramMaxValue
			- 1.0,   // truePeakMaxValue
			- 2.0,   // truePeakTargetLevel
			-70.0,   // absoluteThresholdValue
			-10.0,   // relativeThresholdValue
			LOUDNESS_NAN, // maximalLoudnessRange
			LOUDNESS_NAN, // minimalLoudnessRange
			eStandardEBU_R128     // standard
		);
		return levels;
	}

	static LoudnessLevels Loudness_ATSC_A85( )
	{
		LoudnessLevels levels(
			-22.0,   // programLoudnessLongProgramMaxValue
			-26.0,   // programLoudnessLongProgramMinValue
			-24.0,   // programLoudnessLongProgramTargetLevel
			-23.0,   // programLoudnessLongProgramTargetMaxLevel
			-25.0,   // programLoudnessLongProgramTargetMinLevel
			LOUDNESS_NAN, // shortTermLoudnessLongProgramMaxValue
			LOUDNESS_NAN, // shortTermLoudnessLongProgramMinValue
			-22.0,   // programLoudnessShortProgramMaxValue
			-26.0,   // programLoudnessShortProgramMinValue
			-24.0,   // programLoudnessLongProgramTargetLevel
			-23.0,   // programLoudnessLongProgramTargetMaxLevel
			-25.0,   // programLoudnessLongProgramTargetMinLevel
			LOUDNESS_NAN, // shortTermLoudnessShortProgramMaxValue
			- 2.0,   // truePeakMaxValue
			- 3.0,   // truePeakTargetLevel
			-70.0,   // absoluteThresholdValue
			- 8.0,   // relativeThresholdValue
			LOUDNESS_NAN, // maximalLoudnessRange
			LOUDNESS_NAN, // minimalLoudnessRange
			eStandardATSC_A85     // standard
		);
		return levels;
	}
};

enum ELoudnessResult
{
	eValidResult = 0,                 ///< a compliant result
	eNotValidResult = 1,              ///< not a valid result
	eNotValidResultButNotIllegal = 2, ///< warning value result
	eNoImportance = 3                 ///< when levels are set to LOUDNESS_NAN, disable levels checks
};

class Process;

class LoudnessExport LoudnessAnalyser
{
public:
	LoudnessAnalyser( LoudnessLevels& levels );

	/**
	 * Initialize and start the Loudness meter (Integrated, Momentary and Short-Term, LRA)
	 * \param channels set the number of channels (default = 2 = stereo)
	 * \param frequency set the frequency sampling (default = 48000 = 48kHz)
	 * \param  enableOptimisation enable optimisation code (based on SIMD instructions)
	**/
	void initAndStart( const size_t channels, const size_t frequency, const bool enableOptimization = true );


	/**
	 * Select the upsampling frequency for the TruePeakMeter, by default it set to 192kHz.
	 * Call initAndStart() after to set coefficients filters correctly.
	 * \param frequency frequency of the upsampling data in the TruePeakMeter
	 */
	void setUpsamplingFrequencyForTruePeak( const size_t frequency );

	/**
	 * Add samples need to be processed
	 * \param samplesData data for each channels ( data[channel][sampleTime] )
	 * \param samples number of samples in the data pointer
	**/
	void processSamples( float** samplesData, const size_t nbSamples );


	/**
	 * Return if the program is a Short Program or a Long Program ( > 2'00 )
	 * \param integratedLoudnessValue Integrated Loudness (same as the Program Loudness) in LUFS
	**/
	bool isShortProgram( );

	/**
	 * Return the standard used to valid the program
	**/
	EStandard getStandard( );
	
	/**
	 * Get result of Integrated Loudness, is also called Program Loudness
	 * \param integratedLoudnessValue Integrated Loudness (same as the Program Loudness) in LUFS
	**/
	double getIntegratedLoudness( );

	/**
	 * Get result of LRA in LoudnessUnit
	 * \param integratedRange Loudness Range (LRA) in LU
	**/
	double getIntegratedRange( );

	/**
	 * Get result of maximal Short-Term Loudness value
	 * \param maxShortTermLoudnessValue Short-Term in LUFS ( 3 seconds integrated time )
	**/
	double getMaxShortTermLoudness( );

	/**
	 * Get result of minimal Short-Term Loudness value
	 * \param minShortTermLoudnessValue Short-Term in LUFS ( 3 seconds integrated time )
	**/
	double getMinShortTermLoudness( );

	/**
	 * Get result of Momentary Loudness
	 * \param momentaryLoudnessValue Momentary  in LUFS ( 400 milli-seconds integrated time )
	**/
	double getMomentaryLoudness( );

	/**
	 * Get result of TruePeak meter ( max value of the upsampled signal )
	 * \param truePeakValueInDb TruePeak value [no unit]
	**/
	double getTruePeakValue( );

	/**
	 * Get result of TruePeak meter ( in dBTP )
	 * \param truePeakValueInDb TruePeak value [dBTP]
	**/
	double getTruePeakInDbTP( );

	/**
	 * Get TruePeak history values
	 * \return vector of TruePeak values (each 1 second)
	**/
	std::vector<float> getTruePeakValues( );

	/**
	 * Get Short-Term histy values
	 * \return vector of Short-Term values
	**/
	std::vector<float> getShortTermValues( );

	/**
	 * Get Short-Term histogram
	 * \return datas of histogram
	**/
	std::vector<int> getShortTermHistogram( );


	/**
	 * Print result of Loudness on standard output Integrated, Momentary and Short-Term and LRA
	**/
	void printPloudValues( );

	/**
	 * Return if the program is valid
	**/
	ELoudnessResult     isValidProgram( );
	
	/**
	 * Return if the Loudness is valid
	**/
	ELoudnessResult     isIntegratedLoudnessValid( );

	/**
	 * Return if the Loudness Range (LRA) is valid
	**/
	ELoudnessResult     isIntegratedLoudnessRangeValid( );

	/**
	 * Return if the maximum Short-Term Loudness is valid
	**/
	ELoudnessResult     isMaxShortTermLoudnessValid( );

	/**
	 * Return if the minimum Short-Term Loudness is valid
	**/
	ELoudnessResult     isMinShortTermLoudnessValid( );

	/**
	 * Return if the Momentary Loudness is valid
	**/
	ELoudnessResult     isMomentaryLoudnessValid( );

	/**
	 * Return if the TruePeak is valid
	**/
	ELoudnessResult     isTruePeakValid( );
	
	/**
	 * Return gain to correct loudness to a valid value
	 * if it return 0.0 and loudness is invalid, this means it's impossible to correct without a remix
	**/
	float               getCorrectionGain( const bool limiterIsEnable = false );

protected:
	std::auto_ptr<Process> p_process;
	size_t                 s_durationInSamples;
	size_t                 s_frequency;
	LoudnessLevels         s_levels;
};

}
#endif
