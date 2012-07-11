#ifndef WRITE_CORRECTED_FILE_H
#define WRITE_CORRECTED_FILE_H

#include <fstream>
#include <vector>
#include <SoundFile.h>
#include <CorrectBuffer.h>
#include <LookAheadLimiter.h>
/*
void correctBuffer( Loudness::LoudnessLibrary& analyser, float* data, const size_t samples, const size_t channelsInBuffer, const float gain )
{
	float* dataPerChannel[ channelsInBuffer ];
	
	for( size_t i = 0; i< channelsInBuffer; i++ )
		dataPerChannel [i] = new float [samples];
	
	for( size_t i = 0; i < samples; i++ )
	{
		for( size_t c = 0; c < channelsInBuffer; c++ )
		{
			(*data) = dataPerChannel [c][i] = (*data) * gain;
			data++;
		}
	}
	analyser.processSamples( dataPerChannel, samples );
	
	for( size_t i=0; i < channelsInBuffer; i++ )
		delete[] dataPerChannel[i];
}*/

void writeCorrectedFile( Loudness::LoudnessLibrary& analyser, SoundFile& input, SoundFile& output, const float gain, void (*callback)(int) )
{
	int    bufferSize       = input.rate () / 5;
	int    length           = input.size();
	size_t channelsInBuffer = input.chan();
	size_t cumulOfSamples   = 0;
	
	float* inpb             = new float [input.chan() * bufferSize];
	float* dataPerChannel[ channelsInBuffer ];
	
	for( size_t i = 0; i< channelsInBuffer; i++ )
		dataPerChannel [i] = new float [bufferSize];
	
	input.seek( 0 );
	
	analyser.initAndStart( channelsInBuffer, input.rate() );
	
	while (true)
	{
		int  samples = input.read( inpb, bufferSize );
		if (samples == 0) break;
		
		float* ptr = inpb;
		
		correctBuffer( ptr, samples, channelsInBuffer, gain );
		
		// re-analyse output
		ptr = inpb;
		
		for( int i = 0; i < samples; i++ )
		{
			for( size_t c = 0; c < channelsInBuffer; c++ )
			{
				dataPerChannel [c][i] = (*ptr);
				ptr++;
			}
		}

		analyser.processSamples( dataPerChannel, samples );
		
		samples = output.write( inpb, samples );
		cumulOfSamples += samples;
		callback( (float)cumulOfSamples / length * 100 );
	}
	for( size_t i=0; i < channelsInBuffer; i++ )
		delete[] dataPerChannel[i];
	delete[] inpb;
}

void writeCorrectedFile( Loudness::LoudnessLibrary& analyser, SoundFile& input, SoundFile& output, const float gain, float lookAhead, float threshold, void (*callback)(int) )
{
	int    bufferSize       = input.rate () / 5;
	int    length           = input.size();
	size_t channelsInBuffer = input.chan();
	size_t cumulOfSamples   = 0;

	float* inpb  = new float [ input.chan() * bufferSize ];
	float* dataPerChannel[ channelsInBuffer ];
	
	for( size_t i = 0; i< channelsInBuffer; i++ )
		dataPerChannel [i] = new float [bufferSize];
	
	std::vector<LookAheadLimiter*> limiters;
	
	for( size_t i = 0; i< channelsInBuffer; i++ )
	{
		LookAheadLimiter* lim = new LookAheadLimiter( lookAhead, input.rate(), threshold );
		limiters.push_back( lim );
	}

	input.seek( 0 );
	
	analyser.initAndStart( channelsInBuffer, input.rate() );
	
	while (true)
	{
		int  samples = input.read( inpb, bufferSize );
		if( samples == 0 ) break;
		
		float* ptr = inpb;
		
		correctBuffer( limiters, ptr, samples, channelsInBuffer, gain );
		
		// re-analyse output
		ptr = inpb;
		
		for( int i = 0; i < samples; i++ )
		{
			for( size_t c = 0; c < channelsInBuffer; c++ )
			{
				dataPerChannel [c][i] = (*ptr);
				ptr++;
			}
		}

		analyser.processSamples( dataPerChannel, samples );
		
		samples = output.write( inpb, samples );
		
		cumulOfSamples += samples;
		callback( (float)cumulOfSamples / length * 100 );
	}
	
	for( size_t i = 0; i< channelsInBuffer; i++ )
	{
		delete limiters.at( i );
		delete[] dataPerChannel[i];
	}
	delete[] inpb;
}

#endif
