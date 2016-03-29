#include <loudnessIO/SoundFile.hpp>
#include <loudnessIO/ProcessFile.hpp>
#include <loudnessAnalyser/LoudnessAnalyser.hpp>

#include "gtest/gtest.h"

#include <string>
#include <cmath>
#include <stdlib.h>

#define STR(X) #X
#define STRINGIFY(X) STR(X)

/**
 * @brief Callback used while processing, to check progression value of the loudness analysis.
 */
void checkProgress(const int p)
{
    EXPECT_GE(p, 0);
    EXPECT_LE(p, 100);
}

class FileConfiguration
{
public:
    FileConfiguration()
    {}

    FileConfiguration( const std::string& filename )
        : _filename( filename )
        , _integratedLoudness( LOUDNESS_NAN )
        , _integratedRange( LOUDNESS_NAN )
    {}

    void setIntegratedLoudness( const double integratedLoudness )
    {
        _integratedLoudness = integratedLoudness;
    }

    void setIntegratedRange( const double integratedRange )
    {
        _integratedRange = integratedRange;
    }

    const double getIntegratedLoudness()
    {
        return _integratedLoudness;
    }

    const double getIntegratedRange()
    {
        return _integratedRange;
    }

    const char* getFilename()
    {
        return _filename.c_str();
    }

private:
    std::string _filename;
    double _integratedLoudness;
    double _integratedRange;
};

class CaseLoudnessAnalysis
    : public ::testing::TestWithParam<FileConfiguration>
{
public:
    CaseLoudnessAnalysis()
        : _level( Loudness::analyser::LoudnessLevels::Loudness_EBU_R128() )
        , _loudness( _level )
    {}

    void SetUp( )
    {
        _cfg = GetParam();
        processAnalysis();
    }

    void TearDown( )
    {
    }

    void processAnalysis()
    {
        std::string absoluteFilename = STRINGIFY( EBU_TEST_ESSENCES );
        absoluteFilename += "/";
        absoluteFilename += _cfg.getFilename();
        Loudness::io::SoundFile audioFile;

        std::cout << absoluteFilename << std::endl;
        if(!audioFile.open_read(absoluteFilename.c_str()))
        {
            Loudness::io::AnalyseFile analyser(_loudness, audioFile);
            analyser(checkProgress);
            audioFile.close();
        }
    }

    Loudness::analyser::LoudnessLevels _level;
    Loudness::analyser::LoudnessAnalyser _loudness;
    FileConfiguration _cfg;
};


std::vector<FileConfiguration> getEbuEssences()
{
    std::vector<FileConfiguration> essences;

    FileConfiguration source_1kHz_20LUFS( "1kHz Sine -20 LUFS-16bit.wav" );
    source_1kHz_20LUFS.setIntegratedLoudness( -20.0f );
    FileConfiguration source_1kHz_26LUFS( "1kHz Sine -26 LUFS-16bit.wav" );
    source_1kHz_26LUFS.setIntegratedLoudness( -26.0f );
    FileConfiguration source_1kHz_40LUFS( "1kHz Sine -40 LUFS-16bit.wav" );
    source_1kHz_40LUFS.setIntegratedLoudness( -40.0f );

    FileConfiguration source_3341_1( "seq-3341-1-16bit.wav" );
    source_3341_1.setIntegratedLoudness( -23.0f );

    FileConfiguration source_3341_2( "seq-3341-2-16bit.wav" );
    source_3341_2.setIntegratedLoudness( -33.0f );


    FileConfiguration source_3341_3( "seq-3341-3-16bit-v02.wav" );
    source_3341_3.setIntegratedLoudness( -23.0f );
    FileConfiguration source_3341_4( "seq-3341-4-16bit-v02.wav" );
    source_3341_4.setIntegratedLoudness( -23.0f );
    FileConfiguration source_3341_5( "seq-3341-5-16bit-v02.wav" );
    source_3341_5.setIntegratedLoudness( -23.0f );


    FileConfiguration source_3341_6_1( "seq-3341-6-5channels-16bit.wav" );
    source_3341_6_1.setIntegratedLoudness( -23.0f );
    FileConfiguration source_3341_6_2( "seq-3341-6-6channels-WAVEEX-16bit.wav" );
    // source_3341_6_2.setIntegratedLoudness( -23.0f );
    FileConfiguration source_3341_7( "seq-3341-7_seq-3342-5-24bit.wav" );
    source_3341_7.setIntegratedLoudness( -23.0f );

    FileConfiguration source_3341_9( "seq-3341-9-24bit.wav" );
    source_3341_9.setIntegratedLoudness( -23.0f );


    essences.push_back( source_1kHz_20LUFS );
    essences.push_back( source_1kHz_26LUFS );
    essences.push_back( source_1kHz_40LUFS );
    essences.push_back( source_3341_1 );
    essences.push_back( source_3341_2 );
    essences.push_back( source_3341_3 );
    essences.push_back( source_3341_4 );
    essences.push_back( source_3341_5 );
    essences.push_back( source_3341_6_1 );
    essences.push_back( source_3341_6_2 );
    essences.push_back( source_3341_7 );
    essences.push_back( source_3341_9 );
    return essences;
}

INSTANTIATE_TEST_CASE_P(EbuTestEssences,
                        CaseLoudnessAnalysis,
                        ::testing::ValuesIn(getEbuEssences()));

TEST_P(CaseLoudnessAnalysis, Test)
{
    if( ! isnan( _cfg.getIntegratedLoudness() ) )
    {
        ASSERT_NEAR( _loudness.getIntegratedLoudness(), _cfg.getIntegratedLoudness(), 0.1 );
    }
    if( ! isnan( _cfg.getIntegratedRange() ) )
    {
        ASSERT_NEAR( _loudness.getIntegratedRange(), _cfg.getIntegratedRange(), 0.1 );
    }


    // double getMaxShortTermLoudness();
    // double getMinShortTermLoudness();
    // double getMomentaryLoudness();
    // double getTruePeakValue();
    // double getTruePeakInDbTP();
}

int main(int argc, char** argv)
{
    // Initialize GTest system
    ::testing::InitGoogleTest(&argc, argv);

    // Run GTests
    return RUN_ALL_TESTS();
}
