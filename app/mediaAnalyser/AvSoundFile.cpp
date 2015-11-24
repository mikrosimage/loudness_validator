#include "AvSoundFile.hpp"

#include <AvTranscoder/mediaProperty/print.hpp>

#include <iomanip>
#include <iostream>
#include <algorithm>
#include <stdexcept>


void progress( int p )
{
	std::cout << "[" << std::setw(3) << p << "%]\r" << std::flush;
}

AvSoundFile::AvSoundFile(const std::vector<std::pair<std::string, size_t> >& arrayToAnalyse)
	: _nbChannelsToAnalyse(0)
	, _totalNbSamples(0)
	, _cumulOfSamples(0)
{
	for(size_t fileIndex = 0; fileIndex < arrayToAnalyse.size(); ++fileIndex)
	{
		const std::string filename(arrayToAnalyse.at(fileIndex).first);
		const size_t streamIndex(arrayToAnalyse.at(fileIndex).second);

		// Analyse input file
		avtranscoder::InputFile* inputFile = NULL;
		std::vector<std::string>::iterator iterFilename = std::find(_inputFilenames.begin(), _inputFilenames.end(), filename);
		if(iterFilename != _inputFilenames.end())
		{
			// get existing InputFile
			const size_t filenameIndex = std::distance( _inputFilenames.begin(), iterFilename);
			inputFile = _inputFiles.at(filenameIndex);
		}
		else
		{
			// create new InputFile
			inputFile = new avtranscoder::InputFile( filename );
			_inputFilesAllocated.push_back(inputFile);

			// display file properties
			std::cout << *inputFile;

			// add to list of filename
			_inputFilenames.push_back(filename);
		}
		_inputFiles.push_back(inputFile);

		// Create reader to convert to float planar
		avtranscoder::AudioReader* reader = new avtranscoder::AudioReader( *inputFile, streamIndex, 0, 0, "fltp" );
		_audioReader.push_back(reader);

		// Get data from audio stream
		const avtranscoder::AudioProperties* audioProperties = reader->getAudioProperties();
		const size_t nbSamples = audioProperties->getNbSamples();
		_inputNbSamples.push_back(nbSamples);
		_totalNbSamples += nbSamples;
		const int nbChannels = audioProperties->getChannels();
		_inputNbChannels.push_back(nbChannels);
		const size_t sampleRate = audioProperties->getSampleRate();
		_inputSampleRate.push_back(sampleRate);
	}

	// Check the given configuration
	bool nbChannelsAreEqual  = true;
	bool nbSamplesAreEqual  = true;
	bool sampleRateAreEqual  = true;
	const size_t nbChannels = _inputNbChannels.at(0);
	const size_t nbSamples = _inputNbSamples.at(0);
	const size_t sampleRate = _inputSampleRate.at(0);
	for(size_t i = 1; i < _inputNbChannels.size(); i++)
	{
		// check number of channels
		if(_inputNbChannels.at(i) != nbChannels)
			nbChannelsAreEqual = false;
		//  check number of samples
		if(_inputNbSamples.at(i) != nbSamples)
			nbSamplesAreEqual = false;
		// check sample rate
		if(_inputSampleRate.at(i) != sampleRate)
			sampleRateAreEqual = false;
	}

	if(!nbChannelsAreEqual || !nbSamplesAreEqual || !sampleRateAreEqual)
	{
		std::string msg  = "The given audio configuration isn't supported by the application.\n";
		msg += "Only audio stream with same sample rate, same number of samples, and same duration are supported.\n";
		msg += "Error:\n";
		if (  nbChannelsAreEqual == false ) msg += "- Number of channels are not equals\n";
		if (  nbSamplesAreEqual == false ) msg += "- Number of samples are not equal\n";
		if (  sampleRateAreEqual == false ) msg += "- Sampling rate are not equals\n";

		throw std::runtime_error(msg);
	}

	// Get number of channels to analyse
	int totalInputNbChannels = 0;
	for(size_t i = 0; i < _inputNbChannels.size(); ++i)
		totalInputNbChannels += _inputNbChannels.at(i);
	_nbChannelsToAnalyse = std::min(totalInputNbChannels, 5); // skip LRE
}

AvSoundFile::~AvSoundFile()
{
	for( std::vector< avtranscoder::InputFile* >::iterator it = _inputFilesAllocated.begin(); it != _inputFilesAllocated.end(); ++it )
	{
		delete (*it);
	}
	for( std::vector< avtranscoder::AudioReader* >::iterator it = _audioReader.begin(); it != _audioReader.end(); ++it )
	{
		delete (*it);
	}
}

void AvSoundFile::analyse(Loudness::analyser::LoudnessAnalyser& analyser)
{
	// init
	analyser.initAndStart(_nbChannelsToAnalyse, _inputSampleRate.at(0));

	// Create planar buffer of float data
	float** audioBuffer = new float* [ _nbChannelsToAnalyse ];

	// Decode audio streams
	while(_cumulOfSamples < _totalNbSamples)
	{
		size_t nbSamplesRead = 0;
		size_t nbInputChannelAdded = 0;
		for(size_t fileIndex = 0; fileIndex < _audioReader.size(); ++fileIndex)
		{
			avtranscoder::AudioFrame* dstFrame = static_cast<avtranscoder::AudioFrame*>(_audioReader.at(fileIndex)->readNextFrame());

			// Convert buffer to a float array[channel][samples]
			const size_t sizePerChannel = dstFrame->getSize() / _inputNbChannels.at(fileIndex);
			size_t inputChannel = 0;
			for(size_t channelToAdd = nbInputChannelAdded; channelToAdd < nbInputChannelAdded + _inputNbChannels.at(fileIndex); ++channelToAdd)
			{
				audioBuffer[channelToAdd] = (float*)(dstFrame->getData() + (sizePerChannel * inputChannel));
				++inputChannel;
			}
			nbSamplesRead += dstFrame->getNbSamples();
			nbInputChannelAdded += _inputNbChannels.at(fileIndex);
		}

		// Analyse loudness
		const size_t nbSamplesInOneFrame = nbSamplesRead / _audioReader.size();
		analyser.processSamples(audioBuffer, nbSamplesInOneFrame);

		// Progress
		_cumulOfSamples += nbSamplesRead;
		progress( (float)_cumulOfSamples / _totalNbSamples * 100 );
	}

	// free audio buffer
	delete audioBuffer;
}
