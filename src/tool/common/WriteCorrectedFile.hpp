#ifndef WRITE_CORRECTED_FILE_H
#define WRITE_CORRECTED_FILE_H

#include <fstream>
#include <vector>
#include <SoundFile.hpp>
#include <CorrectBuffer.hpp>
#include <LookAheadLimiter.hpp>

void writeCorrectedFile( Loudness::LoudnessAnalyser& analyser, SoundFile& input, SoundFile& output, const float gain, void (*callback)(int) )
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

void writeCorrectedFile( Loudness::LoudnessAnalyser& analyser, SoundFile& input, SoundFile& output, const float gain, float lookAhead, float threshold, void (*callback)(int) )
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
		size_t samples = input.read( inpb, bufferSize );
		if( samples == 0 ) break;
		
		float* ptr = inpb;
		
		//samples = 
		size_t s = correctBuffer( limiters, ptr, samples, channelsInBuffer, gain );
		
		// re-analyse output
		ptr = inpb;
		
		for( size_t i = 0; i < samples; i++ )
		{
			for( size_t c = 0; c < channelsInBuffer; c++ )
			{
				dataPerChannel [c][i] = (*ptr);
				ptr++;
			}
		}

		analyser.processSamples( dataPerChannel, s );
		
		samples = output.write( inpb, s );
		
		cumulOfSamples += samples;
		callback( (float)cumulOfSamples / length * 100 );
	}
	
	while (true)
	{
		float* ptr = inpb;
		
		//samples = 
		size_t s = getLastData( limiters, ptr, bufferSize, channelsInBuffer, gain );
		
		if( s == 0 ) break;
		// re-analyse output
		ptr = inpb;
		
		for( size_t i = 0; i < s; i++ )
		{
			for( size_t c = 0; c < channelsInBuffer; c++ )
			{
				dataPerChannel [c][i] = (*ptr);
				ptr++;
			}
		}

		analyser.processSamples( dataPerChannel, s );
		
		s = output.write( inpb, s );
		
		cumulOfSamples += s;
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
