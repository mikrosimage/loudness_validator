#ifndef PLOUD_PROCESS_H
#define PLOUD_PROCESS_H

#include <loudnessAnalyser/LoudnessAnalyser.hpp>
#include <loudnessIO/SoundFile.hpp>

#include <string>
#include <vector>

enum ELoudnessStandard
{
    eCSTR017 = 0,
    eEBUR128,
    eATSCA85
};

class PLoudProcess : public Loudness::analyser::LoudnessAnalyser
{
public:
    PLoudProcess(Loudness::analyser::LoudnessLevels levels, float frequencyForTruePeak);

    bool openAudioFiles(std::vector<std::string>& files);
    void closeAudioFiles();

    float getProgramDuration();
    int getProgramLength();

    void processAnalyseFile(void (*callback)(void*, int), void* object, double gain = 1.0);

    void writeFile(void (*callback)(void*, int), void* object, double gain = 1.0);

    bool analyseToFindCorrectionGain(void (*callback)(void*, int), void* object, double& foundedGain);

private:
    std::vector<std::string> filenames;
    std::vector<Loudness::io::SoundFile*> audioFiles;
};

#endif
