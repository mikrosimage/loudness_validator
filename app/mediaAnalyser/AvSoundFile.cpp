#include "AvSoundFile.hpp"

#include <iomanip>
#include <fstream>
#include <algorithm>
#include <stdexcept>

#include <AvTranscoder/encoder/AudioEncoder.hpp>
#include <AvTranscoder/file/OutputFile.hpp>
#include <AvTranscoder/transform/AudioTransform.hpp>

const size_t NB_OF_BYTES_24_BITS = 3;
const float MAX_VALUE_24_BITS = 8388607.0;
const std::string CODEC_NAME_24_BITS = "pcm_s24le";
const std::string SAMPLE_FORMAT_24_BITS = "s32";

void AvSoundFile::printProgress()
{
    const int p = (float)_cumulOfSamplesAnalysed / _totalNbSamplesToAnalyse * 100;

    // print progression to file
    if(!_progressionFileName.empty())
    {
        _outputStream->seekp(0);
        *_outputStream << p;
    }
    // print progression to console
    else
        *_outputStream << "[" << std::setw(3) << p << "%]\r" << std::flush;
}

bool AvSoundFile::isEndOfAnalysis()
{
    return _cumulOfSamplesAnalysed >= _totalNbSamplesToAnalyse;
}

AvSoundFile::AvSoundFile(const std::vector<avtranscoder::InputStreamDesc>& arrayToAnalyse)
    : _nbChannelsToAnalyse(0)
    , _totalNbSamplesToAnalyse(0)
    , _cumulOfSamplesAnalysed(0)
    , _inputNbChannels()
    , _inputSampleRate()
    , _audioReader()
    , _outputStream(&std::cout)
    , _progressionFileName()
    , _forceDurationToAnalyse(0)
{
    for(std::vector<avtranscoder::InputStreamDesc>::const_iterator it = arrayToAnalyse.begin(); it != arrayToAnalyse.end(); ++it)
    {
        // Create reader to convert to float planar
        avtranscoder::AudioReader* reader = new avtranscoder::AudioReader(*it);
        reader->continueWithGenerator();
        _audioReader.push_back(reader);

        // Get data from audio stream
        const avtranscoder::AudioProperties* audioProperties = reader->getSourceAudioProperties();
        const int nbChannels = it->_channelIndexArray.empty() ? audioProperties->getNbChannels() : it->_channelIndexArray.size();
        _inputNbChannels.push_back(std::min(nbChannels, 5)); // skip LRE
        const size_t sampleRate = audioProperties->getSampleRate();
        _inputSampleRate.push_back(sampleRate);
        _totalNbSamplesToAnalyse += audioProperties->getNbSamples() * nbChannels;

        // Update output of reader
        reader->updateOutput(sampleRate, nbChannels, "fltp");
    }

    // Check the given configuration
    bool nbChannelsAreEqual = true;
    bool sampleRateAreEqual = true;
    const size_t nbChannels = _inputNbChannels.at(0);
    const size_t sampleRate = _inputSampleRate.at(0);
    for(size_t i = 1; i < _inputNbChannels.size(); i++)
    {
        // check number of channels
        if(_inputNbChannels.at(i) != nbChannels)
            nbChannelsAreEqual = false;
        // check sample rate
        if(_inputSampleRate.at(i) != sampleRate)
            sampleRateAreEqual = false;
    }

    if(!nbChannelsAreEqual || !sampleRateAreEqual)
    {
        std::string msg = "The given audio configuration isn't supported by the application.\n";
        msg += "Only audio stream with same sample rate and same number of channels are supported.\n";
        msg += "Error:\n";
        if(nbChannelsAreEqual == false)
            msg += "- Number of channels are not equals\n";
        if(sampleRateAreEqual == false)
            msg += "- Sampling rate are not equals\n";

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
	for(std::vector<avtranscoder::AudioReader*>::iterator it = _audioReader.begin(); it != _audioReader.end(); ++it)
    {
        delete(*it);
    }
}

size_t AvSoundFile::getTotalNbSamplesToAnalyse()
{
    size_t newTotalNbSamplesToAnalyse = 0;
    for(size_t i = 0; i < _inputSampleRate.size(); i++)
    {
        newTotalNbSamplesToAnalyse += _forceDurationToAnalyse * _inputSampleRate.at(i) * _inputNbChannels.at(i);
    }
    return newTotalNbSamplesToAnalyse;
}

bool AvSoundFile::fillAudioBuffer(float** audioBuffer, size_t& nbSamplesRead, size_t& nbInputChannelAdded)
{
    for(size_t fileIndex = 0; fileIndex < _audioReader.size(); ++fileIndex)
    {
        avtranscoder::IFrame* dstFrame = _audioReader.at(fileIndex)->readNextFrame();

        // empty frame: go to the end of process
        if(dstFrame == NULL)
        {
            return false;
        }

        size_t inputChannel = 0;
        for(size_t channelToAdd = nbInputChannelAdded;
            channelToAdd < nbInputChannelAdded + _inputNbChannels.at(fileIndex); ++channelToAdd)
        {
            audioBuffer[channelToAdd] = (float*)(dstFrame->getData()[inputChannel]);
            ++inputChannel;
            nbSamplesRead += dstFrame->getAVFrame().nb_samples;
        }
        nbInputChannelAdded += _inputNbChannels.at(fileIndex);
    }
    return true;
}

void AvSoundFile::analyse(Loudness::analyser::LoudnessAnalyser& analyser)
{
    // update number of samples to analyse
    if(_forceDurationToAnalyse)
    {
        // set total number of samples to analyse
        _totalNbSamplesToAnalyse = getTotalNbSamplesToAnalyse();
    }

    // open file to print duration
    std::ofstream outputFile;
    if(! _progressionFileName.empty())
    {
        outputFile.open(_progressionFileName.c_str());
        _outputStream = &outputFile;
    }

    // init
    analyser.initAndStart(_nbChannelsToAnalyse, _inputSampleRate.at(0));

    // Create planar buffer of float data
    float** audioBuffer = new float*[_nbChannelsToAnalyse];

    // Analyze audio streams
    while(!isEndOfAnalysis())
    {
        // Decode audio streams
        size_t nbSamplesRead = 0;
        size_t nbInputChannelAdded = 0;
        if(!fillAudioBuffer(audioBuffer, nbSamplesRead, nbInputChannelAdded))
            break;

        // Analyse loudness
        const size_t nbSamplesInOneFrame = nbSamplesRead / nbInputChannelAdded;
        analyser.processSamples(audioBuffer, nbSamplesInOneFrame);

        // Progress
        _cumulOfSamplesAnalysed += nbSamplesRead;
        printProgress();
    }

    // Close progression file
    if(! _progressionFileName.empty())
        outputFile.close();

    // free audio buffer
    delete[] audioBuffer;
}

void AvSoundFile::correct(Loudness::analyser::LoudnessAnalyser& analyser, const std::string& outputFilePath, const float gain)
{
    // update number of samples to analyse
    if(_forceDurationToAnalyse)
    {
        // set total number of samples to analyse
        _totalNbSamplesToAnalyse = getTotalNbSamplesToAnalyse();
    }

    // open file to print duration
    std::ofstream progressOutputFile;
    if(! _progressionFileName.empty())
    {
        progressOutputFile.open(_progressionFileName.c_str());
        _outputStream = &progressOutputFile;
    }

    // init
    analyser.initAndStart(_nbChannelsToAnalyse, _inputSampleRate.at(0));

    // Create planar buffer of float data
    float** audioBuffer = new float*[_nbChannelsToAnalyse];

    // Initialize output encoder
    int totalInputNbChannels = 0;
    for(size_t i = 0; i < _inputNbChannels.size(); ++i)
        totalInputNbChannels += _inputNbChannels.at(i);

    avtranscoder::AudioFrameDesc outputAudioFrameDesc(_audioReader.at(0)->getOutputSampleRate(), totalInputNbChannels, SAMPLE_FORMAT_24_BITS);
    avtranscoder::AudioEncoder* encoder = new avtranscoder::AudioEncoder(CODEC_NAME_24_BITS);
    encoder->setupAudioEncoder(outputAudioFrameDesc);

    avtranscoder::OutputFile* outputFile = new avtranscoder::OutputFile(outputFilePath);
    outputFile->addAudioStream(encoder->getAudioCodec());
    outputFile->beginWrap();

    // reset counters
    _cumulOfSamplesAnalysed = 0;

    while(!isEndOfAnalysis())
    {
        // Decode audio streams
        size_t nbSamplesRead = 0;
        size_t nbInputChannelAdded = 0;
        if(!fillAudioBuffer(audioBuffer, nbSamplesRead, nbInputChannelAdded))
            break;

        // Apply gain
        const size_t nbSamplesInOneFrame = nbSamplesRead / nbInputChannelAdded;
        applyGain(audioBuffer, nbSamplesInOneFrame, gain);

        // Convert corrected frame
        const size_t rawDataSize = nbSamplesRead * NB_OF_BYTES_24_BITS;
        unsigned char* rawData = new unsigned char[rawDataSize];
        encodePlanarSamplesToInterlacedPcm(audioBuffer, rawData, nbSamplesInOneFrame);

        // Write corrected frame
        avtranscoder::CodedData data;
        data.copyData(rawData, rawDataSize);
        outputFile->wrap(data, 0);

        // Analyse loudness
        analyser.processSamples(audioBuffer, nbSamplesInOneFrame);

        // Progress
        _cumulOfSamplesAnalysed += nbSamplesRead;
        printProgress();

        delete rawData;
    }

    outputFile->endWrap();

    delete encoder;
    delete outputFile;

    // Close progression file
    if(! _progressionFileName.empty())
        progressOutputFile.close();

    // free audio buffer
    delete[] audioBuffer;
}

float AvSoundFile::clipSample(const float value)
{
    if (value > 1.f) {
        return 1.f;
    }
    if (value < -1.f) {
        return -1.f;
    }
    return value;
}

void AvSoundFile::applyGain(float** audioBuffer, const size_t numberOfSamplesPerChannel, const float gain)
{
    for(size_t channel = 0; channel < _nbChannelsToAnalyse; channel++)
    {
        for(size_t sample = 0; sample < numberOfSamplesPerChannel; sample++)
        {
            audioBuffer[channel][sample] = audioBuffer[channel][sample] * gain;
        }
    }
}

void AvSoundFile::encodePlanarSamplesToInterlacedPcm(float** planarBuffer, unsigned char* interlacedBuffer, const size_t numberOfSamplesPerChannel)
{
    size_t sampleCounter = 0;
    for(size_t sample = 0; sample < numberOfSamplesPerChannel; sample++)
    {
        for(size_t channel = 0; channel < _nbChannelsToAnalyse; channel++)
        {
            const size_t offset = sampleCounter++ * NB_OF_BYTES_24_BITS;
            const float sampleValueClipped = clipSample(planarBuffer[channel][sample]);
            const int sampleValue = (int)(sampleValueClipped * MAX_VALUE_24_BITS);
            interlacedBuffer[offset] = sampleValue & 0xff;
            interlacedBuffer[offset + 1] = (sampleValue >> 8) & 0xff;
            interlacedBuffer[offset + 2] = (sampleValue >> 16) & 0xff;
        }
    }
}

void AvSoundFile::setDurationToAnalyse(const float durationToAnalyse)
{
    if(durationToAnalyse > 0)
        _forceDurationToAnalyse = durationToAnalyse;
}

size_t AvSoundFile::getNbChannelsToAnalyse() const
{
    size_t nbChannelsToAnalyse = 0;
    for(std::vector<size_t>::const_iterator it = _inputNbChannels.begin(); it != _inputNbChannels.end(); ++it)
    {
        nbChannelsToAnalyse += *it;
    }
    return nbChannelsToAnalyse;
}
