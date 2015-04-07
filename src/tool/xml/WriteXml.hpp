
#ifndef _LOUDNESS_ASSESSMENT_WRITE_XML_HPP_
#define _LOUDNESS_ASSESSMENT_WRITE_XML_HPP_

#include <loudnessAnalyser/LoudnessAnalyser.hpp>
#include <iostream>
#include <fstream>
#include <sstream>

class WriteXml
{
public:
	WriteXml( const char* filename, const char* srcAudioFilename );
	
	~WriteXml();
	
	void writeResults( const char* channelType, Loudness::LoudnessAnalyser& analyser );
	
private:
	std::string convertValid( Loudness::ELoudnessResult result );
	std::string printStandard( Loudness::EStandard standard );
	std::string writeValues( std::vector<float> datas );
	std::string getDate();
	
	std::ofstream xmlFile;
	const char* filename;
	const char* srcAudioFilename;
};

#endif
