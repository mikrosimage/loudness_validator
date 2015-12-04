#ifndef AVSOUNDFILE_HPP
#define AVSOUNDFILE_HPP

#include <loudnessAnalyser/LoudnessAnalyser.hpp>

#include <AvTranscoder/reader/AudioReader.hpp>

#include <vector>
#include <utility>
#include <iostream>


class AvSoundFile
{
public:
	AvSoundFile(const std::vector<std::pair<std::string, size_t> >& arrayToAnalyse);
	~AvSoundFile();

	void analyse(Loudness::analyser::LoudnessAnalyser& analyser);

	/**
	 * @brief Set the output filename of the progress file.
	 * If empty, no file is created and the progress is printed to the console.
	 */
	void setProgressionFile( const std::string& progressionFileName ) { _progressionFileName = progressionFileName; }

private:
	/**
	 * @brief Print progress of analysis
	 * If _progressionFileName si not empty, print to file _progressionFile
	 * Else print to console
	 */
	void printProgress( const int p );

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
	/**
	 * The boolean indicates if the InputFile is allocated by the class (has ownership)
	 */
	std::vector< std::pair<avtranscoder::InputFile*, bool> > _inputFiles;
	std::vector<avtranscoder::AudioReader*> _audioReader;

	// To print the progession to a stream
	std::ostream* _outputStream;
	std::string _progressionFileName;
};

#endif
