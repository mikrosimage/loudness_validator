#ifndef AVCORRECTOR_HPP
#define AVCORRECTOR_HPP

#include <AvTranscoder/reader/AudioReader.hpp>

union Sample {
    int value;
    unsigned char bytes[4];
};

class AvCorrector
{
public:
    AvCorrector(const std::vector<avtranscoder::InputStreamDesc>& arrayToCorrect, const std::string& correctionOutputName = "");
    ~AvCorrector();

    void correct(const float gain);
    std::vector<avtranscoder::InputStreamDesc> getOutputStreamDescs() { return _correctedStreamDescs; }
    void setProgressionFile(const std::string& progressionFileName) { _progressionFileName = progressionFileName; }

private:
    int getSampleMaxValue(const std::string& sampleFormat);
    void printProgress();

    size_t correctFrame(unsigned char* srcData, unsigned char* dstData, const size_t nbChannels, const size_t nbSamples,
        const size_t bytesPerSample, const float gain);

    bool isEndOfCorrection();

private:
    // for progress
    size_t _totalNbSamplesToCorrect;
    size_t _cumulOfSamplesCorrected;
    std::vector<size_t> _nbSamplesToCorrect;

    // to check audio before correct
    std::vector<size_t> _inputNbChannels;
    std::vector<size_t> _inputSampleRate;

    int _currentReaderSamplesMaxValue;

    // To print the progession to a stream
    std::ostream* _outputStream;
    std::string _progressionFileName;

    // for io
    std::vector<avtranscoder::AudioReader*> _audioReader;
    std::vector<avtranscoder::InputStreamDesc> _correctedStreamDescs;
};

#endif
