#include "AvSoudFile.hpp"

#include <AvTranscoder/mediaProperty/print.hpp>

#include <iostream>
#include <algorithm>
#include <stdexcept>


void progress( int p )
{
	std::cout << "[" << std::setw(3) << p << "%]\r" << std::flush;
}

AvSoudFile::AvSoudFile(const std::vector<std::pair<std::string, size_t> >& arrayToAnalyse)
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

			// display file properties
			std::cout << *inputFile;

			// add to list of filename
			_inputFilenames.push_back(filename);
		}
		_inputFiles.push_back(inputFile);
		avtranscoder::NoDisplayProgress p;
		inputFile->analyse( p, avtranscoder::eAnalyseLevelHeader );

		// Set up decoder of the audio stream
		avtranscoder::InputStream& inputStream = inputFile->getStream(streamIndex);
		inputStream.activate();
		avtranscoder::AudioDecoder* audioDecoder = new avtranscoder::AudioDecoder(inputStream);
		_decoders.push_back(audioDecoder);
		audioDecoder->setupDecoder();

		// Get data from audio stream
		const avtranscoder::AudioProperties& audioProperties = static_cast<const avtranscoder::AudioProperties&>(inputFile->getProperties().getStreamPropertiesWithIndex(inputStream.getStreamIndex()));
		const size_t nbSamples = audioProperties.getNbSamples();
		_inputNbSamples.push_back(nbSamples);
		_totalNbSamples += nbSamples;
		const int nbChannels = audioProperties.getChannels();
		_inputNbChannels.push_back(nbChannels);
		const size_t sampleRate = audioProperties.getSampleRate();
		_inputSampleRate.push_back(sampleRate);

		// Describe source frame
		avtranscoder::AudioFrame* srcFrame = new avtranscoder::AudioFrame(inputStream.getAudioCodec().getAudioFrameDesc());
		_srcFrames.push_back(srcFrame);
		// Describe dest frame (float & planar)
		avtranscoder::AudioFrameDesc dstFrameDesc(sampleRate, nbChannels, AV_SAMPLE_FMT_FLTP);
		avtranscoder::AudioFrame* dstFrame = new avtranscoder::AudioFrame(dstFrameDesc);
		_dstFrames.push_back(dstFrame);
		// Create transform
		avtranscoder::AudioTransform* transform = new avtranscoder::AudioTransform();
		_transforms.push_back(transform);
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

AvSoudFile::~AvSoudFile()
{
	for( std::vector< avtranscoder::InputFile* >::iterator it = _inputFiles.begin(); it != _inputFiles.end(); ++it )
	{
		delete (*it);
	}
	for( std::vector< avtranscoder::AudioDecoder* >::iterator it = _decoders.begin(); it != _decoders.end(); ++it )
	{
		delete (*it);
	}
	for( std::vector< avtranscoder::AudioFrame* >::iterator it = _srcFrames.begin(); it != _srcFrames.end(); ++it )
	{
		delete (*it);
	}
	for( std::vector< avtranscoder::AudioFrame* >::iterator it = _dstFrames.begin(); it != _dstFrames.end(); ++it )
	{
		delete (*it);
	}
	for( std::vector< avtranscoder::AudioTransform* >::iterator it = _transforms.begin(); it != _transforms.end(); ++it )
	{
		delete (*it);
	}
}

void AvSoudFile::analyse(Loudness::analyser::LoudnessAnalyser& analyser)
{
	// init
	analyser.initAndStart(_nbChannelsToAnalyse, _inputSampleRate.at(0));

	// Create planar buffer of float data
	float** audioBuffer = new float* [ _nbChannelsToAnalyse ];

	// Decode audio streams
	bool frameDecoded = true;
	while(frameDecoded)
	{
		size_t totalNbSamples = 0;
		size_t nbInputChannelAdded = 0;
		for(size_t fileIndex = 0; fileIndex < _decoders.size(); ++fileIndex)
		{
			frameDecoded = _decoders.at(fileIndex)->decodeNextFrame(*_srcFrames.at(fileIndex));
			if(!frameDecoded)
				break;

			// Convert input to float planar sample format
			_transforms.at(fileIndex)->convert(*_srcFrames.at(fileIndex), *_dstFrames.at(fileIndex));

			// Convert buffer to a float array[channel][samples]
			const size_t sizePerChannel = _dstFrames.at(fileIndex)->getSize() / _inputNbChannels.at(fileIndex);
			size_t inputChannel = 0;
			for(size_t channelToAdd = nbInputChannelAdded; channelToAdd < nbInputChannelAdded + _inputNbChannels.at(fileIndex); ++channelToAdd)
			{
				audioBuffer[channelToAdd] = (float*)(_dstFrames.at(fileIndex)->getData() + (sizePerChannel * inputChannel));
				++inputChannel;
			}
			totalNbSamples += _dstFrames.at(fileIndex)->getNbSamples();
			nbInputChannelAdded += _inputNbChannels.at(fileIndex);
		}

		if(!frameDecoded)
			break;

		// Analyse loudness
		analyser.processSamples(audioBuffer, _dstFrames.at(0)->getNbSamples());

		// Progress
		_cumulOfSamples += totalNbSamples;
		progress( (float)_cumulOfSamples / _totalNbSamples * 100 );
	}

	// free audio buffer
	delete audioBuffer;
}
