#ifndef PROCESS_FILE_H
#define PROCESS_FILE_H

#include <loudnessAnalyser/LoudnessAnalyser.hpp>
#include <tool/io/SoundFile.hpp>

void processAnalyseFile( Loudness::LoudnessAnalyser& analyser, SoundFile& audioFile, void (*callback)(int) )
{
	int cumulOfSamples = 0;
	int length = audioFile.size();
	size_t channelsInBuffer = std::min( 5, audioFile.chan() );
	int bufferSize = audioFile.rate () / 5;
	float* data [ channelsInBuffer ];
	float* inpb = new float [ audioFile.chan() * bufferSize ];
	
	for( size_t i = 0; i< channelsInBuffer; i++ )
		data [i] = new float [bufferSize];
	
	analyser.initAndStart( channelsInBuffer, audioFile.rate() );
	
	audioFile.seek( 0 );
	
	while (true)
	{
		int  samples = audioFile.read( inpb, bufferSize );
		if (samples == 0) break;

		cumulOfSamples += samples;
		callback( (float)cumulOfSamples / length * 100 );
		float* p = inpb;
		for (int i = 0; i < samples; i++)
		{
			for(size_t c = 0; c < channelsInBuffer; c++ )
				data [c][i] = (*p++);
		}
		analyser.processSamples( data, samples );
	}
	
	delete[] inpb;
	for( size_t i=0; i< channelsInBuffer; i++ )
		delete[] data[i];
}

#endif
