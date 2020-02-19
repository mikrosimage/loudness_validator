#ifndef AVSOUNDFILE_HPP
#define AVSOUNDFILE_HPP

#include <loudnessAnalyser/LoudnessAnalyser.hpp>

#include <AvTranscoder/reader/AudioReader.hpp>

#include <vector>
#include <utility>
#include <iostream>

/**
 * @brief Read and analyse the given audio elements.
 */
class AvSoundFile
{
public:
    AvSoundFile(const std::vector<avtranscoder::InputStreamDesc>& arrayToAnalyse);
    ~AvSoundFile();

    void analyse(Loudness::analyser::LoudnessAnalyser& analyser);

    /**
     * @brief Set the output filename of the progress file.
     * If empty, no file is created and the progress is printed to the console.
     */
    void setProgressionFile(const std::string& progressionFileName) { _progressionFileName = progressionFileName; }

    /**
     * @brief Force the analysis to be on a specific duration (in seconds)
     * @note By default we use the duration of the input.
     */
    void setDurationToAnalyse(const float durationToAnalyse);

    size_t getNbChannelsToAnalyse() const;

private:
    /**
     * @brief Print progress of analysis
     * If _progressionFileName si not empty, print to file _progressionFile
     * Else print to console
     */
    void printProgress();

    /**
     * @return If it is the end of the loudness analysis.
     */
    bool isEndOfAnalysis();

    /**
     * @brief Compute the total number of samples to analyse from the several inputs
     * sample rate, the number of channels and the expected duration.
     * @return the number of samples to analyse
     */
    size_t getTotalNbSamplesToAnalyse();

private:
    // for loudness analyser
    size_t _nbChannelsToAnalyse;

    // for progress
    size_t _totalNbSamplesToAnalyse;
    size_t _cumulOfSamplesAnalysed;

    // to check audio before analyse
    std::vector<size_t> _inputNbChannels;
    std::vector<size_t> _inputSampleRate;

    // for io
    std::vector<avtranscoder::AudioReader*> _audioReader;

    // To print the progession to a stream
    std::ostream* _outputStream;
    std::string _progressionFileName;

    // To force the duration to analyse
    float _forceDurationToAnalyse;
};

#endif
