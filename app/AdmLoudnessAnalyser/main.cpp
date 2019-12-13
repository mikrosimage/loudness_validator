#include <iostream>

#include <admLoudnessAnalyser/AdmLoudnessAnalyser.hpp>

// size_t readAndRenderAdmAudioBlock(const std::unique_ptr<bw64::Bw64Reader>& inputFile,
//                                const admengine::Renderer& renderer,
//                                float* readFileBuffer,
//                                float* renderingBuffer) {
//     const size_t nbFrames = inputFile->read(readFileBuffer, admengine::BLOCK_SIZE);
//     if(nbFrames == 0)
//         return 0;
//     return renderer.processBlock(nbFrames, readFileBuffer, renderingBuffer);
// }

void displayUsage(const char* application) {
    std::cout << "Usage: " << application << " INPUT [OPTIONS]" << std::endl;
    std::cout << std::endl;
    std::cout << "  INPUT                  BW64/ADM audio file" << std::endl;
    std::cout << "  OPTIONS:" << std::endl;
    std::cout << "    -o OUTPUT            Output file with updated ADM with loudness values" << std::endl;
    std::cout << "    -d --display         Display loudness analyse values" << std::endl;
    std::cout << "    -c --correction      Enable correction (if an output file is specified)" << std::endl;
    std::cout << "    -l --limiter         Enable peak limiter (if correction is enabled)" << std::endl;
    std::cout << "    -e ELEMENT_ID        Select the AudioProgramme to be analysed by ELEMENT_ID" << std::endl;
    std::cout << std::endl;
}

int main(int argc, char const *argv[])
{
    // std::cout << "Welcome to ADM Loundess Analyser!" << std::endl;
    if (argc < 2) {
        displayUsage(argv[0]);
        return 1;
    }

    std::string inputFilePath = argv[1];
    std::string outputFilePath;
    std::string elementIdToRender;
    bool displayValues = false;
    bool enableCorrection = false;
    bool enableLimiter = false;

    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        if(arg == "-o") {
            outputFilePath = argv[++i];
        } else if(arg == "-e") {
            elementIdToRender = argv[++i];
        } else if(arg == "-d" || arg == "--display") {
            displayValues = true;
        } else if(arg == "-c" || arg == "--correction") {
            enableCorrection = true;
        } else if(arg == "-l" || arg == "--limiter") {
            enableLimiter = true;
        } else {
            std::cerr << "Invalid argument: " << arg << std::endl;
            displayUsage(argv[0]);
            return 1;
        }
    }

    if(outputFilePath.empty() && (enableCorrection || enableLimiter)) {
        std::cerr << "Error: an output file must be specified to enable correction and limiter." << std::endl;
        displayUsage(argv[0]);
        return 1;
    }
    if(enableLimiter && !enableCorrection) {
        std::cerr << "Error: correction must be enabled to enable limiter." << std::endl;
        displayUsage(argv[0]);
        return 1;
    }

    std::cout << "Input file:            " << inputFilePath << std::endl;
    std::cout << "Output file:           " << outputFilePath << std::endl;
    std::cout << "ADM element to render: " << elementIdToRender << std::endl;
    std::cout << "Display result:        " << displayValues << std::endl;
    std::cout << "Correction enabled:    " << enableCorrection << std::endl;
    std::cout << "Peak limiter enabled:  " << enableLimiter << std::endl;

    const std::string outputLayout("0+2+0");

    try {
        Loudness::admanalyser::AdmLoudnessAnalyser analyser(inputFilePath, outputLayout, outputFilePath);
        analyser.process(displayValues, enableCorrection, enableLimiter);
    } catch(std::exception e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
