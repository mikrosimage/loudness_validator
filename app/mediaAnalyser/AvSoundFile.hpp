#ifndef AVSOUNDFILE_HPP
#define AVSOUNDFILE_HPP

#include <loudnessAnalyser/LoudnessAnalyser.hpp>

#include <AvTranscoder/file/InputFile.hpp>
#include <AvTranscoder/mediaProperty/AudioProperties.hpp>
#include <AvTranscoder/progress/NoDisplayProgress.hpp>
#include <AvTranscoder/decoder/AudioDecoder.hpp>
#include <AvTranscoder/stream/InputStream.hpp>
#include <AvTranscoder/frame/AudioFrame.hpp>
#include <AvTranscoder/transform/AudioTransform.hpp>

#include <vector>
#include <utility>


class AvSoundFile
{
public:
	AvSoundFile(const std::vector<std::pair<std::string, size_t> >& arrayToAnalyse);
	~AvSoundFile();

	void analyse(Loudness::analyser::LoudnessAnalyser& analyser);

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
	std::vector<std::string> _inputFilenames;
	std::vector<avtranscoder::InputFile*> _inputFiles;
	std::vector<avtranscoder::AudioDecoder*> _decoders;
	std::vector<avtranscoder::AudioFrame*> _srcFrames;
	std::vector<avtranscoder::AudioFrame*> _dstFrames;
	std::vector<avtranscoder::AudioTransform*> _transforms;
};

#endif
