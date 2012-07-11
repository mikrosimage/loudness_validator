#ifndef PLOUD_PROCESS_H
#define PLOUD_PROCESS_H

#include "loudnessLibrary.h"
#include <SoundFile.h>
#include <string>
#include <vector>

enum ELoudnessStandard
{
	eCSTR017 = 0,
	eEBUR128,
	eATSCA85
};

class PLoudProcess : public Loudness::LoudnessLibrary
{
public:
	PLoudProcess( Loudness::LoudnessLevels levels, float frequencyForTruePeak );
	
	bool openAudioFiles( std::vector<std::string>& files );
	void closeAudioFiles( );
	
	float getProgramDuration();
	int   getProgramLength();
	
	void processAnalyseFile( void (*callback)(void*, int), void* object, double gain = 1.0 );
	
	void writeFile( void (*callback)(void*, int), void* object, double gain = 1.0 );

	bool analyseToFindCorrectionGain( void (*callback)(void*, int), void* object, double& foundedGain );
	
private:
	bool                     isMultichannelFile;
	std::vector<std::string> filenames;
	std::vector<SoundFile*>  audioFiles;
};

#endif
