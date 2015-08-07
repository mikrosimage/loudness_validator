#include "PLoudProcess.hpp"

#include <loudnessAnalyser/common.hpp>

#include <iostream>
#include <algorithm>

PLoudProcess::PLoudProcess( Loudness::LoudnessLevels levels, float frequencyForTruePeak )
	: Loudness::LoudnessAnalyser( levels )
{
	setUpsamplingFrequencyForTruePeak( frequencyForTruePeak );
}

bool PLoudProcess::openAudioFiles( std::vector<std::string>& files )
{
	bool ret = true;
	for( size_t i = 0; i< files.size(); i++ )
	{
		filenames.push_back( files.at( i ) );
		Loudness::io::SoundFile* audioFile = new Loudness::io::SoundFile();
		bool tmpRet = audioFile->open_read( files.at( i ).c_str() );
		tmpRet = !tmpRet;
		if( tmpRet )
		{
			audioFiles.push_back( audioFile );
		}
		else
		{
			delete audioFile;
			ret = false;
		}
	}
	return ret;
}

void PLoudProcess::closeAudioFiles( )
{
	for( size_t i = 0; i< audioFiles.size(); i++ )
	{
		audioFiles.at(i)->close();
		delete audioFiles.at(i);
	}
	audioFiles.clear();
}

float PLoudProcess::getProgramDuration()
{
	if( audioFiles.size() < 1 )
		return -1.0;
	
        return audioFiles.at(0)->getNbSamples() / audioFiles.at(0)->getSampleRate();
}

int PLoudProcess::getProgramLength()
{
	if( audioFiles.size() < 1 )
		return -1.0;
	
        return audioFiles.at(0)->getNbSamples();
}

void PLoudProcess::processAnalyseFile( void (*callback)(void*, int), void* object, double gain )
{
	int cumulOfSamples = 0;
	Loudness::io::SoundFile* audioFile = audioFiles.at(0);
	
	size_t channelsInBuffer = std::min( 5, audioFile->getNbChannels() );
	int bufferSize = audioFile->getSampleRate() / 5;
	float *data [ MAX_CHANNELS ];
	float* inpb = new float [ audioFile->getNbChannels() * bufferSize ];
	
	for( size_t i = 0; i< channelsInBuffer; i++ )
		data [i] = new float [bufferSize];
	
	initAndStart( channelsInBuffer, audioFile->getSampleRate() );
	
	audioFile->seek( 0 );
	
	while (true)
	{
		int  samples = audioFile->read( inpb, bufferSize );
		if (samples == 0) break;

		cumulOfSamples += samples;

		callback( object, cumulOfSamples );
		float* p = inpb;
		for (int i = 0; i < samples; i++)
		{
			for(size_t c = 0; c < channelsInBuffer; c++ )
				data [c][i] = (*p++) * gain;
		}
		processSamples( data, samples );
	}
	
	delete[] inpb;
	for( int i=0; i< audioFile->getNbChannels(); i++ )
		delete[] data[i];
}

bool PLoudProcess::analyseToFindCorrectionGain( void (*callback)(void*, int), void* object, double& foundedGain )
{
	double IL = getIntegratedLoudness( );
	double TP = getTruePeakInDbTP( );
	double maxGain   = 1.0;
	double minGain   = 1.0;
	double gain      = 1.0;
	size_t countPass = 0;
	
	std::cout.precision(6);
	std::cout << "gain: " << gain ;
	std::cout.precision(1);
	std::cout << " Program Loudness = " << IL << " True Peak = " << TP << std::endl;
	
	if( IL > s_levels.programLoudnessLongProgramTargetMaxLevel )
	{
		minGain = 0.1;
		do
		{
			countPass ++;
			//progressMsg.setText( "Analyse pass " + QString::number( countPass ) );
			gain = ( maxGain + minGain ) * 0.5;
			processAnalyseFile( callback, object, gain );
			IL = getIntegratedLoudness( );
			TP = getTruePeakInDbTP( );
			std::cout.precision(6);
			std::cout << "gain: " << gain ;
			std::cout.precision(1);
			std::cout << " Program Loudness = " << IL << " True Peak = " << TP << std::endl;
			
			if( IL > s_levels.programLoudnessLongProgramTargetLevel )
				maxGain = gain;
			else
				minGain = gain;
		}
		while( ( IL > s_levels.programLoudnessLongProgramTargetMaxLevel || IL < s_levels.programLoudnessLongProgramTargetMinLevel ) && TP < s_levels.truePeakMaxValue );
	}
	else
	{
		if( IL < s_levels.programLoudnessLongProgramTargetMinLevel )
		{
			maxGain = 10;
			do
			{
				countPass ++;
				//progressMsg.setText( "Analyse pass " + QString::number( countPass ) );
				gain = ( maxGain + minGain ) * 0.5;
				processAnalyseFile( callback, object, gain );
				IL = getIntegratedLoudness( );
				TP = getTruePeakInDbTP( );
				
				std::cout.precision(6);
				std::cout << "gain: " << gain ;
				std::cout.precision(1);
				std::cout << " Program Loudness = " << IL << " True Peak = " << TP << std::endl;
				
				if( IL > s_levels.programLoudnessLongProgramTargetLevel )
					maxGain = gain;
				else
					minGain = gain;
			}
			while( ( IL > s_levels.programLoudnessLongProgramTargetMaxLevel || IL < s_levels.programLoudnessLongProgramTargetMinLevel ) && TP < s_levels.truePeakMaxValue );
		}
		else
		{
			//progressMsg.setText( "No correction required" );
			foundedGain = 1.0;
			return false;
		}
	}
	foundedGain = gain;
	return true;
}

void PLoudProcess::writeFile( void (*callback)(void*, int), void* object, double outputGain )
{
	Loudness::io::SoundFile audioOutputFile;
	
	std::string outputFilemane = filenames.at(0);
	
	// openning output file
	size_t position = 0;
	if( outputFilemane.at( outputFilemane.length() - 4 ) == '.' ) // is wav or aif file, not aiff
		position = 4;
	else
		position = 5;
	
	outputFilemane.insert( outputFilemane.length() - position, "_corrected" );
	
	audioOutputFile.open_write ( outputFilemane.c_str(), audioFiles.at(0)->getAudioCodec(), audioFiles.at(0)->getBitDepth(), audioFiles.at(0)->getSampleRate(), audioFiles.at(0)->getNbChannels() );
	
	// correction of the file, and write into a corrected file.
	
	int bufferSize = audioFiles.at(0)->getSampleRate() / 5;
	size_t channelsInBuffer = std::min( 5, audioFiles.at(0)->getNbChannels() );
	size_t cumulOfSamples = 0;

	float* data [ MAX_CHANNELS ];
	float* inpb  = new float [audioFiles.at(0)->getNbChannels() * bufferSize];
	
	for( size_t i=0; i < channelsInBuffer; i++ )
		data [i] = new float [bufferSize];
	
        audioFiles.at(0)->seek( 0 );
	
	while (true)
	{
		int  samples = audioFiles.at(0)->read( inpb, bufferSize );
		if (samples == 0) break;
		
		float* ptr = inpb;
		for( size_t i = 0; i < samples * channelsInBuffer; i++, ptr++ )
		{
			*ptr = (*ptr) * outputGain;
		}
		
		samples = audioOutputFile.write( inpb, samples );
		cumulOfSamples += samples;
		callback( object, cumulOfSamples );
	}
	
	for( int i=0; i< audioFiles.at(0)->getNbChannels(); i++ )
		delete[] data[i];
	
	audioOutputFile.close();
}
