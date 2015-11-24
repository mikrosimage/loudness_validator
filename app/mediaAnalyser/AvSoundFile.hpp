#ifndef AVSOUNDFILE_HPP
#define AVSOUNDFILE_HPP

#include <loudnessAnalyser/LoudnessAnalyser.hpp>

#include <AvTranscoder/reader/AudioReader.hpp>

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
	std::vector<avtranscoder::InputFile*> _inputFiles;  //< All input files
	std::vector<avtranscoder::InputFile*> _inputFilesAllocated;  //< All input files allocated by the class (has ownership))
	std::vector<avtranscoder::AudioReader*> _audioReader;
};

#endif
