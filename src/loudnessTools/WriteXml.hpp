#ifndef WRITE_XML_H
#define WRITE_XML_H

#include <loudnessAnalyser/LoudnessAnalyser.hpp>

#include <iostream>
#include <fstream>
#include <sstream>

namespace Loudness {
namespace tools {

class WriteXml
{
public:
	WriteXml( const char* filename, const char* srcAudioFilename );

	~WriteXml();

	void writeResults( const char* channelType, Loudness::analyser::LoudnessAnalyser& analyser );

private:
	std::string convertValid( Loudness::analyser::ELoudnessResult result );
	std::string printStandard( Loudness::analyser::EStandard standard );
	std::string writeValues( std::vector<float> datas );
	std::string getDate();

	std::ofstream xmlFile;
	const char* srcAudioFilename;
};

}
}

#endif
