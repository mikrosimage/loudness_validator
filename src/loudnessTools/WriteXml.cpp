#include "WriteXml.hpp"

#include <iostream>
#include <sstream>
#include <ctime>

namespace Loudness
{
namespace tools
{

WriteXml::WriteXml(const std::string& xmlFilename, const std::string& srcAudioFilename)
{
    srcAudioFilenames.push_back(srcAudioFilename);
    openXMLFile(xmlFilename);
}

WriteXml::WriteXml(const std::string& xmlFilename, const std::vector<std::string>& srcAudioFilenames)
    : srcAudioFilenames(srcAudioFilenames)
{
    openXMLFile(xmlFilename);
}

WriteXml::~WriteXml()
{
    xmlFile << "</loudness>\n";
    xmlFile.close();
}

void WriteXml::writeResults(const std::string& channelType, Loudness::analyser::LoudnessAnalyser& analyser)
{
    xmlFile << "<Program filename=\"";
    size_t i;
    for(i = 0; i < srcAudioFilenames.size() - 1; ++i)
    {
        xmlFile << replaceXmlSpecialCharacters(srcAudioFilenames.at(i)).c_str() << " - ";
    }
    xmlFile << replaceXmlSpecialCharacters(srcAudioFilenames.at(i)).c_str();

    xmlFile << "\" " << printStandard(analyser.getStandard()) << " " << convertValid(analyser.isValidProgram()) << " "
            << "channelsType=\"" << channelType << "\" "
            << "correctionGain=\"" << analyser.getCorrectionGain() << "\" "
            << "date=\"" << getDate() << "\">\n";
    xmlFile << "\t<ProgramLoudness " << convertValid(analyser.isIntegratedLoudnessValid()) << ">"
            << analyser.getIntegratedLoudness() << "</ProgramLoudness>\n";
    xmlFile << "\t<LRA " << convertValid(analyser.isIntegratedLoudnessRangeValid()) << ">" << analyser.getIntegratedRange()
            << "</LRA>\n";
    xmlFile << "\t<MaxMomentaryLoudness " << convertValid(analyser.isMomentaryLoudnessValid()) << ">"
            << analyser.getMomentaryLoudness() << "</MaxMomentaryLoudness>\n";
    xmlFile << "\t<MaxShortTermLoudness " << convertValid(analyser.isMaxShortTermLoudnessValid()) << ">"
            << analyser.getMaxShortTermLoudness() << "</MaxShortTermLoudness>\n";
    xmlFile << "\t<MinShortTermLoudness " << convertValid(analyser.isMinShortTermLoudnessValid()) << ">"
            << analyser.getMinShortTermLoudness() << "</MinShortTermLoudness>\n";
    xmlFile << "\t<TruePeak " << convertValid(analyser.isTruePeakValid()) << ">" << analyser.getTruePeakInDbTP()
            << "</TruePeak>\n";
    xmlFile << "\t<MaxShortTermValues>" << writeValues(analyser.getShortTermValues()) << "</MaxShortTermValues>\n";
    xmlFile << "\t<TruePeakValues>" << writeValues(analyser.getTruePeakValues()) << "</TruePeakValues>\n";
    xmlFile << "</Program>\n";
}

void WriteXml::openXMLFile(const std::string& xmlFilename)
{
    xmlFile.precision(1);
    xmlFile.setf(std::ios::fixed, std::ios::floatfield);
    xmlFile.open(xmlFilename.c_str());
    xmlFile << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n";
    xmlFile << "<loudness>\n";
}

std::string WriteXml::convertValid(Loudness::analyser::ELoudnessResult result)
{
    std::string st = "";
    switch(result)
    {
        case Loudness::analyser::eValidResult:
            st = "valid";
            break;
        case Loudness::analyser::eNotValidResult:
            st = "illegal";
            break;
        case Loudness::analyser::eNotValidResultButNotIllegal:
            st = "not illegal";
            break;
        case Loudness::analyser::eNoImportance:
            st = "no importance";
            break;
    }
    return "status=\'" + st + "'";
}

std::string WriteXml::printStandard(Loudness::analyser::EStandard standard)
{
    std::string st = "";
    switch(standard)
    {
        case Loudness::analyser::eStandardCST_R017:
            st = "CST-R017";
            break;
        case Loudness::analyser::eStandardEBU_R128:
            st = "EBU-R128";
            break;
        case Loudness::analyser::eStandardATSC_A85:
            st = "ATSC-A85";
            break;
        case Loudness::analyser::eStandardUnknown:
            st = "Unknown";
            break;
    }
    return " standard=\'" + st + "'";
}

std::string WriteXml::writeValues(std::vector<float> datas)
{
    std::ostringstream ss;
    if(datas.size() == 0)
        return ss.str();

    std::vector<float>::iterator it;
    for(it = datas.begin(); it < datas.end() - 1; it++)
        ss << *it << ", ";
    ss << datas.at(datas.size() - 1);
    return ss.str();
}

const std::string xmlSpecialCharacters[5][2] = {
    {"&", "&amp;"},
    {"<", "&lt;"},
    {">", "&gt;"},
    {"\"", "&quot;"},
    {"\'", "&apos;"}
};

std::string WriteXml::replaceXmlSpecialCharacters(std::string& text) {
    for (int i = 0; i < 5; ++i) {
        size_t position = 0;
        const std::string specChar = xmlSpecialCharacters[i][0];
        const std::string code = xmlSpecialCharacters[i][1];
        while((position = text.find(specChar, position)) != std::string::npos) {
            text.replace(position, specChar.length(), code);
            position += code.length();
        }
    }
    return text;
}

std::string WriteXml::getDate()
{
    std::string date = "";
    time_t now;
    struct tm* timeInfo;
    char buffer[32];

    time(&now);
    timeInfo = localtime(&now);
    if(std::strftime(buffer, 32, "%a, %d.%m.%Y %H:%M:%S", timeInfo) != 0)
        date.assign(buffer);
    return date;
}
}
}
