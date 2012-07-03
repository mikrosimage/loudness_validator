
#include <fstream>
#include <SoundFile.h>
#include <LookAheadLimiter.h>

void writeCorrectedFile( SoundFile& input, SoundFile& output, const float gain, void (*callback)(int) )
{
	int bufferSize = input.rate () / 5;
	int length = input.size();
	size_t channelsInBuffer = input.chan();
	size_t cumulOfSamples = 0;
	
	float* data [ channelsInBuffer ];
	float* inpb  = new float [input.chan() * bufferSize];
	
	for( size_t i=0; i < channelsInBuffer; i++ )
		data [i] = new float [bufferSize];
	
	input.seek( 0 );
	
	while (true)
	{
		int  samples = input.read( inpb, bufferSize );
		if (samples == 0) break;
		
		float* ptr = inpb;
		for( size_t i = 0; i < samples * channelsInBuffer; i++, ptr++ )
		{
			*ptr = (*ptr) * gain;
		}
		
		samples = output.write( inpb, samples );
		cumulOfSamples += samples;
		callback( (float)cumulOfSamples / length * 100 );
	}
	
	for( size_t i=0; i< channelsInBuffer; i++ )
		delete[] data[i];
	delete[] inpb;
}

void writeCorrectedFile( SoundFile& input, SoundFile& output, const float gain, const float attackMs, const float releaseMs, float lookAhead, float threshold, void (*callback)(int) )
{
	int bufferSize = input.rate () / 5;
	int length = input.size();
	size_t channelsInBuffer = input.chan();
	size_t cumulOfSamples = 0;
	
	float* data [ channelsInBuffer ];
	float* inpb  = new float [input.chan() * bufferSize];
	
	for( size_t i=0; i < channelsInBuffer; i++ )
		data [i] = new float [bufferSize];
	
	input.seek( 0 );

	//Limiter limiter( attackMs, releaseMs, input.rate(), threshold, lookAhead );
	
	float inputGain = 1.0;
	float outputGain = 1.0;
	
	LookAheadLimiter limiter( lookAhead, inputGain, outputGain, releaseMs, input.rate(), threshold );
	
	while (true)
	{
		int  samples = input.read( inpb, bufferSize );
		if (samples == 0) break;
		
		float* ptr = inpb;
		for( size_t i = 0; i < samples * channelsInBuffer; i++, ptr++ )
		{
			*ptr = limiter.process( (*ptr) * gain );
		}
		
		samples = output.write( inpb, samples );
		cumulOfSamples += samples;
		callback( (float)cumulOfSamples / length * 100 );
	}
	
	for( size_t i=0; i< channelsInBuffer; i++ )
		delete[] data[i];
	delete[] inpb;
}


//std::pow ( 10, ( correctionGain ) / 20 )


