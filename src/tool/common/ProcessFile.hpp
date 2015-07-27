#ifndef PROCESS_FILE_H
#define PROCESS_FILE_H

#include <loudnessAnalyser/LoudnessAnalyser.hpp>
#include <tool/io/SoundFile.hpp>

// Based class for functor which process audio file and fill LoudnessAnalyser
class Processor
{
public:
	Processor( Loudness::LoudnessAnalyser& analyser, SoundFile& audioFile )
		: _analyser( analyser )
		, _inputAudioFile( audioFile )
		, _cumulOfSamples( 0 )
		, _totalNbSamples( _inputAudioFile.getNbSamples() )
		, _channelsInBuffer( std::min( 5, _inputAudioFile.getNbChannels() ) ) // skip last channel if 5.1 (LRE channel)
		, _bufferSize( _inputAudioFile.getSampleRate() / 5 )
	{
		_data = new float* [ _channelsInBuffer ];
		_inpb = new float [ _inputAudioFile.getNbChannels() * _bufferSize ];

		for( size_t i = 0; i< _channelsInBuffer; i++ )
			_data [i] = new float [ _bufferSize ];

		// Init structures of analysis and seek at the beginning of the file
		_analyser.initAndStart( _channelsInBuffer, _inputAudioFile.getSampleRate() );
		_inputAudioFile.seek( 0 );
	}
	
	~Processor()
	{
		delete[] _inpb;
		for( size_t i = 0; i < _channelsInBuffer; i++ )
			delete[] _data[i];
	}

protected:
	Loudness::LoudnessAnalyser& _analyser;
	SoundFile& _inputAudioFile;

	size_t _cumulOfSamples;
	const size_t _totalNbSamples;
	const size_t _channelsInBuffer;
	const size_t _bufferSize;

	// data
	float** _data;
	float * _inpb;
};

// Functor to analyse audio file
class AnalyseFile : public Processor
{
public:
	AnalyseFile( Loudness::LoudnessAnalyser& analyser, SoundFile& audioFile )
		: Processor( analyser, audioFile )
	{}

	void operator()( void (*callback)(int) )
	{
		// While we read samples
		while (true)
		{
			const size_t nbSamples = _inputAudioFile.read( _inpb, _bufferSize );
			if (nbSamples == 0) break;

			float* p = _inpb;
			for( size_t i = 0; i < nbSamples; i++ )
			{
				for( size_t c = 0; c < _channelsInBuffer; c++ )
					_data [c][i] = (*p++);
			}
			_analyser.processSamples( _data, nbSamples );

			// Callback for progression
			_cumulOfSamples += nbSamples;
			callback( (float)_cumulOfSamples / _totalNbSamples * 100 );
		}
	}
};

#endif
