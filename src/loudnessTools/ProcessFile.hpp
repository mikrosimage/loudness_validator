#ifndef _LOUDNESS_TOOLS_PROCESS_FILE_HPP_
#define _LOUDNESS_TOOLS_PROCESS_FILE_HPP_

#include <loudnessCommon/common.hpp>

#include <loudnessAnalyser/LoudnessAnalyser.hpp>

#include <loudnessTools/io/SoundFile.hpp>

#include <loudnessCorrector/CorrectBuffer.hpp>
#include <loudnessCorrector/LookAheadLimiter.hpp>

namespace Loudness {
namespace tools {

// Based class for functor which process audio file and fill LoudnessAnalyser
class LoudnessExport Processor
{
public:
	Processor( Loudness::analyser::LoudnessAnalyser& analyser, Loudness::tools::SoundFile& audioFile )
		: _inputAudioFile( audioFile )
		, _cumulOfSamples( 0 )
		, _totalNbSamples( _inputAudioFile.getNbSamples() )
		, _channelsInBuffer( std::min( 5, _inputAudioFile.getNbChannels() ) ) // skip last channel if 5.1 (LRE channel)
		, _bufferSize( _inputAudioFile.getSampleRate() / 5 )
		, _enableOptimization( true )
		, _analyser( analyser )
	{
		_data = new float* [ _channelsInBuffer ];
		_inpb = new float [ _inputAudioFile.getNbChannels() * _bufferSize ];

		for( size_t i = 0; i< _channelsInBuffer; i++ )
			_data [i] = new float [ _bufferSize ];

		init();
	}

	~Processor()
	{
		delete[] _inpb;
		for( size_t i = 0; i < _channelsInBuffer; i++ )
			delete[] _data[i];
	}

	void init()
	{
		// Init structures of analysis and seek at the beginning of the file
		_analyser.initAndStart( _channelsInBuffer, _inputAudioFile.getSampleRate(), _enableOptimization );
		_inputAudioFile.seek( 0 );
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

	void enableOptimization( const bool enableOptimization = true )
	{
		_enableOptimization = enableOptimization;
		init();

	}

protected:
	Loudness::tools::SoundFile& _inputAudioFile;

	size_t _cumulOfSamples;
	const size_t _totalNbSamples;
	const size_t _channelsInBuffer;
	const size_t _bufferSize;

	bool _enableOptimization;  // if true, use SIMD instructions

	float * _inpb;  // input pointer buffer

private:
	Loudness::analyser::LoudnessAnalyser& _analyser;
	float** _data; // data to analyse loudness
};

// Functor to analyse audio file
class AnalyseFile : public Processor
{
public:
	AnalyseFile( Loudness::analyser::LoudnessAnalyser& analyser, Loudness::tools::SoundFile& audioFile )
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
	CorrectFile( Loudness::analyser::LoudnessAnalyser& analyser, Loudness::tools::SoundFile& inputAudioFile, Loudness::tools::SoundFile& outputAudioFile, const float gain )
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
			corrector::correctBuffer( p, nbSamples, _channelsInBuffer, _gain );

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
	Loudness::tools::SoundFile& _outputAudioFile;

	const float _gain;
};

// Functor to correct audio file with limiters
// @note: experimental class
class CorrectFileWithCompressor : public CorrectFile
{
public:
	CorrectFileWithCompressor( Loudness::analyser::LoudnessAnalyser& analyser, Loudness::tools::SoundFile& inputAudioFile, Loudness::tools::SoundFile& outputAudioFile, const float gain, const float lookAhead, const float threshold )
		: CorrectFile( analyser, inputAudioFile, outputAudioFile, gain )
		, _lookAhead( lookAhead )
		, _threshold( threshold )
	{
		for( size_t i = 0; i< _channelsInBuffer; i++ )
		{
			corrector::LookAheadLimiter* lim = new corrector::LookAheadLimiter( _lookAhead, _inputAudioFile.getSampleRate(), _threshold );
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
			size_t nbSamplesCorrected = corrector::correctBuffer( _limiters, ptr, nbSamples, _channelsInBuffer, _gain );

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
	std::vector<corrector::LookAheadLimiter*> _limiters;

	const float _lookAhead;
	const float _threshold;
};

}
}

#endif
