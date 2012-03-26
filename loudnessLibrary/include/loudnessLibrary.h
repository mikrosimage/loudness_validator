/*
 * loudnessLibrary.h
 *
 *  Created on: Aug 26, 2011
 *      Author: Marc-Antoine ARNAUD
 */

#ifndef LOUDNESS_LIBRARY_H
#define LOUDNESS_LIBRARY_H

#include <cstdlib>
#include <vector>
#include <memory>
#include <cmath>
#include <limits>

namespace Loudness
{

#define LOUDNESS_NAN std::numeric_limits<float>::quiet_NaN()

struct LoudnessLevels
{
	float programLoudnessLongProgramMaxValue;
	float programLoudnessLongProgramMinValue;
	float shortTermLoudnessLongProgramMaxValue;
	float shortTermLoudnessLongProgramMinValue;

	float programLoudnessShortProgramMaxValue;
	float programLoudnessShortProgramMinValue;
	float shortTermLoudnessShortProgramMaxValue;

	float truePeakMaxValue;

	float absoluteThresholdValue;
	float relativeThresholdValue;

	float maximalLoudnessRange;
	float minimalLoudnessRange;

	// custom levels
	LoudnessLevels(
		float ProgramLoudnessLongProgramMaxValue,
		float ProgramLoudnessLongProgramMinValue,
		float ShortTermLoudnessLongProgramMaxValue,
		float ShortTermLoudnessLongProgramMinValue,
		float ProgramLoudnessShortProgramMaxValue,
		float ProgramLoudnessShortProgramMinValue,
		float ShortTermLoudnessShortProgramMaxValue,
		float TruePeakMaxValue,
		float AbsoluteThresholdValue,
		float RelativeThresholdValue,
		float MaximalLoudnessRange,
		float MinimalLoudnessRange ) :
	programLoudnessLongProgramMaxValue    ( ProgramLoudnessLongProgramMaxValue ),
	programLoudnessLongProgramMinValue    ( ProgramLoudnessLongProgramMinValue ),
	shortTermLoudnessLongProgramMaxValue  ( ShortTermLoudnessLongProgramMaxValue ),
	shortTermLoudnessLongProgramMinValue  ( ShortTermLoudnessLongProgramMinValue ),
	programLoudnessShortProgramMaxValue   ( ProgramLoudnessShortProgramMaxValue ),
	programLoudnessShortProgramMinValue   ( ProgramLoudnessShortProgramMinValue ),
	shortTermLoudnessShortProgramMaxValue ( ShortTermLoudnessShortProgramMaxValue ),
	truePeakMaxValue                      ( TruePeakMaxValue ),
	absoluteThresholdValue                ( AbsoluteThresholdValue ),
	relativeThresholdValue                ( RelativeThresholdValue ),
	maximalLoudnessRange                  ( MaximalLoudnessRange ),
	minimalLoudnessRange                  ( MinimalLoudnessRange )
	{
	}

	static LoudnessLevels Loudness_CST_R017( )
	{
		LoudnessLevels levels(
			-22.f,   // programLoudnessLongProgramMaxValue
			-24.f,   // programLoudnessLongProgramMinValue
			LOUDNESS_NAN, // shortTermLoudnessLongProgramMaxValue
			LOUDNESS_NAN, // shortTermLoudnessLongProgramMinValue
			-23.f,   // programLoudnessShortProgramMaxValue
			-24.f,   // programLoudnessShortProgramMinValue
			+ 3.f,   // shortTermLoudnessShortProgramMaxValue
			- 3.f,   // truePeakMaxValue
			-70.f,   // absoluteThresholdValue
			-10.f,   // relativeThresholdValue
			+20.f,   // maximalLoudnessRange
			+ 5.f    // minimalLoudnessRange
		);
		return levels;
	}

	static LoudnessLevels Loudness_EBU_R128( )
	{
		LoudnessLevels levels(
			-22.0,   // programLoudnessLongProgramMaxValue
			-24.0,   // programLoudnessLongProgramMinValue
			LOUDNESS_NAN, // shortTermLoudnessLongProgramMaxValue
			LOUDNESS_NAN, // shortTermLoudnessLongProgramMinValue
			-22.0,   // programLoudnessShortProgramMaxValue
			-24.0,   // programLoudnessShortProgramMinValue
			LOUDNESS_NAN, // shortTermLoudnessShortProgramMaxValue
			- 1.0,   // truePeakMaxValue
			-70.0,   // absoluteThresholdValue
			-10.0,   // relativeThresholdValue
			LOUDNESS_NAN, // maximalLoudnessRange
			LOUDNESS_NAN  // minimalLoudnessRange
		);
		return levels;
	}

	static LoudnessLevels Loudness_ATSC_A85( )
	{
		LoudnessLevels levels(
			-22.0,   // programLoudnessLongProgramMaxValue
			-26.0,   // programLoudnessLongProgramMinValue
			LOUDNESS_NAN, // shortTermLoudnessLongProgramMaxValue
			LOUDNESS_NAN, // shortTermLoudnessLongProgramMinValue
			-22.0,   // programLoudnessShortProgramMaxValue
			-26.0,   // programLoudnessShortProgramMinValue
			LOUDNESS_NAN, // shortTermLoudnessShortProgramMaxValue
			- 2.0,   // truePeakMaxValue
			-70.0,   // absoluteThresholdValue
			- 8.0,   // relativeThresholdValue
			LOUDNESS_NAN, // maximalLoudnessRange
			LOUDNESS_NAN  // minimalLoudnessRange
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

class LoudnessProcess;

class LoudnessLibrary
{
public:
	LoudnessLibrary( LoudnessLevels levels );

	/**
	 * Initialize and start the Loudness meter (Integrated, Momentary and Short-Term, LRA)
	 * \param channels set the number of channels (default = 2 = stereo)
	 * \param frequency set the frequency sampling (default = 48000 = 48kHz)
	**/
	void initAndStart ( const size_t& channels, const size_t& frequency );


	/**
	 * Select the upsampling frequency for the TruePeakMeter, by default it set to 192kHz.
	 * Call initAndStart() after to set coefficients filters correctly.
	 * \param frequency frequency of the upsampling data in the TruePeakMeter
	 */
	void setUpsamplingFrequencyForTruePeak  ( const size_t& frequency );

	/**
	 * Add samples need to be processed
	 * \param samplesData data for each channels ( data[channel][sampleTime] )
	 * \param samples number of samples in the data pointer
	**/
	void processSamples                     ( float** samplesData, const size_t& samples );


	/**
	 * Return if the program is a Short Program or a Long Program ( > 2'00 )
	 * \param integratedLoudnessValue Integrated Loudness (same as the Program Loudness) in LUFS
	**/
	void getIsShortProgram                  ( bool& isShortProgram );

	/**
	 * Get result of Integrated Loudness, is also called Program Loudness
	 * \param integratedLoudnessValue Integrated Loudness (same as the Program Loudness) in LUFS
	**/
	void getIntegratedLoudness              ( double& integratedLoudnessValue );

	/**
	 * Get result of LRA in LoudnessUnit
	 * \param integratedRange Loudness Range (LRA) in LU
	**/
	void getIntegratedRange                 ( double& integratedRange );

	/**
	 * Get result of maximal Short-Term Loudness value
	 * \param maxShortTermLoudnessValue Short-Term in LUFS ( 3 seconds integrated time )
	**/
	void getMaxShortTermLoudness            ( double& maxShortTermLoudnessValue );

	/**
	 * Get result of minimal Short-Term Loudness value
	 * \param minShortTermLoudnessValue Short-Term in LUFS ( 3 seconds integrated time )
	**/
	void getMinShortTermLoudness            ( double& minShortTermLoudnessValue );

	/**
	 * Get result of Momentary Loudness
	 * \param momentaryLoudnessValue Momentary  in LUFS ( 400 milli-seconds integrated time )
	**/
	void getMomentaryLoudness               ( double& momentaryLoudnessValue);

	/**
	 * Get result of TruePeak meter ( max value of the upsampled signal )
	 * \param truePeakValueInDb TruePeak value [no unit]
	**/
	void getTruePeakValue                   ( double& truePeakValue );

	/**
	 * Get result of TruePeak meter ( in dBTP )
	 * \param truePeakValueInDb TruePeak value [dBTP]
	**/
	void getTruePeakInDbTP                  ( double& truePeakValueInDb );

	/**
	 * Get TruePeak history values
	 * \return vector of TruePeak values (each 1 second)
	**/
	std::vector<float> getTruePeakValues                 ( );

	/**
	 * Get Short-Term histy values
	 * \return vector of Short-Term values
	**/
	std::vector<float> getShortTermValues                 ( );

	/**
	 * Get Short-Term histogram
	 * \return datas of histogram
	**/
	std::vector<int> getShortTermHistogram                ( );



	/**
	 * Print result of Loudness on standard outpu: Integrated, Momentary and Short-Term and LRA
	**/
	void               printPloudValues                   ( );

	/**
	 * Return if the Loudness is valid
	**/
	ELoudnessResult     isIntegratedLoudnessValid          ( );

	/**
	 * Return if the Loudness Range (LRA) is valid
	**/
	ELoudnessResult     isIntegratedLoudnessRangeValid     ( );

	/**
	 * Return if the maximum Short-Term Loudness is valid
	**/
	ELoudnessResult     isMaxShortTermLoudnessValid        ( );

	/**
	 * Return if the minimum Short-Term Loudness is valid
	**/
	ELoudnessResult     isMinShortTermLoudnessValid        ( );

	/**
	 * Return if the Momentary Loudness is valid
	**/
	ELoudnessResult     isMomentaryLoudnessValid           ( );

	/**
	 * Return if the TruePeak is valid
	**/
	ELoudnessResult     isTruePeakValid                    ( );

private:
	std::auto_ptr<LoudnessProcess> p_LoudnessProcess;
	size_t                         s_durationInSamples;
	size_t                         s_frequency;
	LoudnessLevels                 s_levels;
};

}
#endif
