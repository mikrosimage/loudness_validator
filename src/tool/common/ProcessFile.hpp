#ifndef PROCESS_FILE_H
#define PROCESS_FILE_H

#include <loudnessAnalyser/LoudnessAnalyser.hpp>
#include <tool/io/SoundFile.hpp>

void processAnalyseFile( Loudness::LoudnessAnalyser& analyser, SoundFile& audioFile, void (*callback)(int) )
{
	// Allocate buffers
	unsigned int cumulOfSamples = 0;
	const unsigned int length = audioFile.size();
	const size_t channelsInBuffer = std::min( 5, audioFile.chan() ); // skip last channel if 5.1 (LRE channel)
	const int bufferSize = audioFile.rate() / 5;
	float* data [ channelsInBuffer ];
	float* inpb = new float [ audioFile.chan() * bufferSize ];
	
	for( size_t i = 0; i< channelsInBuffer; i++ )
		data [i] = new float [bufferSize];
	
	// Init structures of analysis and seek at the beginning of the file
	analyser.initAndStart( channelsInBuffer, audioFile.rate() );
	audioFile.seek( 0 );
	
	// While we read samples, analyse them
	while (true)
	{
		const unsigned int nbSamples = audioFile.read( inpb, bufferSize );
		if (nbSamples == 0) break;

		cumulOfSamples += nbSamples;
		callback( (float)cumulOfSamples / length * 100 );
		float* p = inpb;
		for( size_t i = 0; i < nbSamples; i++ )
		{
			for( size_t c = 0; c < channelsInBuffer; c++ )
				data [c][i] = (*p++);
		}
		analyser.processSamples( data, nbSamples );
	}

	// Desallocate buffers
	delete[] inpb;
	for( size_t i=0; i< channelsInBuffer; i++ )
		delete[] data[i];
}

#endif
