#include "AvSoundFile.hpp"

#include <iomanip>
#include <fstream>
#include <algorithm>
#include <stdexcept>

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

    bool emptyFrameDecoded = false;
    // Decode audio streams
    while(!isEndOfAnalysis())
    {
        size_t nbSamplesRead = 0;
        size_t nbInputChannelAdded = 0;
        for(size_t fileIndex = 0; fileIndex < _audioReader.size(); ++fileIndex)
        {
            avtranscoder::IFrame* dstFrame = _audioReader.at(fileIndex)->readNextFrame();

            // empty frame: go to the end of process
            if(dstFrame == NULL)
            {
                emptyFrameDecoded = true;
                break;
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
        if(emptyFrameDecoded)
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
