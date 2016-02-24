#include "AvSoundFile.hpp"

#include <loudnessAnalyser/LoudnessAnalyser.hpp>
#include <loudnessTools/WriteXml.hpp>

#include <vector>
#include <utility>
#include <fstream>

std::vector<AudioElement> parseConfigFile(const std::string& configFilename)
{
    std::vector<AudioElement> result;

    std::ifstream configFile(configFilename.c_str(), std::ifstream::in);

    std::string line;
    while(std::getline(configFile, line))
    {
        std::istringstream is_line(line);
        std::string filename;
        if(std::getline(is_line, filename, '='))
        {
            std::string stream;
            std::getline(is_line, stream);

            std::stringstream ss(stream);
            size_t streamIndex = 0;
            char separator;
            int channelIndex = -1;
            ss >> streamIndex;
            ss >> separator;
            if(separator == '.')
                ss >> channelIndex;

            result.push_back(AudioElement(filename, streamIndex, channelIndex));
        }
    }

    configFile.close();

    return result;
}

void printHelp()
{
    std::string help;
    help += "Usage\n";
    help += "\tmedia-analyser CONFIG.TXT [--output XMLReportName][--progressionInFile "
            "progressionName][--forceDurationToAnalyse durationToAnalyse][--help]\n";
    help += "CONFIG.TXT\n";
    help += "\tEach line will be one audio stream analysed by the loudness library.\n";
    help += "\tPattern of each line is:\n";
    help += "\t[inputFile]=STREAM_ID\n";
    help += "Command line options\n";
    help += "\t--help: display this help\n";
    help += "\t--output: filename of the XML report\n";
    help += "\t--progressionInFile: to print the progression in a file instead of in console\n";
    help += "\t--forceDurationToAnalyse: to force loudness analysis on a specific duration (in seconds). By default this is "
            "the duration of the input.\n";
    std::cout << help << std::endl;
}

int main(int argc, char** argv)
{
    std::string outputXMLReportName("PLoud.xml");
    std::string outputProgressionName;
    float durationToAnalyse = 0;

    std::vector<std::string> arguments;
    for(int argument = 1; argument < argc; ++argument)
    {
        arguments.push_back(argv[argument]);
    }

    for(size_t argument = 1; argument < arguments.size(); ++argument)
    {
        if(arguments.at(argument) == "--help")
        {
            printHelp();
            return 0;
        }
        else if(arguments.at(argument) == "--output")
        {
            try
            {
                outputXMLReportName = arguments.at(++argument);
            }
            catch(...)
            {
                printHelp();
                return 1;
            }
        }
        else if(arguments.at(argument) == "--progressionInFile")
        {
            try
            {
                outputProgressionName = arguments.at(++argument);
            }
            catch(...)
            {
                printHelp();
                return 1;
            }
        }
        else if(arguments.at(argument) == "--forceDurationToAnalyse")
        {
            try
            {
                durationToAnalyse = atof(arguments.at(++argument).c_str());
            }
            catch(...)
            {
                printHelp();
                return 1;
            }
        }
        // unknown argument
        else
        {
            printHelp();
            return 1;
        }
    }

    // Check required arguments
    if(argc < 2)
    {
        std::cout << "completeAnalyser requires a media filename" << std::endl;
        std::cout << "Use option --help to display help" << std::endl;
        return (-1);
    }

    avtranscoder::preloadCodecsAndFormats();
    avtranscoder::Logger::setLogLevel(AV_LOG_QUIET);

    // Get list of files / streamIndex to analyse
    std::vector<AudioElement> arrayToAnalyse = parseConfigFile(arguments.at(0));
    AvSoundFile soudFile(arrayToAnalyse);
    soudFile.setProgressionFile(outputProgressionName);
    soudFile.setDurationToAnalyse(durationToAnalyse);

    // Analyse loudness according to EBU R-128
    Loudness::analyser::LoudnessLevels level = Loudness::analyser::LoudnessLevels::Loudness_EBU_R128();
    Loudness::analyser::LoudnessAnalyser analyser(level);
    soudFile.analyse(analyser);

    // Print analyse
    analyser.printPloudValues();

    // Write XML
    std::vector<std::string> mediaFilenames;
    for(size_t i = 0; i < arrayToAnalyse.size(); ++i)
    {
        mediaFilenames.push_back(arrayToAnalyse.at(i)._inputFile);
    }
    Loudness::tools::WriteXml writerXml(outputXMLReportName, mediaFilenames);
    writerXml.writeResults("unknown", analyser);
}
