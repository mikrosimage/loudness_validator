#include <loudnessIO/SoundFile.hpp>
#include <loudnessIO/ProcessFile.hpp>
#include <loudnessAnalyser/LoudnessAnalyser.hpp>

#include "gtest/gtest.h"

#include <string>
#include <cmath>
#include <cstdlib>
#include <sstream>

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

/**
 * @brief The file to analyse, with its expected loudness values.
 */
class FileConfiguration
{
public:
    FileConfiguration() {}

    FileConfiguration(const std::string& filename)
        : _filename(filename)
        , _integratedLoudness(LOUDNESS_NAN)
        , _integratedRange(LOUDNESS_NAN)
        , _maxShortTermLoudness(LOUDNESS_NAN)
        , _minShortTermLoudness(LOUDNESS_NAN)
        , _momentaryLoudness(LOUDNESS_NAN)
        , _truePeakValue(LOUDNESS_NAN)
        , _truePeakInDbTP(LOUDNESS_NAN)
    {
    }

    void setIntegratedLoudness(const double integratedLoudness) { _integratedLoudness = integratedLoudness; }
    void setIntegratedRange(const double integratedRange) { _integratedRange = integratedRange; }
    void setMaxShortTermLoudness(const double maxShortTermLoudness) { _maxShortTermLoudness = maxShortTermLoudness; }
    void setMinShortTermLoudness(const double minShortTermLoudness) { _minShortTermLoudness = minShortTermLoudness; }
    void setMomentaryLoudness(const double momentaryLoudness) { _momentaryLoudness = momentaryLoudness; }
    void setTruePeakValue(const double truePeakValue) { _truePeakValue = truePeakValue; }
    void setTruePeakInDbTP(const double truePeakInDbTP) { _truePeakInDbTP = truePeakInDbTP; }

    const double getIntegratedLoudness() { return _integratedLoudness; }
    const double getIntegratedRange() { return _integratedRange; }
    const double getMaxShortTermLoudness() { return _maxShortTermLoudness; }
    const double getMinShortTermLoudness() { return _minShortTermLoudness; }
    const double getMomentaryLoudness() { return _momentaryLoudness; }
    const double getTruePeakValue() { return _truePeakValue; }
    const double getTruePeakInDbTP() { return _truePeakInDbTP; }
    const char* getFilename() { return _filename.c_str(); }

private:
    std::string _filename;
    double _integratedLoudness;
    double _integratedRange;
    double _maxShortTermLoudness;
    double _minShortTermLoudness;
    double _momentaryLoudness;
    double _truePeakValue;
    double _truePeakInDbTP;
};


/**
 * @brief Fixture to manage parameterized tests based on FileConfiguration class.
 */
class CaseLoudnessAnalysis : public ::testing::TestWithParam<FileConfiguration>
{
public:
    CaseLoudnessAnalysis()
        : _level(Loudness::analyser::LoudnessLevels::Loudness_EBU_R128())
        , _loudness(_level)
    {
    }

    void SetUp()
    {
        _file = GetParam();
        processAnalysis();
    }

    void processAnalysis()
    {
        std::string absoluteFilename = STRINGIFY(EBU_TEST_ESSENCES);
        absoluteFilename += "/";
        absoluteFilename += _file.getFilename();
        Loudness::io::SoundFile audioFile;

        if(!audioFile.open_read(absoluteFilename.c_str()))
        {
            Loudness::io::AnalyseFile analyser(_loudness, audioFile);
            analyser(checkProgress);
            audioFile.close();
        }
        std::cout << absoluteFilename << std::endl;
        _loudness.printPloudValues();
    }

public:
    Loudness::analyser::LoudnessLevels _level; //< The loudness specification used.
    Loudness::analyser::LoudnessAnalyser _loudness; //< The actual loudness values.
    FileConfiguration _file; //< The expected loudness values.
};

/**
 * @return The list of file to analyse and check.
 */
std::vector<FileConfiguration> getEbuEssences()
{
    std::vector<FileConfiguration> essences;

    // 1kHz Sine cases
    FileConfiguration source_1kHz_20LUFS("1kHz Sine -20 LUFS-16bit.wav");
    source_1kHz_20LUFS.setIntegratedLoudness(-20.0f);
    source_1kHz_20LUFS.setMomentaryLoudness(-20.0f);
    source_1kHz_20LUFS.setMaxShortTermLoudness(-20.0f);
    FileConfiguration source_1kHz_26LUFS("1kHz Sine -26 LUFS-16bit.wav");
    source_1kHz_26LUFS.setIntegratedLoudness(-26.0f);
    source_1kHz_26LUFS.setMomentaryLoudness(-26.0f);
    source_1kHz_26LUFS.setMaxShortTermLoudness(-26.0f);
    FileConfiguration source_1kHz_40LUFS("1kHz Sine -40 LUFS-16bit.wav");
    source_1kHz_40LUFS.setIntegratedLoudness(-40.0f);
    source_1kHz_40LUFS.setMomentaryLoudness(-40.0f);
    source_1kHz_40LUFS.setMaxShortTermLoudness(-40.0f);

    // seq-3341 cases
    FileConfiguration source_3341_1("seq-3341-1-16bit.wav");
    source_3341_1.setIntegratedLoudness(-23.0f);
    source_3341_1.setMomentaryLoudness(-23.0f);
    source_3341_1.setMaxShortTermLoudness(-23.0f);

    FileConfiguration source_3341_2("seq-3341-2-16bit.wav");
    source_3341_2.setIntegratedLoudness(-33.0f);
    source_3341_2.setMomentaryLoudness(-33.0f);
    source_3341_2.setMaxShortTermLoudness(-33.0f);

    FileConfiguration source_3341_3("seq-3341-3-16bit-v02.wav");
    source_3341_3.setIntegratedLoudness(-23.0f);
    FileConfiguration source_3341_4("seq-3341-4-16bit-v02.wav");
    source_3341_4.setIntegratedLoudness(-23.0f);
    FileConfiguration source_3341_5("seq-3341-5-16bit-v02.wav");
    source_3341_5.setIntegratedLoudness(-23.0f);

    FileConfiguration source_3341_6_1("seq-3341-6-5channels-16bit.wav");
    source_3341_6_1.setIntegratedLoudness(-23.0f);
    FileConfiguration source_3341_6_2("seq-3341-6-6channels-WAVEEX-16bit.wav");
    // @TODO support this test
    // source_3341_6_2.setIntegratedLoudness( -23.0f );
    FileConfiguration source_3341_7("seq-3341-7_seq-3342-5-24bit.wav");
    source_3341_7.setIntegratedLoudness(-23.0f);

    FileConfiguration source_3341_9("seq-3341-9-24bit.wav");
    source_3341_9.setMaxShortTermLoudness(-23.0f);

    essences.push_back(source_1kHz_20LUFS);
    essences.push_back(source_1kHz_26LUFS);
    essences.push_back(source_1kHz_40LUFS);
    essences.push_back(source_3341_1);
    essences.push_back(source_3341_2);
    essences.push_back(source_3341_3);
    essences.push_back(source_3341_4);
    essences.push_back(source_3341_5);
    essences.push_back(source_3341_6_1);
    essences.push_back(source_3341_6_2);
    essences.push_back(source_3341_7);
    essences.push_back(source_3341_9);

    for(int i = 1; i < 21; ++i)
    {
        std::ostringstream os;
        os << "seq-3341-10-";
        os << i;
        os << "-24bit.wav";
        FileConfiguration source_3341_10(os.str());
        source_3341_10.setMaxShortTermLoudness(-23.0f);
        essences.push_back(source_3341_10);
    }

    FileConfiguration source_3341_11("seq-3341-11-24bit.wav");
    source_3341_11.setMaxShortTermLoudness(-19.0f);

    FileConfiguration source_3341_12("seq-3341-12-24bit.wav");
    source_3341_12.setMomentaryLoudness(-23.0f);

    essences.push_back(source_3341_11);
    essences.push_back(source_3341_12);

    for(int i = 1; i < 21; ++i)
    {
        std::ostringstream os;
        os << "seq-3341-13-";
        os << i;
        os << "-24bit.wav";
        if(i > 2)
        {
            os << ".wav";
        }
        FileConfiguration source_3341_13(os.str());
        // @TODO support this test
        // source_3341_13.setMomentaryLoudness( -23.0f );
        essences.push_back(source_3341_13);
    }

    FileConfiguration source_3341_14("seq-3341-14-24bit.wav.wav");
    // @TODO support this test
    // source_3341_14.setMomentaryLoudness( -19.0f );

    essences.push_back(source_3341_14);

    FileConfiguration source_3341_15("seq-3341-15-24bit.wav.wav");
    source_3341_15.setTruePeakInDbTP(-6.0f);
    FileConfiguration source_3341_16("seq-3341-16-24bit.wav.wav");
    source_3341_16.setTruePeakInDbTP(-6.0f);

    FileConfiguration source_3341_17("seq-3341-17-24bit.wav.wav");
    source_3341_17.setTruePeakInDbTP(-6.0f);
    FileConfiguration source_3341_18("seq-3341-18-24bit.wav.wav");
    source_3341_18.setTruePeakInDbTP(-6.0f);
    FileConfiguration source_3341_19("seq-3341-19-24bit.wav.wav");
    source_3341_19.setTruePeakInDbTP(+3.0f);
    FileConfiguration source_3341_20("seq-3341-20-24bit.wav.wav");
    source_3341_20.setTruePeakInDbTP(-0.0f);
    FileConfiguration source_3341_21("seq-3341-21-24bit.wav.wav");
    source_3341_21.setTruePeakInDbTP(-0.0f);
    FileConfiguration source_3341_22("seq-3341-22-24bit.wav.wav");
    source_3341_22.setTruePeakInDbTP(-0.0f);
    FileConfiguration source_3341_23("seq-3341-23-24bit.wav.wav");
    source_3341_23.setTruePeakInDbTP(-0.0f);

    essences.push_back(source_3341_15);
    essences.push_back(source_3341_16);
    essences.push_back(source_3341_17);
    essences.push_back(source_3341_18);
    essences.push_back(source_3341_19);
    essences.push_back(source_3341_20);
    essences.push_back(source_3341_21);
    essences.push_back(source_3341_22);
    essences.push_back(source_3341_23);

    // seq-3342 cases
    FileConfiguration source_3342_1("seq-3342-1-16bit.wav");
    source_3342_1.setIntegratedRange(10.0f);
    FileConfiguration source_3342_2("seq-3342-2-16bit.wav");
    source_3342_2.setIntegratedRange(5.0f);
    FileConfiguration source_3342_3("seq-3342-3-16bit.wav");
    source_3342_3.setIntegratedRange(20.0f);
    FileConfiguration source_3342_4("seq-3342-4-16bit.wav");
    source_3342_4.setIntegratedRange(15.0f);

    FileConfiguration source_3342_5("seq-3341-7_seq-3342-5-24bit.wav");
    source_3342_5.setIntegratedRange(5.0f);

    essences.push_back(source_3342_1);
    essences.push_back(source_3342_2);
    essences.push_back(source_3342_3);
    essences.push_back(source_3342_4);
    essences.push_back(source_3342_5);

    return essences;
}

/**
 * @brief Creating the tests from the given list of EBU essences.
 */
INSTANTIATE_TEST_CASE_P(EbuTestEssences, CaseLoudnessAnalysis, ::testing::ValuesIn(getEbuEssences()));


/**
 * @brief For each test generated, do those checks.
 */
TEST_P(CaseLoudnessAnalysis, Test)
{
    if(!isnan(_file.getIntegratedLoudness()))
    {
        ASSERT_NEAR(_loudness.getIntegratedLoudness(), _file.getIntegratedLoudness(), 0.1);
    }
    if(!isnan(_file.getIntegratedRange()))
    {
        ASSERT_NEAR(_loudness.getIntegratedRange(), _file.getIntegratedRange(), 1.0);
    }
    if(!isnan(_file.getMaxShortTermLoudness()))
    {
        ASSERT_NEAR(_loudness.getMaxShortTermLoudness(), _file.getMaxShortTermLoudness(), 0.1);
    }
    if(!isnan(_file.getMinShortTermLoudness()))
    {
        ASSERT_NEAR(_loudness.getMinShortTermLoudness(), _file.getMinShortTermLoudness(), 0.1);
    }
    if(!isnan(_file.getMomentaryLoudness()))
    {
        ASSERT_NEAR(_loudness.getMomentaryLoudness(), _file.getMomentaryLoudness(), 0.1);
    }
    if(!isnan(_file.getTruePeakValue()))
    {
        ASSERT_NEAR(_loudness.getTruePeakValue(), _file.getTruePeakValue(), 0.1);
    }
    if(!isnan(_file.getTruePeakInDbTP()))
    {
        ASSERT_GT(_loudness.getTruePeakInDbTP(), _file.getTruePeakInDbTP() - 0.4);
        ASSERT_LT(_loudness.getTruePeakInDbTP(), _file.getTruePeakInDbTP() + 0.2);
    }
}

int main(int argc, char** argv)
{
    // Initialize GTest system
    ::testing::InitGoogleTest(&argc, argv);

    // Run GTests
    return RUN_ALL_TESTS();
}
