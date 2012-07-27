#ifndef WRITE_XML_H
#define WRITE_XML_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <loudnessLibrary.h>

class WriteXml
{
public:
	WriteXml( const char* filename, const char* srcAudioFilename );
	
	~WriteXml();
	
	void writeResults( const char* channelType, Loudness::LoudnessLibrary& analyser );
	
private:
	std::string convertValid( Loudness::ELoudnessResult result );
	std::string printStandard( Loudness::EStandard standard );
	std::string writeValues( std::vector<float> datas );
	
	std::ofstream xmlFile;
	const char* filename;
	const char* srcAudioFilename;
};

#endif
