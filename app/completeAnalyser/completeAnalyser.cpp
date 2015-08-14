#include <loudnessAnalyser/LoudnessAnalyser.hpp>

#include <AvTranscoder/file/InputFile.hpp>
#include <AvTranscoder/mediaProperty/AudioProperties.hpp>
#include <AvTranscoder/mediaProperty/print.hpp>
#include <AvTranscoder/progress/NoDisplayProgress.hpp>
#include <AvTranscoder/decoder/AudioDecoder.hpp>
#include <AvTranscoder/stream/InputStream.hpp>
#include <AvTranscoder/frame/AudioFrame.hpp>
#include <AvTranscoder/transform/AudioTransform.hpp>

#include <vector>
#include <utility>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <stdexcept>

void progress( int p )
{
	std::cout << "[" << std::setw(3) << p << "%]\r" << std::flush;
}


class AvSoudFiles
{
public:
	AvSoudFiles(const std::vector<std::pair<std::string, size_t> > arrayToAnalyse)
	: _nbChannelsToAnalyse(0)
	, _totalNbSamples(0)
	, _cumulOfSamples(0)
	{
		for(size_t fileIndex = 0; fileIndex < arrayToAnalyse.size(); ++fileIndex)
		{
			const std::string filename(arrayToAnalyse.at(fileIndex).first);
			const size_t streamIndex(arrayToAnalyse.at(fileIndex).second);

			// Analyse input file
			avtranscoder::InputFile* inputFile = new avtranscoder::InputFile( filename );
			_inputFiles.push_back(inputFile);
			avtranscoder::NoDisplayProgress p;
			inputFile->analyse( p, avtranscoder::eAnalyseLevelHeader );

			// display file properties
			std::cout << *inputFile;

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

	void analyse(Loudness::analyser::LoudnessAnalyser& analyser)
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

	~AvSoudFiles()
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

private:
	// for loudness analyser
	size_t _nbChannelsToAnalyse;

	// for progress
	size_t _totalNbSamples;
	size_t _cumulOfSamples;

	// to check audio before analyse
	std::vector<size_t> _inputNbChannels;
	std::vector<size_t> _inputNbSamples;
	std::vector<size_t> _inputSampleRate;

	// for io
	std::vector<avtranscoder::InputFile*> _inputFiles;
	std::vector<avtranscoder::AudioDecoder*> _decoders;
	std::vector<avtranscoder::AudioFrame*> _srcFrames;
	std::vector<avtranscoder::AudioFrame*> _dstFrames;
	std::vector<avtranscoder::AudioTransform*> _transforms;
};

std::vector<std::pair<std::string, size_t> > parseConfigFile( const std::string& configFilename )
{
	std::vector<std::pair<std::string, size_t> > result;

	std::ifstream configFile( configFilename.c_str(), std::ifstream::in );

	std::string line;
	while( std::getline( configFile, line ) )
	{
		std::istringstream is_line( line );
		std::string filename;
		if( std::getline( is_line, filename, '=' ) )
		{
			std::string streamId;
			if( std::getline( is_line, streamId, ':' ) )
			{
				std::string transcodeProfile;
				std::getline( is_line, transcodeProfile );

				std::stringstream ss( streamId );
				size_t streamIndex = 0;
				ss >> streamIndex;

				result.push_back(std::make_pair(filename, streamIndex));
			}
		}
	}

	configFile.close();

	return result;
}

int main( int argc, char** argv )
{
	std::vector< std::string > arguments;
	for( int argument = 1; argument < argc; ++argument )
	{
		arguments.push_back( argv[argument] );
	}

	for( size_t argument = 0; argument < arguments.size(); ++argument )
	{
		if( arguments.at( argument ) == "--help" )
		{
			std::string help;
			help += "Usage\n";
			help += "\tanalyser-complete CONFIG.TXT [--help]\n";
			help += "CONFIG.TXT\n";
			help += "\tEach line will be one audio stream analysed by the loudness library.\n";
			help += "\tPattern of each line is:\n";
			help += "\t[inputFile]=STREAM_ID\n";
			help += "Command line options\n";
			help += "\t--help: display this help\n";
			std::cout << help << std::endl;
			return 0;
		}
	}

	// Check required arguments
	if( argc < 2 )
	{
		std::cout << "completeAnalyser requires a media filename" << std::endl;
		std::cout << "Use option --help to display help" << std::endl; 
		return( -1 );
	}

	avtranscoder::preloadCodecsAndFormats();
	avtranscoder::Logger::setLogLevel(AV_LOG_QUIET);

	// Get list of files / streamIndex to analyse
	std::vector<std::pair<std::string, size_t> > arrayToAnalyse = parseConfigFile(arguments.at(0));
	AvSoudFiles soudFiles(arrayToAnalyse);

	// Analyse loudness according to EBU R-128
	Loudness::analyser::LoudnessLevels level = Loudness::analyser::LoudnessLevels::Loudness_EBU_R128();
	Loudness::analyser::LoudnessAnalyser analyser(level);
	soudFiles.analyse(analyser);

	// Print analyse
	analyser.printPloudValues();
}
