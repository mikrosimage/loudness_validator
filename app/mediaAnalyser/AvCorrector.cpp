#include "AvCorrector.hpp"

#include <AvTranscoder/data/decoded/AudioFrame.hpp>
#include <AvTranscoder/encoder/AudioEncoder.hpp>
#include <AvTranscoder/file/OutputFile.hpp>
#include <AvTranscoder/filter/FilterGraph.hpp>
#include <AvTranscoder/codec/AudioCodec.hpp>

AvCorrector::AvCorrector(const std::vector<avtranscoder::InputStreamDesc>& arrayToCorrect, const std::string& correctionOutputName)
    : _totalNbSamplesToCorrect(0)
    , _cumulOfSamplesCorrected(0)
    , _inputNbChannels()
    , _inputSampleRate()
    , _currentReaderSamplesMaxValue()
    , _audioReader()
    , _correctedStreamDescs()
{
    for (int i = 0; i < arrayToCorrect.size(); ++i)
    {
        const avtranscoder::InputStreamDesc inputStreamDesc = arrayToCorrect.at(i);
        // Create reader to convert to float planar
        avtranscoder::AudioReader* reader = new avtranscoder::AudioReader(inputStreamDesc);
        reader->continueWithGenerator();
        _audioReader.push_back(reader);

        // Get data from audio stream
        const avtranscoder::AudioProperties* audioProperties = reader->getSourceAudioProperties();
        const int nbChannels = inputStreamDesc._channelIndexArray.empty() ?
                               audioProperties->getNbChannels() : inputStreamDesc._channelIndexArray.size();
        _inputNbChannels.push_back(nbChannels);
        const size_t sampleRate = audioProperties->getSampleRate();
        _inputSampleRate.push_back(sampleRate);

        const int nbSamples = (audioProperties->getNbSamples() / audioProperties->getNbChannels()) * nbChannels;
        _nbSamplesToCorrect.push_back(nbSamples);
        _totalNbSamplesToCorrect += nbSamples;

        reader->updateOutput(sampleRate, nbChannels, audioProperties->getSampleFormatName());

        std::stringstream outputStreamFileName;
        if(!correctionOutputName.empty())
        {
            outputStreamFileName << correctionOutputName << "_" << i << ".wav";
        }
        else
        {
            outputStreamFileName << inputStreamDesc._filename.substr(0, inputStreamDesc._filename.size() - 4)
                                 << "_corrected_" << i << ".wav";
        }
        avtranscoder::InputStreamDesc outputStreamDesc(outputStreamFileName.str(), 0);
        _correctedStreamDescs.push_back(outputStreamDesc);
    }
}

AvCorrector::~AvCorrector()
{
    for(std::vector<avtranscoder::AudioReader*>::iterator it = _audioReader.begin(); it != _audioReader.end(); ++it)
    {
        delete(*it);
    }
}

void AvCorrector::correct(const float gain)
{
    for (int i = 0; i < _audioReader.size(); ++i)
    {
        avtranscoder::AudioReader* reader = _audioReader.at(i);
        const avtranscoder::AudioProperties* inputAudioProperties = reader->getSourceAudioProperties();

        avtranscoder::AudioFrameDesc audioFrameDesc(inputAudioProperties->getSampleRate(),
                                                    _inputNbChannels.at(i),
                                                    inputAudioProperties->getSampleFormatName());

        avtranscoder::AudioEncoder* encoder = new avtranscoder::AudioEncoder(inputAudioProperties->getCodecName());
        encoder->setupAudioEncoder(audioFrameDesc);

        avtranscoder::OutputFile* outputFile = new avtranscoder::OutputFile(_correctedStreamDescs.at(i)._filename);
        outputFile->addAudioStream(encoder->getAudioCodec());
        outputFile->beginWrap();

        bool emptyFrameDecoded = false;

        _currentReaderSamplesMaxValue = getSampleMaxValue(inputAudioProperties->getSampleFormatName());

        // Decode audio streams
        int count = 0;
        while(count < _nbSamplesToCorrect.at(i))
        {
            if(isEndOfCorrection())
            {
                break;
            }

            avtranscoder::AudioFrame* srcFrame = (avtranscoder::AudioFrame*) _audioReader.at(i)->readNextFrame();

            // empty frame: go to the end of process
            if(srcFrame == NULL)
            {
                emptyFrameDecoded = true;
                break;
            }

            const size_t nbChannels = _inputNbChannels.at(i);
            const size_t nbSamples = srcFrame->getNbSamplesPerChannel();
            const size_t bytesPerSample = srcFrame->getBytesPerSample();

            avtranscoder::AudioFrame* audioFrame = new avtranscoder::AudioFrame(audioFrameDesc, false);
            audioFrame->setNbSamplesPerChannel(nbSamples);
            audioFrame->allocateData();

            unsigned char* srcData = srcFrame->getData()[0];
            unsigned char* dstData = audioFrame->getData()[0];

            // Correct input
            const int correctedSamples = correctFrame(srcData, dstData, nbChannels, nbSamples, bytesPerSample, gain);

            avtranscoder::CodedData data;
            encoder->encodeFrame(*audioFrame, data);
            outputFile->wrap(data, 0);

            delete audioFrame;

            count += correctedSamples;
            _cumulOfSamplesCorrected += correctedSamples;
        }

        outputFile->endWrap();
        delete encoder;
        delete outputFile;
    }
}

int AvCorrector::getSampleMaxValue(const std::string& sampleFormat)
{
    if(sampleFormat == "s8")
        return 0xFF000000;
    if(sampleFormat == "s16")
        return 0xFFFF0000;
    if(sampleFormat == "s32")
        return 0xFFFFFF00;
    throw std::runtime_error("Cannot handle such a sample format: " + sampleFormat);
}

size_t AvCorrector::correctFrame(unsigned char* srcData, unsigned char* dstData, const size_t nbChannels, const size_t nbSamples,
    const size_t bytesPerSample, const float gain)
{
    const int half = _currentReaderSamplesMaxValue / 2;

    size_t correctedSamples = 0;
    const size_t channelDataSize = nbSamples * bytesPerSample;

    int byte = 0;
    for(size_t c = 0; c < nbChannels; c++)
    {
        // Get channel data
        unsigned char* channelData = &srcData[c*channelDataSize];
        for(size_t s = 0; s < nbSamples; s++)
        {
            // Correct audio sample
            Sample audioSample;
            for (size_t b = 0; b < bytesPerSample; ++b)
            {
                audioSample.bytes[b] = (*channelData);
                channelData++;
            }

            if(audioSample.value > half)
            {
                const int value = audioSample.value - _currentReaderSamplesMaxValue;
                int gainedValue = value * gain;
                if(gainedValue - value == 0)
                {
                    if(gainedValue > 0)
                        gainedValue++;
                    else
                        gainedValue--;
                }
                audioSample.value = (gainedValue + _currentReaderSamplesMaxValue) & _currentReaderSamplesMaxValue;
            } else {
                audioSample.value = (int)(audioSample.value * gain) & _currentReaderSamplesMaxValue;
            }

            for (size_t b = 0; b < bytesPerSample; ++b)
            {
                dstData[byte] = audioSample.bytes[b];
                byte++;
            }
            correctedSamples++;
        }
    }
    return correctedSamples;
}


bool AvCorrector::isEndOfCorrection()
{
    return _cumulOfSamplesCorrected >= _totalNbSamplesToCorrect;
}
