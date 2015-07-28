#ifndef PROCESS_FILE_H
#define PROCESS_FILE_H

#include <loudnessAnalyser/LoudnessAnalyser.hpp>
#include <tool/io/SoundFile.hpp>
#include "CorrectBuffer.hpp"
#include "LookAheadLimiter.hpp"

namespace Loudness {
namespace tool {

// Based class for functor which process audio file and fill LoudnessAnalyser
class Processor
{
public:
	Processor( Loudness::LoudnessAnalyser& analyser, Loudness::io::SoundFile& audioFile )
		: _inputAudioFile( audioFile )
		, _cumulOfSamples( 0 )
		, _totalNbSamples( _inputAudioFile.getNbSamples() )
		, _channelsInBuffer( std::min( 5, _inputAudioFile.getNbChannels() ) ) // skip last channel if 5.1 (LRE channel)
		, _bufferSize( _inputAudioFile.getSampleRate() / 5 )
		, _analyser( analyser )
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

	// Analyse the nbSamples in _data, and fill LoudnessAnalyser
	void processSamples( const size_t nbSamples )
	{
		float* p = _inpb;
		for( size_t i = 0; i < nbSamples; i++ )
		{
			for( size_t c = 0; c < _channelsInBuffer; c++ )
				_data [c][i] = (*p++);
		}
		_analyser.processSamples( _data, nbSamples );
	}

protected:
	Loudness::io::SoundFile& _inputAudioFile;

	size_t _cumulOfSamples;
	const size_t _totalNbSamples;
	const size_t _channelsInBuffer;
	const size_t _bufferSize;

	float * _inpb;  // input pointer buffer

private:
	Loudness::LoudnessAnalyser& _analyser;
	float** _data; // data to analyse loudness
};

// Functor to analyse audio file
class AnalyseFile : public Processor
{
public:
	AnalyseFile( Loudness::LoudnessAnalyser& analyser, Loudness::io::SoundFile& audioFile )
		: Processor( analyser, audioFile )
	{}

	void operator()( void (*callback)(int) )
	{
		// While we read samples
		while (true)
		{
			const size_t nbSamples = _inputAudioFile.read( _inpb, _bufferSize );
			if (nbSamples == 0) break;

			// Analyse input
			processSamples( nbSamples );

			// Callback for progression
			_cumulOfSamples += nbSamples;
			callback( (float)_cumulOfSamples / _totalNbSamples * 100 );
		}
	}
};

// Functor to correct audio file
class CorrectFile : public Processor
{
public:
	CorrectFile( Loudness::LoudnessAnalyser& analyser, Loudness::io::SoundFile& inputAudioFile, Loudness::io::SoundFile& outputAudioFile, const float gain )
		: Processor( analyser, inputAudioFile )
		, _outputAudioFile( outputAudioFile )
		, _gain( gain )
	{}

	void operator()( void (*callback)(int) )
	{
		while (true)
		{
			const size_t nbSamples = _inputAudioFile.read( _inpb, _bufferSize );
			if (nbSamples == 0) break;

			// Correct input
			float* p = _inpb;
			correctBuffer( p, nbSamples, _channelsInBuffer, _gain );

			// Analyse output
			processSamples( nbSamples );

			// Write output
			const size_t nbSamplesWritten = _outputAudioFile.write( _inpb, nbSamples );

			// Callback for progression
			_cumulOfSamples += nbSamplesWritten;
			callback( (float)_cumulOfSamples / _totalNbSamples * 100 );
		}
	}

protected:
	Loudness::io::SoundFile& _outputAudioFile;

	const float _gain;
};

// Functor to correct audio file with limiters
// @note: experimental class
class CorrectFileWithCompressor : public CorrectFile
{
public:
	CorrectFileWithCompressor( Loudness::LoudnessAnalyser& analyser, Loudness::io::SoundFile& inputAudioFile, Loudness::io::SoundFile& outputAudioFile, const float gain, const float lookAhead, const float threshold )
		: CorrectFile( analyser, inputAudioFile, outputAudioFile, gain )
		, _lookAhead( lookAhead )
		, _threshold( threshold )
	{
		for( size_t i = 0; i< _channelsInBuffer; i++ )
		{
			LookAheadLimiter* lim = new LookAheadLimiter( _lookAhead, _inputAudioFile.getSampleRate(), _threshold );
			_limiters.push_back( lim );
		}
	}

	~CorrectFileWithCompressor()
	{
		for( size_t i = 0; i < _channelsInBuffer; i++ )
		{
			delete _limiters.at( i );
		}
	}

	void operator()( void (*callback)(int) )
	{
		while (true)
		{
			const size_t nbSamples = _inputAudioFile.read( _inpb, _bufferSize );
			if( nbSamples == 0 ) break;

			// Correct input
			float* ptr = _inpb;
			size_t nbSamplesCorrected = correctBuffer( _limiters, ptr, nbSamples, _channelsInBuffer, _gain );

			// Analyse output
			processSamples( nbSamples );

			// Write output
			const size_t nbSamplesWritten = _outputAudioFile.write( _inpb, nbSamplesCorrected );

			// Callback for progression
			_cumulOfSamples += nbSamplesWritten;
			callback( (float)_cumulOfSamples / _totalNbSamples * 100 );
		}

		while (true)
		{
			float* ptr = _inpb;

			const size_t lastSamples = getLastData( _limiters, ptr, _bufferSize, _channelsInBuffer, _gain );
			if( lastSamples == 0 ) break;

			// Analyse output
			processSamples( lastSamples );

			// Write output
			const size_t lastSamplesWritten = _outputAudioFile.write( _inpb, lastSamples );

			// Callback for progression
			_cumulOfSamples += lastSamplesWritten;
			callback( (float)_cumulOfSamples / _totalNbSamples * 100 );
		}
	}

private:	
	std::vector<LookAheadLimiter*> _limiters;

	const float _lookAhead;
	const float _threshold;
};

}
}

#endif
