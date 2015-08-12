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
#include <iostream>
#include <algorithm>
#include <cstring>

int main( int argc, char** argv )
{
	if( argc != 2 )
	{
		std::cout << "completeAnalyser requires a media filename" << std::endl;
		return( -1 );
	}

	avtranscoder::preloadCodecsAndFormats();
	avtranscoder::Logger::setLogLevel(AV_LOG_DEBUG);

	// analyse inputFile
	avtranscoder::InputFile input( argv[1] );
	avtranscoder::NoDisplayProgress p;
	input.analyse( p, avtranscoder::eAnalyseLevelHeader );

	// display file properties
	std::cout << input;

	// Extract info from audio stream 0
	const size_t streamIndex = 0;

	avtranscoder::InputStream& inputStream = input.getStream(streamIndex);
	inputStream.activate();
	avtranscoder::AudioDecoder audioDecoder(inputStream);
	audioDecoder.setupDecoder();

	std::vector< avtranscoder::AudioProperties > audioProperties = input.getProperties().getAudioProperties();
	const int channels = audioProperties.at(streamIndex).getChannels();
	const size_t channelsToAnalyse = std::min(channels, 5);
	const size_t sampleRate = audioProperties.at(streamIndex).getSampleRate();

	// prepare conversion to float planar buffers
	avtranscoder::AudioFrame srcFrame(inputStream.getAudioCodec().getAudioFrameDesc());
	avtranscoder::AudioFrameDesc dstFrameDesc(sampleRate, 1, AV_SAMPLE_FMT_FLTP);
	avtranscoder::AudioFrame dstFrame(dstFrameDesc);
	avtranscoder::AudioTransform transform;

	// Get loudness analyser
	Loudness::analyser::LoudnessLevels level = Loudness::analyser::LoudnessLevels::Loudness_EBU_R128();
	Loudness::analyser::LoudnessAnalyser analyser(level);
	analyser.initAndStart(1, sampleRate);

	// Read audio stream
	float** audioBuffer = new float* [ channelsToAnalyse ];
	while( audioDecoder.decodeNextFrame(srcFrame) )
	{
		// Convert input to float planar sample format
		transform.convert(srcFrame, dstFrame);

		// Convert buffer to a float array[channel][samples]
		const int channelsToAnalyse = 2;
		const size_t sizePerChannel = dstFrame.getSize() / channelsToAnalyse;
		for(size_t channel = 0; channel < channelsToAnalyse; ++channel)
		{
			audioBuffer[channel] = (float*)(dstFrame.getData() + (sizePerChannel * channel));
		}

		// Analyse loudness
		analyser.processSamples(audioBuffer, dstFrame.getNbSamples());
	}

	// end
	analyser.printPloudValues();

	delete audioBuffer;
}
