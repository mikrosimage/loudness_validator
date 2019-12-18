
#include "AdmLoudnessAnalyser.hpp"

#include <adm_engine/adm_helper.hpp>
#include <adm_engine/parser.hpp>

#include <loudnessCorrector/PeakLimiter.hpp>

#include <chrono>
#include <sys/stat.h>

namespace Loudness
{

namespace admanalyser
{


struct stat s;

enum EPathType {
    unknown = 0,
    file,
    directory
};

EPathType getPathType(const std::string& path) {
    if(stat(path.c_str(), &s) == 0)
    {
        if( s.st_mode & S_IFDIR ) {
            return EPathType::directory;
        }
        else if( s.st_mode & S_IFREG ) {
            return EPathType::file;
        }
        else {
            return EPathType::unknown;
        }
    } else {
        return EPathType::unknown;
    }
}

AdmLoudnessAnalyser::AdmLoudnessAnalyser(const std::string& inputFilePath,
                                         const std::string& outputLayout,
                                         const std::map<std::string, float>& elementGainsMapping,
                                         const std::string& outputPath,
                                         const std::string& audioProgrammeIdToRender)
    : _inputFilePath(inputFilePath)
    , _inputFile(bw64::readFile(_inputFilePath))
    , _renderer(_inputFile, outputLayout, ".", elementGainsMapping, audioProgrammeIdToRender)
    , _outputPath(outputPath)
    , _audioProgrammeIdToRender(audioProgrammeIdToRender) {

}

std::shared_ptr<adm::Document> AdmLoudnessAnalyser::process(const bool displayValues, const bool enableCorrection, const bool enableLimiter) {
    std::shared_ptr<adm::Document> admDocument = _renderer.getDocument();
    const std::shared_ptr<bw64::ChnaChunk> chnaChunk = _renderer.getAdmChnaChunk();
    std::vector<std::shared_ptr<adm::AudioProgramme>> audioProgrammes = _renderer.getDocumentAudioProgrammes();

    // Check output path
    if(enableCorrection) {
        if(_outputPath.empty()) {
            throw std::runtime_error("An output file must be specified to enable correction.");
        } else if(getPathType(_outputPath) != EPathType::directory) {
            throw std::runtime_error("Invalid argument: correction output path must be a directory.");
        }
    } else if(_outputPath.size() && getPathType(_outputPath) == EPathType::directory) {
        throw std::runtime_error("Invalid argument: analyse output path cannot be a directory.");
    }


    if(audioProgrammes.size()) {
        for(auto audioProgramme : audioProgrammes) {

            const std::string audioProgrammeId = formatId(audioProgramme->get<adm::AudioProgrammeId>());

            if(!_audioProgrammeIdToRender.empty() &&
                _audioProgrammeIdToRender != audioProgrammeId) {
                // skip audioProgramme
                continue;
            }

            std::string correctedFilePath;
            std::unique_ptr<bw64::Bw64Writer> correctedFile;

            if(enableCorrection) {
                // Initialize temporary correction file
                std::stringstream tempFilePath;
                tempFilePath << _outputPath << "/";
                tempFilePath << "adm_loudness_correction_";
                tempFilePath << audioProgrammeId << "_";
                tempFilePath << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
                tempFilePath << ".wav";
                correctedFilePath = tempFilePath.str();
                correctedFile = bw64::writeFile(correctedFilePath, _renderer.getNbOutputChannels(), _inputFile->sampleRate(), _inputFile->bitDepth());
            }

            // Initialize audio programme loudness
            std::cout << "### Render audio programme: " << admengine::toString(audioProgramme) << std::endl;
            _renderer.initAudioProgrammeRendering(audioProgramme);
            const adm::LoudnessMetadata loudnessMetadata = analyseLoudness(displayValues, correctedFile, enableLimiter);

            // Update audio programme
            admDocument->remove(audioProgramme);
            audioProgramme->set(loudnessMetadata);
            admDocument->add(audioProgramme);

            if(correctedFile) {
                // Create output ADM document
                std::shared_ptr<adm::Document> outputAdmDocument = adm::Document::create();
                admengine::copyAudioProgramme(outputAdmDocument, audioProgramme);

                const std::string programmeTitle = audioProgramme->get<adm::AudioProgrammeName>().get();

                std::stringstream outputFilePath;
                outputFilePath << _outputPath << "/";
                outputFilePath << programmeTitle << "_loudness_corrected.wav";
                std::cout << "Write corrected output file with updated ADM document: " << outputFilePath.str() << std::endl;

                // in case of correction
                // close corrected file writer
                correctedFile.reset();
                // and open a reader on it
                std::unique_ptr<bw64::Bw64Reader> correctedFileReader = bw64::readFile(correctedFilePath);
                // Replace ADM document into output file
                std::shared_ptr<bw64::AxmlChunk> axml = admengine::createAxmlChunk(outputAdmDocument);
                std::unique_ptr<bw64::Bw64Writer> outputFile = bw64::writeFile(outputFilePath.str(), _renderer.getNbOutputChannels(), _inputFile->sampleRate(), _inputFile->bitDepth(), chnaChunk, axml);

                // transfer data to output file
                std::vector<float> buffer(admengine::BLOCK_SIZE * correctedFileReader->channels());
                size_t readFrames = 0;
                while (!correctedFileReader->eof()) {
                    readFrames = correctedFileReader->read(&buffer[0], admengine::BLOCK_SIZE);
                    outputFile->write(&buffer[0], readFrames);
                }

                // remove temporary file
                correctedFileReader.reset();
                if(std::remove(correctedFilePath.c_str())) {
                    std::stringstream message;
                    message << "Could not remove temporary file: " << correctedFilePath;
                    throw std::runtime_error(message.str());
                }
            }
        }

        if(!enableCorrection && !_outputPath.empty()) {
            // Replace ADM document into output file
            std::shared_ptr<bw64::AxmlChunk> axml = admengine::createAxmlChunk(admDocument);
            std::unique_ptr<bw64::Bw64Writer> outputFile = bw64::writeFile(_outputPath, _inputFile->channels(), _inputFile->sampleRate(), _inputFile->bitDepth(), chnaChunk, axml);

            size_t readFrames = 0;
            // re-init input file
            _inputFile->seek(0);

            // transfer data to output file
            std::vector<float> buffer(admengine::BLOCK_SIZE * _inputFile->channels());
            while (!_inputFile->eof()) {
                readFrames = _inputFile->read(&buffer[0], admengine::BLOCK_SIZE);
                outputFile->write(&buffer[0], readFrames);
            }
        }
        return admDocument;
    } else {
        throw std::runtime_error("No ADM audio programme found.");
    }
}

adm::LoudnessMetadata AdmLoudnessAnalyser::analyseLoudness(const bool displayValues,
                                                           const std::unique_ptr<bw64::Bw64Writer>& correctedFile,
                                                           const bool enableLimiter) {
    // Analyse loudness according to EBU R-128
    Loudness::analyser::LoudnessLevels levels = Loudness::analyser::LoudnessLevels::Loudness_EBU_R128();
    Loudness::analyser::LoudnessAnalyser analyser(levels);

    // init
    const size_t nbChannelsToAnalyse = _renderer.getNbOutputChannels();
    const size_t nbInputChannels = _inputFile->channels();
    const size_t sampleRate = _inputFile->sampleRate();
    analyser.initAndStart(nbChannelsToAnalyse, sampleRate);

    // Create interlaced buffer
    float readFileBuffer[admengine::BLOCK_SIZE * nbInputChannels] = {0.0,}; // nb of samples * nb input channels

    while (!_inputFile->eof()) {
        // Read a data block
        const size_t nbFrames = _inputFile->read(readFileBuffer, admengine::BLOCK_SIZE);
        if(nbFrames == 0)
            break;

        float admRenderBuffer[admengine::BLOCK_SIZE * nbChannelsToAnalyse] = {0.0,}; // nb of samples * nb output channels
        const size_t renderedSamples = _renderer.processBlock(nbFrames, readFileBuffer, admRenderBuffer);

        // Create planar buffer of float data
        float** loudnessInputBuffer = new float*[nbChannelsToAnalyse];
        convertInterlacedToPlanarBuffer(admRenderBuffer, renderedSamples, loudnessInputBuffer, nbChannelsToAnalyse, nbFrames);
        analyser.processSamples(loudnessInputBuffer, nbFrames);

        // free audio buffer
        delete[] loudnessInputBuffer;
    }
    _inputFile->seek(0);

    displayResult(analyser.isValidProgram());
    if(displayValues) {
        analyser.printPloudValues();
    }

    if(correctedFile) {
        const float attackMs = 1.0f;  // maximum attack/lookahead time in milliseconds
        const float releaseMs = 20.0f;    // release time in milliseconds (90% time constant)

        float gain = analyser.getCorrectionGain(enableLimiter);
        std::cout << " => applying correction: gain = " << gain;
        float threshold = std::pow(10, (levels.truePeakTargetLevel) / 20);
        std::cout << ", levels.truePeakTargetLevel: " << levels.truePeakTargetLevel;
        std::cout << ", threshold: " << threshold << std::endl;

        Loudness::analyser::LoudnessAnalyser analyserAfterCorrection(levels);
        analyserAfterCorrection.initAndStart(nbChannelsToAnalyse, sampleRate);

        // correction with limiter
        float* writeBuffer = new float[nbChannelsToAnalyse * admengine::BLOCK_SIZE];

        Loudness::corrector::PeakLimiter peakLimiter(attackMs, releaseMs, threshold, nbChannelsToAnalyse, sampleRate);
        while(true)
        {
            // Read a data block
            const size_t nbFrames = _inputFile->read(readFileBuffer, admengine::BLOCK_SIZE);
            if(nbFrames == 0)
                break;

            float admRenderBuffer[admengine::BLOCK_SIZE * nbChannelsToAnalyse] = {0.0,}; // nb of samples * nb output channels
            const size_t renderedSamples = _renderer.processBlock(nbFrames, readFileBuffer, admRenderBuffer);

            // Correct
            float* inData = &admRenderBuffer[0];
            float* outData = new float[renderedSamples];

            for(size_t i = 0; i < renderedSamples; i++)
            {
                outData[i] = (*inData) * gain;
                inData++;
            }

            if(enableLimiter) {
                // Apply limiter
                if(peakLimiter.apply(outData, writeBuffer, nbFrames)) {
                    std::cerr << "An error occurred applying limiter" << std::endl;
                }
                delete[] outData;
            } else {
                writeBuffer = &outData[0];
            }

            // analyse corrected data
            // Create planar buffer of float data
            float** loudnessInputBuffer = new float*[nbChannelsToAnalyse];
            convertInterlacedToPlanarBuffer(writeBuffer, renderedSamples, loudnessInputBuffer, nbChannelsToAnalyse, nbFrames);
            analyserAfterCorrection.processSamples(loudnessInputBuffer, nbFrames);

            // free audio buffer
            delete[] loudnessInputBuffer;

            correctedFile->write(writeBuffer, nbFrames);
            // correctedFile->write(admRenderBuffer, nbFrames);
        }
        delete[] writeBuffer;
        _inputFile->seek(0);

        displayResult(analyserAfterCorrection.isValidProgram());
        if(displayValues) {
            analyserAfterCorrection.printPloudValues();
        }
        return getLoudnessMetadata(analyserAfterCorrection);
    }
    return getLoudnessMetadata(analyser);
}

void AdmLoudnessAnalyser::convertInterlacedToPlanarBuffer(float* interlaced, const size_t nbSamples, float** planar, const size_t nbChannels, const size_t nbFrames) {
    for (size_t c = 0; c < nbChannels; ++c) {
        planar[c] = new float[nbFrames];
    }

    size_t channel_id = 0;
    size_t frame_id = 0;
    for (size_t s = 0; s < nbSamples; ++s) {
        channel_id = s % nbChannels;
        frame_id = s / nbChannels;
        planar[channel_id][frame_id] = interlaced[s];
    }
}

void AdmLoudnessAnalyser::displayResult(const Loudness::analyser::ELoudnessResult& result) {
    std::cout << "Program loudness: ";
    switch(result) {
        case Loudness::analyser::ELoudnessResult::eValidResult:
            std::cout << "Valid" << std::endl;
            break;
        case Loudness::analyser::ELoudnessResult::eNotValidResult:
            std::cout << "Invalid" << std::endl;
            break;
        case Loudness::analyser::ELoudnessResult::eNotValidResultButNotIllegal:
            std::cout << "Invalid but legal" << std::endl;
            break;
        case Loudness::analyser::ELoudnessResult::eNoImportance:
            std::cout << "No importance (according to the specification)" << std::endl;
            break;
    }
}

adm::LoudnessMetadata AdmLoudnessAnalyser::getLoudnessMetadata(Loudness::analyser::LoudnessAnalyser& analyser) {
    adm::LoudnessMetadata loudnessMetadata;
    loudnessMetadata.set(adm::LoudnessMethod("ITU-R BS.1770"));
    loudnessMetadata.set(adm::LoudnessRecType("EBU R128"));
    loudnessMetadata.set(adm::IntegratedLoudness(analyser.getIntegratedLoudness()));
    loudnessMetadata.set(adm::MaxTruePeak(analyser.getTruePeakValue()));
    loudnessMetadata.set(adm::MaxMomentary(analyser.getMomentaryLoudness()));

    if(analyser.isShortProgram()) {
        loudnessMetadata.set(adm::LoudnessRange(analyser.getIntegratedRange()));
        loudnessMetadata.set(adm::MaxShortTerm(analyser.getMaxShortTermLoudness()));
    }
    return loudnessMetadata;
}

}

}
