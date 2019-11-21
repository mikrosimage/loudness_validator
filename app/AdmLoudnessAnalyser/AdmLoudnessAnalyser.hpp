#ifndef ADM_LOUDNESS_ANALYSER_HPP
#define ADM_LOUDNESS_ANALYSER_HPP

#include <adm_engine/renderer.hpp>

#include <loudnessAnalyser/LoudnessAnalyser.hpp>

class AdmLoudnessAnalyser {
public:
    AdmLoudnessAnalyser(const std::string& inputFilePath,
                        const std::string& outputLayout,
                        const std::string& outputFilePath = "",
                        const std::string& audioProgrammeIdToRender = "");

    void process(const bool displayValues, const bool enableCorrection, const bool enableLimiter = true);

private:
    adm::LoudnessMetadata analyseLoudness(const bool displayValues,
                                          const std::unique_ptr<bw64::Bw64Writer>& outputFile,
                                          const bool enableLimiter);

    void convertInterlacedToPlanarBuffer(float* interlaced, const size_t nbSamples, float** planar, const size_t nbChannels, const size_t nbFrames);
    void displayResult(const Loudness::analyser::ELoudnessResult& result);

private:
    const std::string _inputFilePath;
    const std::unique_ptr<bw64::Bw64Reader> _inputFile;
    admengine::Renderer _renderer;

    const std::string _outputFilePath;
    const std::string _audioProgrammeIdToRender;
};

#endif
