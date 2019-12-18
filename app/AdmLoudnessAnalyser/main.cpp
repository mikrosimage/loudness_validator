#include <iostream>

#include <admLoudnessAnalyser/AdmLoudnessAnalyser.hpp>
#include <adm_engine/parser.hpp>

void displayUsage(const char* application) {
    std::cout << "Usage: " << application << " MODE INPUT [OPTIONS]" << std::endl;
    std::cout << std::endl;
    std::cout << "  MODE   analyse           Enable loudness analyse" << std::endl;
    std::cout << "         correction        Enable loudness analyse and correction" << std::endl;
    std::cout << std::endl;
    std::cout << "  INPUT                    BW64/ADM audio file" << std::endl;
    std::cout << std::endl;
    std::cout << "  OPTIONS:" << std::endl;
    std::cout << "    Common options:" << std::endl;
    std::cout << "      -d --display         Display loudness analyse values" << std::endl;
    std::cout << "      -e ELEMENT_ID        Select the AudioProgramme to be rendered and analysed (and corrected) by ELEMENT_ID" << std::endl;
    std::cout << "      -g ELEMENT_ID=GAIN   GAIN value (in dB) to apply to ADM element defined by its ELEMENT_ID at ADM rendering" << std::endl;
    std::cout << std::endl;
    std::cout << "    Analyse mode options:" << std::endl;
    std::cout << "      -o OUTPUT            Output file with updated ADM with loudness values" << std::endl;
    std::cout << std::endl;
    std::cout << "    Correction mode options:" << std::endl;
    std::cout << "      -o OUTPUT            Output directory where ADM rendered and loudness corrected files will be written" << std::endl;
    std::cout << "      -l --limiter         Enable peak limiter (if correction is enabled)" << std::endl;
    std::cout << std::endl;
}

std::map<std::string, float> parseElementGains(const std::vector<std::string>& elementGainsPairs) {
    std::map<std::string, float> elementGains;
    for(const std::string& gainPair : elementGainsPairs) {
        const size_t splitPos = gainPair.find("=");
        const std::string elemId = gainPair.substr(0, splitPos);
        const std::string gainDbStr = gainPair.substr(splitPos + 1, gainPair.size());
        elementGains[elemId] = pow(10.0, std::atof(gainDbStr.c_str()) / 20.0);
        std::cout << "Gain:                  " << elementGains[elemId] << " (" << gainDbStr << " dB) applied to " << elemId << std::endl;
    }
    return elementGains;
}

int main(int argc, char const *argv[])
{
    // std::cout << "Welcome to ADM Loundess Analyser!" << std::endl;
    if (argc < 3) {
        displayUsage(argv[0]);
        return 1;
    }

    const std::string mode = argv[1];
    const std::string inputFilePath = argv[2];

    std::string outputPath; // file or directory
    std::string elementIdToRender;
    std::vector<std::string> elementGainsPairs;
    bool displayValues = false;
    bool enableCorrection = false;
    bool enableLimiter = false;

    if(mode == "analyse") {
        for (int i = 3; i < argc; ++i) {
            std::string arg = argv[i];
            if(arg == "-o") {
                outputPath = argv[++i];
            } else if(arg == "-e") {
                elementIdToRender = argv[++i];
            } else if(arg == "-d" || arg == "--display") {
                displayValues = true;
            } else if(arg == "-g") {
                elementGainsPairs.push_back(argv[++i]);
            } else {
                std::cerr << "Invalid argument: " << arg << std::endl;
                displayUsage(argv[0]);
                return 1;
            }
        }

    } else if(mode == "correction") {
        enableCorrection = true;

        for (int i = 3; i < argc; ++i) {
            std::string arg = argv[i];
            if(arg == "-o") {
                outputPath = argv[++i];
            } else if(arg == "-e") {
                elementIdToRender = argv[++i];
            } else if(arg == "-d" || arg == "--display") {
                displayValues = true;
            } else if(arg == "-l" || arg == "--limiter") {
                enableLimiter = true;
            } else if(arg == "-g") {
                elementGainsPairs.push_back(argv[++i]);
            } else {
                std::cerr << "Invalid argument: " << arg << std::endl;
                displayUsage(argv[0]);
                return 1;
            }
        }

        if(outputPath.empty()) {
            std::cerr << "Error: an output directory must be specified to enable correction." << std::endl << std::endl;
            displayUsage(argv[0]);
            return 1;
        }

    } else {
        std::cerr << "Error: unknown mode: " << mode << std::endl << std::endl;
        displayUsage(argv[0]);
        return 1;
    }

    std::cout << "Input file:            " << inputFilePath << std::endl;
    std::cout << "Output path:           " << (outputPath.empty()? "-" : outputPath) << std::endl;
    std::cout << "ADM element to render: " << (elementIdToRender.empty()? "-" : elementIdToRender) << std::endl;
    std::cout << "Display result:        " << (displayValues? "true" : "false") << std::endl;
    std::cout << "Correction enabled:    " << (enableCorrection? "true" : "false") << std::endl;
    if(enableCorrection) {
        std::cout << "Peak limiter enabled:  " << (enableLimiter? "true" : "false") << std::endl;
    }

    const std::string outputLayout("0+2+0");
    std::map<std::string, float> elementGains = parseElementGains(elementGainsPairs);

    try {
        Loudness::admanalyser::AdmLoudnessAnalyser analyser(inputFilePath, outputLayout, elementGains, outputPath, elementIdToRender);
        analyser.process(displayValues, enableCorrection, enableLimiter);
    } catch(const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
