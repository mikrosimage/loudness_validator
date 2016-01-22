#include "AvSoundFile.hpp"

#include <iomanip>
#include <fstream>
#include <algorithm>
#include <stdexcept>


void AvSoundFile::printProgress( const int p )
{
	// print progression to file
	if( ! _progressionFileName.empty() )
	{
		_outputStream->seekp(0);
		*_outputStream << p;
	}
	// print progression to console
	else
		*_outputStream << "[" << std::setw(3) << p << "%]\r" << std::flush;
}

AvSoundFile::AvSoundFile(const std::vector<std::pair<std::string, size_t> >& arrayToAnalyse)
	: _nbChannelsToAnalyse(0)
	, _totalNbSamples(0)
	, _cumulOfSamples(0)
	, _outputStream(&std::cout)
	, _progressionFileName()
{
	for(size_t fileIndex = 0; fileIndex < arrayToAnalyse.size(); ++fileIndex)
	{
		const std::string filename(arrayToAnalyse.at(fileIndex).first);
		const size_t streamIndex(arrayToAnalyse.at(fileIndex).second);

		// Analyse input file
		avtranscoder::InputFile* inputFile = NULL;
		std::vector<std::string>::iterator iterFilename = std::find(_inputFilenames.begin(), _inputFilenames.end(), filename);
		const bool isAlreadyAllocated = (iterFilename != _inputFilenames.end());
		if(isAlreadyAllocated)
		{
			// get existing InputFile
			const size_t filenameIndex = std::distance( _inputFilenames.begin(), iterFilename);
			inputFile = _inputFiles.at(filenameIndex).first;
		}
		else
		{
			// create new InputFile
			inputFile = new avtranscoder::InputFile( filename );

			// display file properties
			//std::cout << *inputFile;

			// add to list of filename
			_inputFilenames.push_back(filename);
		}
		_inputFiles.push_back(std::make_pair(inputFile, ! isAlreadyAllocated));

		// Create reader to convert to float planar
		avtranscoder::AudioReader* reader = new avtranscoder::AudioReader( *inputFile, streamIndex );
		_audioReader.push_back(reader);

		// Get data from audio stream
		const avtranscoder::AudioProperties* audioProperties = reader->getSourceAudioProperties();
		const size_t nbSamples = audioProperties->getNbSamples();
		_inputNbSamples.push_back(nbSamples);
		_totalNbSamples += nbSamples;
		const int nbChannels = audioProperties->getNbChannels();
		_inputNbChannels.push_back(nbChannels);
		const size_t sampleRate = audioProperties->getSampleRate();
		_inputSampleRate.push_back(sampleRate);

		// Update output of reader
		reader->updateOutput(sampleRate, nbChannels, "fltp");
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
	for( std::vector< std::pair<avtranscoder::InputFile*, bool> >::iterator it = _inputFiles.begin(); it != _inputFiles.end(); ++it )
	{
		// if the input file was allocated
		if( it->second )
			delete it->first;
	}
	for( std::vector< avtranscoder::AudioReader* >::iterator it = _audioReader.begin(); it != _audioReader.end(); ++it )
	{
		delete (*it);
	}
}

void AvSoundFile::analyse(Loudness::analyser::LoudnessAnalyser& analyser)
{
	// open file to print duration
	std::ofstream outputFile;
	if( ! _progressionFileName.empty() )
	{
		outputFile.open(_progressionFileName.c_str());
		_outputStream = &outputFile;
	}

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
			size_t inputChannel = 0;
			for(size_t channelToAdd = nbInputChannelAdded; channelToAdd < nbInputChannelAdded + _inputNbChannels.at(fileIndex); ++channelToAdd)
			{
				audioBuffer[channelToAdd] = (float*)(dstFrame->getData()[inputChannel]);
				++inputChannel;
				nbSamplesRead += dstFrame->getNbSamplesPerChannel();
			}
			nbInputChannelAdded += _inputNbChannels.at(fileIndex);
		}

		// Analyse loudness
		const size_t nbSamplesInOneFrame = nbSamplesRead / _audioReader.size();
		analyser.processSamples(audioBuffer, nbSamplesInOneFrame);

		// Progress
		_cumulOfSamples += nbSamplesRead;
		printProgress( (float)_cumulOfSamples / _totalNbSamples * 100 );
	}

	// Close progression file
	outputFile.close();

	// free audio buffer
	delete audioBuffer;
}
