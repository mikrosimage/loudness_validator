#ifndef LOUDNESS_TOOLS_WRITE_XML_HPP_
#define LOUDNESS_TOOLS_WRITE_XML_HPP_

#include <loudnessAnalyser/LoudnessAnalyser.hpp>

#include <vector>
#include <fstream>

namespace Loudness
{
namespace tools
{

class WriteXml
{
public:
    WriteXml(const std::string& xmlFilename, const std::string& srcAudioFilename);
    WriteXml(const std::string& xmlFilename, const std::vector<std::string>& srcAudioFilenames);

    ~WriteXml();

    void writeResults(const std::string& channelType, Loudness::analyser::LoudnessAnalyser& analyser);

private:
    void openXMLFile(const std::string& xmlFilename);
    std::string convertValid(Loudness::analyser::ELoudnessResult result);
    std::string printStandard(Loudness::analyser::EStandard standard);
    std::string writeValues(std::vector<float> datas);
    std::string replaceXmlSpecialCharacters(const std::string& text);
    std::string getDate();

    std::ofstream xmlFile;
    std::vector<std::string> srcAudioFilenames;
};
}
}

#endif
