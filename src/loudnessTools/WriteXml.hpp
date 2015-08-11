#ifndef WRITE_XML_H
#define WRITE_XML_H

#include <loudnessAnalyser/LoudnessAnalyser.hpp>

#include <iostream>
#include <fstream>
#include <sstream>

namespace Loudness {
namespace tool {

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
	const char* srcAudioFilename;
};

}
}

#endif
