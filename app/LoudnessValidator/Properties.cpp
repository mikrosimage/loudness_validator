#include "Properties.h"

#include <loudnessAnalyser/LoudnessAnalyser.hpp>

#include <iostream>
#include <cmath>
#include <limits>

#define LOUDNESS_NAN std::numeric_limits<float>::quiet_NaN()

Properties::Properties(QWidget* parent, Qt::WindowFlags f)
    : QDialog(parent, f)
    , normalisation(0)
    , programLoudnessLongProgramMaxValue(0)
    , programLoudnessLongProgramMinValue(0)
    , programLoudnessLongProgramTargetLevel(0)
    , programLoudnessLongProgramTargetMaxLevel(0)
    , programLoudnessLongProgramTargetMinLevel(0)
    , shortTermLoudnessLongProgramMaxValue(0)
    , shortTermLoudnessLongProgramMinValue(0)
    , programLoudnessShortProgramMaxValue(0)
    , programLoudnessShortProgramMinValue(0)
    , programLoudnessShortProgramTargetLevel(0)
    , programLoudnessShortProgramTargetMaxLevel(0)
    , programLoudnessShortProgramTargetMinLevel(0)
    , shortTermLoudnessShortProgramMaxValue(0)
    , truePeakMaxValue(0)
    , truePeakTargetLevel(0)
    , maximalLoudnessRange(0)
    , minimalLoudnessRange(0)
    , frequencyTruePeak(0)
    , layoutParameters()
    , tabProperties()
    , frameLongProgram()
    , frameShortProgram()
    , frameAllProgram()
    , frameProcessProperties()
    , layoutLongProgram(&frameLongProgram)
    , layoutShortProgram(&frameShortProgram)
    , layoutAllProgram(&frameAllProgram)
    , layoutProcessProperties(&frameProcessProperties)
    , groupCorrectLongProgram(tr("correct"))
    , groupCorrectShortProgram(tr("correct"))
    , groupCorrectAllsProgram(tr("correct"))
    , gridCorrectLongProgram(&groupCorrectLongProgram)
    , gridCorrectShortProgram(&groupCorrectShortProgram)
    , gridCorrectAllsProgram(&groupCorrectAllsProgram)
    , comboSelectNormalisation()
    , labelProgramLoudnessLongProgramMaxValue(tr("<b><font color='red'>max</font></b> program loudness"))
    , labelProgramLoudnessLongProgramMinValue(tr("<b><font color='green'>min</font></b> program loudness"))
    , labelProgramLoudnessLongProgramTargetLevel(tr("<b><font color='blue'>target</font></b> program loudness"))
    , labelProgramLoudnessLongProgramTargetMaxLevel(
          tr("<b><font color='blue'>target</font> <font color='red'>max</font></b> program loudness"))
    , labelProgramLoudnessLongProgramTargetMinLevel(
          tr("<b><font color='blue'>target</font> <font color='green'>min</font></b> program loudness"))
    , labelShortTermLoudnessLongProgramMaxValue(tr("<b><font color='red'>max</font></b> short-term loudness"))
    , labelShortTermLoudnessLongProgramMinValue(tr("<b><font color='green'>min</font></b> short-term loudness"))
    , labelProgramLoudnessShortProgramMaxValue(tr("<b><font color='red'>max</font></b> program loudness"))
    , labelProgramLoudnessShortProgramMinValue(tr("<b><font color='green'>min</font></b> program loudness"))
    , labelProgramLoudnessShortProgramTargetLevel(tr("<b><font color='blue'>target</font></b> program loudness"))
    , labelProgramLoudnessShortProgramTargetMaxLevel(
          tr("<b><font color='blue'>target</font> <font color='red'>max</font></b> program loudness"))
    , labelProgramLoudnessShortProgramTargetMinLevel(
          tr("<b><font color='blue'>target</font> <font color='green'>min</font></b> program loudness"))
    , labelShortTermLoudnessShortProgramMaxValue(tr("<b><font color='red'>max</font></b> short-term loudness"))
    , labelTruePeakMaxValue(tr("<b><font color='red'>max</font></b> true peak value"))
    , labelTruePeakTargetLevel(tr("<b><font color='blue'>target</font></b> true peak"))
    , labelAbsoluteThresholdValue(tr("<b><font color='red'>absolute</font></b> threshold value"))
    , labelRelativeThresholdValue(tr("<b><font color='red'>relative</font></b> threshold value"))
    , labelMaximalLoudnessRange(tr("<b><font color='red'>max</font></b> loudness Range (LRA)"))
    , labelMinimalLoudnessRange(tr("<b><font color='green'>min</font></b> loudness Range (LRA)"))
    , spinProgramLoudnessLongProgramMaxValue()
    , spinProgramLoudnessLongProgramMinValue()
    , spinProgramLoudnessLongProgramTargetLevel()
    , spinProgramLoudnessLongProgramTargetMaxLevel()
    , spinProgramLoudnessLongProgramTargetMinLevel()
    , spinShortTermLoudnessLongProgramMaxValue()
    , spinShortTermLoudnessLongProgramMinValue()
    , spinProgramLoudnessShortProgramMaxValue()
    , spinProgramLoudnessShortProgramMinValue()
    , spinProgramLoudnessShortProgramTargetLevel()
    , spinProgramLoudnessShortProgramTargetMaxLevel()
    , spinProgramLoudnessShortProgramTargetMinLevel()
    , spinShortTermLoudnessShortProgramMaxValue()
    , spinTruePeakMaxValue()
    , spinTruePeakTargetLevel()
    , spinAbsoluteThresholdValue()
    , spinRelativeThresholdValue()
    , spinMaximalLoudnessRange()
    , spinMinimalLoudnessRange()
    , labelProgramLoudnessLongProgramMaxUnit(tr("LUFS"))
    , labelProgramLoudnessLongProgramMinUnit(tr("LUFS"))
    , labelProgramLoudnessLongProgramTargetLevelUnit(tr("LUFS"))
    , labelProgramLoudnessLongProgramTargetMaxLevelUnit(tr("LUFS"))
    , labelProgramLoudnessLongProgramTargetMinLevelUnit(tr("LUFS"))
    , labelShortTermLoudnessLongProgramMaxUnit(tr("LU"))
    , labelShortTermLoudnessLongProgramMinUnit(tr("LU"))
    , labelProgramLoudnessShortProgramMaxUnit(tr("LUFS"))
    , labelProgramLoudnessShortProgramMinUnit(tr("LUFS"))
    , labelProgramLoudnessShortProgramTargetLevelUnit(tr("LUFS"))
    , labelProgramLoudnessShortProgramTargetMaxLevelUnit(tr("LUFS"))
    , labelProgramLoudnessShortProgramTargetMinLevelUnit(tr("LUFS"))
    , labelShortTermLoudnessShortProgramMaxUnit(tr("LU"))
    , labelTruePeakMaxUnit(tr("dBTP"))
    , labelTruePeakTargetLevelUnit(tr("dBTP"))
    , labelAbsoluteThresholdUnit(tr("LUFS"))
    , labelRelativeThresholdUnit(tr("LU"))
    , labelMaximalLoudnessRangeUnit(tr("LU"))
    , labelMinimalLoudnessRangeUnit(tr("LU"))
    , labelFrequencyTruePeak(tr("upsampling frequency for True Peak Meter"))
    , comboFrequencyTruePeak()
    ,

    endOfEdit(QIcon(":/icons/dialog-close.png"), tr("Close"))
{
    setWindowTitle("PLoud Validator - Properties");
    setMinimumWidth(600);
    setMinimumHeight(400);

    setLayout(&layoutParameters);

    comboSelectNormalisation.addItem("France CST - RT 017", 0);
    comboSelectNormalisation.addItem("EBU R128", 1);
    comboSelectNormalisation.addItem("ATSC A85", 2);
    comboSelectNormalisation.addItem("custom", 3);

    tabProperties.addTab(&frameLongProgram, tr("long program"));
    tabProperties.addTab(&frameShortProgram, tr("short program"));
    tabProperties.addTab(&frameAllProgram, tr("all programs"));
    tabProperties.addTab(&frameProcessProperties, tr("process properties"));

    labelProgramLoudnessLongProgramMaxValue.setTextFormat(Qt::RichText);
    labelProgramLoudnessLongProgramMinValue.setTextFormat(Qt::RichText);
    labelProgramLoudnessLongProgramTargetLevel.setTextFormat(Qt::RichText);
    labelProgramLoudnessLongProgramTargetMaxLevel.setTextFormat(Qt::RichText);
    labelProgramLoudnessLongProgramTargetMinLevel.setTextFormat(Qt::RichText);
    labelShortTermLoudnessLongProgramMaxValue.setTextFormat(Qt::RichText);
    labelShortTermLoudnessLongProgramMinValue.setTextFormat(Qt::RichText);

    labelProgramLoudnessShortProgramMaxValue.setTextFormat(Qt::RichText);
    labelProgramLoudnessShortProgramTargetLevel.setTextFormat(Qt::RichText);
    labelProgramLoudnessShortProgramTargetMaxLevel.setTextFormat(Qt::RichText);
    labelProgramLoudnessShortProgramTargetMinLevel.setTextFormat(Qt::RichText);
    labelShortTermLoudnessShortProgramMaxValue.setTextFormat(Qt::RichText);

    labelTruePeakMaxValue.setTextFormat(Qt::RichText);
    labelTruePeakTargetLevel.setTextFormat(Qt::RichText);
    labelAbsoluteThresholdValue.setTextFormat(Qt::RichText);
    labelRelativeThresholdValue.setTextFormat(Qt::RichText);
    labelMaximalLoudnessRange.setTextFormat(Qt::RichText);
    labelMinimalLoudnessRange.setTextFormat(Qt::RichText);

    spinProgramLoudnessLongProgramMaxValue.setMinimum(-200.0);
    spinProgramLoudnessLongProgramMinValue.setMinimum(-200.0);
    spinProgramLoudnessLongProgramTargetLevel.setMinimum(-200.0);
    spinProgramLoudnessLongProgramTargetMaxLevel.setMinimum(-200.0);
    spinProgramLoudnessLongProgramTargetMinLevel.setMinimum(-200.0);
    spinShortTermLoudnessLongProgramMaxValue.setMinimum(-200.0);
    spinShortTermLoudnessLongProgramMinValue.setMinimum(-200.0);
    spinProgramLoudnessShortProgramMaxValue.setMinimum(-200.0);
    spinProgramLoudnessShortProgramMinValue.setMinimum(-200.0);
    spinProgramLoudnessShortProgramTargetLevel.setMinimum(-200.0);
    spinProgramLoudnessShortProgramTargetMaxLevel.setMinimum(-200.0);
    spinProgramLoudnessShortProgramTargetMinLevel.setMinimum(-200.0);
    spinShortTermLoudnessShortProgramMaxValue.setMinimum(-200.0);
    spinTruePeakMaxValue.setMinimum(-200.0);
    spinTruePeakTargetLevel.setMinimum(-200.0);
    spinAbsoluteThresholdValue.setMinimum(-200.0);
    spinRelativeThresholdValue.setMinimum(-200.0);
    spinMaximalLoudnessRange.setMinimum(-200.0);
    spinMinimalLoudnessRange.setMinimum(-200.0);

    spinProgramLoudnessLongProgramMaxValue.setDecimals(1);
    spinProgramLoudnessLongProgramMinValue.setDecimals(1);
    spinProgramLoudnessLongProgramTargetLevel.setDecimals(1);
    spinProgramLoudnessLongProgramTargetMaxLevel.setDecimals(1);
    spinProgramLoudnessLongProgramTargetMinLevel.setDecimals(1);
    spinShortTermLoudnessLongProgramMaxValue.setDecimals(1);
    spinShortTermLoudnessLongProgramMinValue.setDecimals(1);
    spinProgramLoudnessShortProgramMaxValue.setDecimals(1);
    spinProgramLoudnessShortProgramTargetLevel.setDecimals(1);
    spinProgramLoudnessShortProgramTargetMaxLevel.setDecimals(1);
    spinProgramLoudnessShortProgramTargetMinLevel.setDecimals(1);
    spinProgramLoudnessShortProgramMinValue.setDecimals(1);
    spinShortTermLoudnessShortProgramMaxValue.setDecimals(1);
    spinTruePeakMaxValue.setDecimals(1);
    spinTruePeakTargetLevel.setDecimals(1);
    spinAbsoluteThresholdValue.setDecimals(1);
    spinRelativeThresholdValue.setDecimals(1);
    spinMaximalLoudnessRange.setDecimals(1);
    spinMinimalLoudnessRange.setDecimals(1);

    comboFrequencyTruePeak.addItem("192kHz", QVariant(192000));
    comboFrequencyTruePeak.addItem("384kHz", QVariant(384000));

    setEnableProperties(false);

    // long programs
    int line = 0;
    int col = 0;
    gridCorrectLongProgram.addWidget(&labelProgramLoudnessLongProgramTargetLevel, line++, col);
    gridCorrectLongProgram.addWidget(&labelProgramLoudnessLongProgramTargetMaxLevel, line++, col);
    gridCorrectLongProgram.addWidget(&labelProgramLoudnessLongProgramTargetMinLevel, line++, col);
    line = 0;
    col++;
    gridCorrectLongProgram.addWidget(&spinProgramLoudnessLongProgramTargetLevel, line++, col);
    gridCorrectLongProgram.addWidget(&spinProgramLoudnessLongProgramTargetMaxLevel, line++, col);
    gridCorrectLongProgram.addWidget(&spinProgramLoudnessLongProgramTargetMinLevel, line++, col);
    line = 0;
    col++;
    gridCorrectLongProgram.addWidget(&labelProgramLoudnessLongProgramTargetLevelUnit, line++, col);
    gridCorrectLongProgram.addWidget(&labelProgramLoudnessLongProgramTargetMaxLevelUnit, line++, col);
    gridCorrectLongProgram.addWidget(&labelProgramLoudnessLongProgramTargetMinLevelUnit, line++, col);

    line = 0;
    col = 0;
    layoutLongProgram.addWidget(&labelProgramLoudnessLongProgramMaxValue, line++, col);
    layoutLongProgram.addWidget(&labelProgramLoudnessLongProgramMinValue, line++, col);
    layoutLongProgram.addWidget(&labelShortTermLoudnessLongProgramMaxValue, line++, col);
    layoutLongProgram.addWidget(&labelShortTermLoudnessLongProgramMinValue, line++, col);
    layoutLongProgram.addWidget(&groupCorrectLongProgram, line++, col, 1, 3);
    line = 0;
    col++;
    layoutLongProgram.addWidget(&spinProgramLoudnessLongProgramMaxValue, line++, col);
    layoutLongProgram.addWidget(&spinProgramLoudnessLongProgramMinValue, line++, col);
    layoutLongProgram.addWidget(&spinShortTermLoudnessLongProgramMaxValue, line++, col);
    layoutLongProgram.addWidget(&spinShortTermLoudnessLongProgramMinValue, line++, col);
    line = 0;
    col++;
    layoutLongProgram.addWidget(&labelProgramLoudnessLongProgramMaxUnit, line++, col);
    layoutLongProgram.addWidget(&labelProgramLoudnessLongProgramMinUnit, line++, col);
    layoutLongProgram.addWidget(&labelShortTermLoudnessLongProgramMaxUnit, line++, col);
    layoutLongProgram.addWidget(&labelShortTermLoudnessLongProgramMinUnit, line++, col);

    // short programs
    line = 0;
    col = 0;
    gridCorrectShortProgram.addWidget(&labelProgramLoudnessShortProgramTargetLevel, line++, col);
    gridCorrectShortProgram.addWidget(&labelProgramLoudnessShortProgramTargetMaxLevel, line++, col);
    gridCorrectShortProgram.addWidget(&labelProgramLoudnessShortProgramTargetMinLevel, line++, col);
    line = 0;
    col++;
    gridCorrectShortProgram.addWidget(&spinProgramLoudnessShortProgramTargetLevel, line++, col);
    gridCorrectShortProgram.addWidget(&spinProgramLoudnessShortProgramTargetMaxLevel, line++, col);
    gridCorrectShortProgram.addWidget(&spinProgramLoudnessShortProgramTargetMinLevel, line++, col);
    line = 0;
    col++;
    gridCorrectShortProgram.addWidget(&labelProgramLoudnessShortProgramTargetLevelUnit, line++, col);
    gridCorrectShortProgram.addWidget(&labelProgramLoudnessShortProgramTargetMaxLevelUnit, line++, col);
    gridCorrectShortProgram.addWidget(&labelProgramLoudnessShortProgramTargetMinLevelUnit, line++, col);

    line = 0;
    col = 0;
    layoutShortProgram.addWidget(&labelProgramLoudnessShortProgramMaxValue, line++, col);
    layoutShortProgram.addWidget(&labelProgramLoudnessShortProgramMinValue, line++, col);
    layoutShortProgram.addWidget(&labelShortTermLoudnessShortProgramMaxValue, line++, col);
    layoutShortProgram.addWidget(&groupCorrectShortProgram, line++, col, 1, 3);
    line = 0;
    col++;
    layoutShortProgram.addWidget(&spinProgramLoudnessShortProgramMaxValue, line++, col);
    layoutShortProgram.addWidget(&spinProgramLoudnessShortProgramMinValue, line++, col);
    layoutShortProgram.addWidget(&spinShortTermLoudnessShortProgramMaxValue, line++, col);
    line = 0;
    col++;
    layoutShortProgram.addWidget(&labelProgramLoudnessShortProgramMaxUnit, line++, col);
    layoutShortProgram.addWidget(&labelProgramLoudnessShortProgramMinUnit, line++, col);
    layoutShortProgram.addWidget(&labelShortTermLoudnessShortProgramMaxUnit, line++, col);

    // all programs
    line = 0;
    col = 0;
    gridCorrectAllsProgram.addWidget(&labelTruePeakTargetLevel, line++, col);
    line = 0;
    col++;
    gridCorrectAllsProgram.addWidget(&spinTruePeakTargetLevel, line++, col);
    line = 0;
    col++;
    gridCorrectAllsProgram.addWidget(&labelTruePeakTargetLevelUnit, line++, col);

    line = 0;
    col = 0;
    layoutAllProgram.addWidget(&labelTruePeakMaxValue, line++, col);
    layoutAllProgram.addWidget(&labelMaximalLoudnessRange, line++, col);
    layoutAllProgram.addWidget(&labelMinimalLoudnessRange, line++, col);
    layoutAllProgram.addWidget(&groupCorrectAllsProgram, line++, col, 1, 3);
    line = 0;
    col++;
    layoutAllProgram.addWidget(&spinTruePeakMaxValue, line++, col);
    layoutAllProgram.addWidget(&spinMaximalLoudnessRange, line++, col);
    layoutAllProgram.addWidget(&spinMinimalLoudnessRange, line++, col);
    line = 0;
    col++;
    layoutAllProgram.addWidget(&labelTruePeakMaxUnit, line++, col);
    layoutAllProgram.addWidget(&labelMaximalLoudnessRangeUnit, line++, col);
    layoutAllProgram.addWidget(&labelMinimalLoudnessRangeUnit, line++, col);

    line = 0;
    col = 0;
    layoutProcessProperties.addWidget(&labelAbsoluteThresholdValue, line++, col);
    layoutProcessProperties.addWidget(&labelRelativeThresholdValue, line++, col);
    layoutProcessProperties.addWidget(&labelFrequencyTruePeak, line++, col);
    line = 0;
    col++;
    layoutProcessProperties.addWidget(&spinAbsoluteThresholdValue, line++, col);
    layoutProcessProperties.addWidget(&spinRelativeThresholdValue, line++, col);
    layoutProcessProperties.addWidget(&comboFrequencyTruePeak, line++, col);
    line = 0;
    col++;
    layoutProcessProperties.addWidget(&labelAbsoluteThresholdUnit, line++, col);
    layoutProcessProperties.addWidget(&labelRelativeThresholdUnit, line++, col);

    line = 0;
    col = 0;
    layoutParameters.addWidget(&comboSelectNormalisation, line++, col);
    layoutParameters.addWidget(&tabProperties, line++, col);

    layoutParameters.addWidget(&endOfEdit, line++, col);

    setDefaultValues(comboSelectNormalisation.currentIndex());

    connect(&endOfEdit, SIGNAL(pressed()), this, SLOT(closeDialog()));

    connect(&endOfEdit, SIGNAL(pressed()), this, SLOT(closeDialog()));
    connect(&comboSelectNormalisation, SIGNAL(currentIndexChanged(int)), this, SLOT(setDefaultValues(int)));
}

void Properties::updateValues()
{
    programLoudnessLongProgramMaxValue = spinProgramLoudnessLongProgramMaxValue.text().length() == 0
                                             ? LOUDNESS_NAN
                                             : spinProgramLoudnessLongProgramMaxValue.value();
    programLoudnessLongProgramMinValue = spinProgramLoudnessLongProgramMinValue.text().length() == 0
                                             ? LOUDNESS_NAN
                                             : spinProgramLoudnessLongProgramMinValue.value();
    programLoudnessLongProgramTargetLevel = spinProgramLoudnessLongProgramTargetLevel.text().length() == 0
                                                ? LOUDNESS_NAN
                                                : spinProgramLoudnessLongProgramTargetLevel.value();
    programLoudnessLongProgramTargetMinLevel = spinProgramLoudnessLongProgramTargetMinLevel.text().length() == 0
                                                   ? LOUDNESS_NAN
                                                   : spinProgramLoudnessLongProgramTargetMinLevel.value();
    programLoudnessLongProgramTargetMaxLevel = spinProgramLoudnessLongProgramTargetMaxLevel.text().length() == 0
                                                   ? LOUDNESS_NAN
                                                   : spinProgramLoudnessLongProgramTargetMaxLevel.value();

    shortTermLoudnessLongProgramMaxValue = spinShortTermLoudnessLongProgramMaxValue.text().length() == 0
                                               ? LOUDNESS_NAN
                                               : spinShortTermLoudnessLongProgramMaxValue.value();
    shortTermLoudnessLongProgramMinValue = spinShortTermLoudnessLongProgramMinValue.text().length() == 0
                                               ? LOUDNESS_NAN
                                               : spinShortTermLoudnessLongProgramMinValue.value();

    programLoudnessShortProgramMaxValue = spinProgramLoudnessShortProgramMaxValue.text().length() == 0
                                              ? LOUDNESS_NAN
                                              : spinProgramLoudnessShortProgramMaxValue.value();
    programLoudnessShortProgramMinValue = spinProgramLoudnessShortProgramMinValue.text().length() == 0
                                              ? LOUDNESS_NAN
                                              : spinProgramLoudnessShortProgramMinValue.value();
    programLoudnessShortProgramTargetLevel = spinProgramLoudnessShortProgramTargetLevel.text().length() == 0
                                                 ? LOUDNESS_NAN
                                                 : spinProgramLoudnessShortProgramTargetLevel.value();
    programLoudnessShortProgramTargetMinLevel = spinProgramLoudnessShortProgramTargetMinLevel.text().length() == 0
                                                    ? LOUDNESS_NAN
                                                    : spinProgramLoudnessShortProgramTargetMinLevel.value();
    programLoudnessShortProgramTargetMaxLevel = spinProgramLoudnessShortProgramTargetMaxLevel.text().length() == 0
                                                    ? LOUDNESS_NAN
                                                    : spinProgramLoudnessShortProgramTargetMaxLevel.value();

    shortTermLoudnessShortProgramMaxValue = spinShortTermLoudnessShortProgramMaxValue.text().length() == 0
                                                ? LOUDNESS_NAN
                                                : spinShortTermLoudnessShortProgramMaxValue.value();
    truePeakMaxValue = spinTruePeakMaxValue.text().length() == 0 ? LOUDNESS_NAN : spinTruePeakMaxValue.value();
    truePeakTargetLevel = spinTruePeakTargetLevel.text().length() == 0 ? LOUDNESS_NAN : spinTruePeakTargetLevel.value();

    absoluteThreshold = spinAbsoluteThresholdValue.text().length() == 0 ? LOUDNESS_NAN : spinAbsoluteThresholdValue.value();
    relativeThreshold = spinRelativeThresholdValue.text().length() == 0 ? LOUDNESS_NAN : spinRelativeThresholdValue.value();
    maximalLoudnessRange = spinMaximalLoudnessRange.text().length() == 0 ? LOUDNESS_NAN : spinMaximalLoudnessRange.value();
    minimalLoudnessRange = spinMinimalLoudnessRange.text().length() == 0 ? LOUDNESS_NAN : spinMinimalLoudnessRange.value();
    frequencyTruePeak = comboFrequencyTruePeak.currentIndex();
}

void Properties::closeDialog()
{
    updateValues();
    emit close(normalisation);
    accept();
}

void Properties::setDefaultValues(int norm)
{
    normalisation = norm;
    switch(normalisation)
    {
        case 0:
        {
            setEnableProperties(false);
            Loudness::analyser::LoudnessLevels levels = Loudness::analyser::LoudnessLevels::Loudness_CST_R017();
            setPropertiesValues(levels);
            break;
        }
        case 1:
        {
            setEnableProperties(false);
            Loudness::analyser::LoudnessLevels levels = Loudness::analyser::LoudnessLevels::Loudness_EBU_R128();
            setPropertiesValues(levels);
            break;
        }
        case 2:
        {
            setEnableProperties(false);
            Loudness::analyser::LoudnessLevels levels = Loudness::analyser::LoudnessLevels::Loudness_ATSC_A85();
            setPropertiesValues(levels);
            break;
        }
        case 3:
            setEnableProperties(true);
            break;
        default:
            setEnableProperties(false);
            break;
    }
    updateValues();
}

void Properties::setEnableProperties(bool enable)
{
    spinProgramLoudnessLongProgramMinValue.setEnabled(enable);
    spinProgramLoudnessLongProgramMaxValue.setEnabled(enable);
    /*spinProgramLoudnessLongProgramTargetLevel.setEnabled( enable );
    spinProgramLoudnessLongProgramTargetMaxLevel.setEnabled( enable );
    spinProgramLoudnessLongProgramTargetMinLevel.setEnabled( enable );*/
    spinShortTermLoudnessLongProgramMinValue.setEnabled(enable);
    spinShortTermLoudnessLongProgramMaxValue.setEnabled(enable);
    spinProgramLoudnessShortProgramMaxValue.setEnabled(enable);
    spinProgramLoudnessShortProgramMinValue.setEnabled(enable);
    /*spinProgramLoudnessShortProgramTargetLevel.setEnabled( enable );
    spinProgramLoudnessShortProgramTargetMaxLevel.setEnabled( enable );
    spinProgramLoudnessShortProgramTargetMinLevel.setEnabled( enable );*/
    spinShortTermLoudnessShortProgramMaxValue.setEnabled(enable);
    spinTruePeakMaxValue.setEnabled(enable);
    /*spinTruePeakTargetLevel.setEnabled    ( enable );*/
    spinAbsoluteThresholdValue.setEnabled(enable);
    spinRelativeThresholdValue.setEnabled(enable);
    spinMaximalLoudnessRange.setEnabled(enable);
    spinMinimalLoudnessRange.setEnabled(enable);
    comboFrequencyTruePeak.setEnabled(enable);
}

void Properties::setPropertiesValues(Loudness::analyser::LoudnessLevels values)
{
    spinProgramLoudnessLongProgramMaxValue.setValue(values.programLoudnessLongProgramMaxValue);
    spinProgramLoudnessLongProgramMinValue.setValue(values.programLoudnessLongProgramMinValue);
    spinProgramLoudnessLongProgramTargetLevel.setValue(values.programLoudnessLongProgramTargetLevel);
    spinProgramLoudnessLongProgramTargetMaxLevel.setValue(values.programLoudnessLongProgramTargetMaxLevel);
    spinProgramLoudnessLongProgramTargetMinLevel.setValue(values.programLoudnessLongProgramTargetMinLevel);
    spinShortTermLoudnessLongProgramMaxValue.setValue(values.shortTermLoudnessLongProgramMaxValue);
    spinShortTermLoudnessLongProgramMinValue.setValue(values.shortTermLoudnessLongProgramMinValue);
    spinProgramLoudnessShortProgramMaxValue.setValue(values.programLoudnessShortProgramMaxValue);
    spinProgramLoudnessShortProgramMinValue.setValue(values.programLoudnessShortProgramMinValue);
    spinProgramLoudnessShortProgramTargetLevel.setValue(values.programLoudnessShortProgramTargetLevel);
    spinProgramLoudnessShortProgramTargetMaxLevel.setValue(values.programLoudnessShortProgramTargetMaxLevel);
    spinProgramLoudnessShortProgramTargetMinLevel.setValue(values.programLoudnessShortProgramTargetMinLevel);
    spinShortTermLoudnessShortProgramMaxValue.setValue(values.shortTermLoudnessShortProgramMaxValue);
    spinTruePeakMaxValue.setValue(values.truePeakMaxValue);
    spinTruePeakTargetLevel.setValue(values.truePeakTargetLevel);
    spinAbsoluteThresholdValue.setValue(values.absoluteThresholdValue);
    spinRelativeThresholdValue.setValue(values.relativeThresholdValue);
    spinMaximalLoudnessRange.setValue(values.maximalLoudnessRange);
    spinMinimalLoudnessRange.setValue(values.minimalLoudnessRange);

    if(std::isnan(values.programLoudnessLongProgramMaxValue))
        spinProgramLoudnessLongProgramMaxValue.clear();
    if(std::isnan(values.programLoudnessLongProgramMinValue))
        spinProgramLoudnessLongProgramMinValue.clear();
    if(std::isnan(values.programLoudnessLongProgramTargetLevel))
        spinProgramLoudnessLongProgramTargetLevel.clear();
    if(std::isnan(values.programLoudnessLongProgramTargetMaxLevel))
        spinProgramLoudnessLongProgramTargetMaxLevel.clear();
    if(std::isnan(values.programLoudnessLongProgramTargetMinLevel))
        spinProgramLoudnessLongProgramTargetMinLevel.clear();
    if(std::isnan(values.shortTermLoudnessLongProgramMaxValue))
        spinShortTermLoudnessLongProgramMaxValue.clear();
    if(std::isnan(values.shortTermLoudnessLongProgramMinValue))
        spinShortTermLoudnessLongProgramMinValue.clear();
    if(std::isnan(values.programLoudnessShortProgramMaxValue))
        spinProgramLoudnessShortProgramMaxValue.clear();
    if(std::isnan(values.programLoudnessShortProgramTargetLevel))
        spinProgramLoudnessShortProgramTargetLevel.clear();
    if(std::isnan(values.programLoudnessShortProgramTargetMaxLevel))
        spinProgramLoudnessShortProgramTargetMaxLevel.clear();
    if(std::isnan(values.programLoudnessShortProgramTargetMinLevel))
        spinProgramLoudnessShortProgramTargetMinLevel.clear();
    if(std::isnan(values.programLoudnessShortProgramMinValue))
        spinProgramLoudnessShortProgramMinValue.clear();
    if(std::isnan(values.shortTermLoudnessShortProgramMaxValue))
        spinShortTermLoudnessShortProgramMaxValue.clear();

    if(std::isnan(values.truePeakMaxValue))
        spinTruePeakMaxValue.clear();
    /*if( std::isnan( values.truePeakTargetLevel ) )
            spinTruePeakTargetLevel.clear();*/
    if(std::isnan(values.absoluteThresholdValue))
        spinAbsoluteThresholdValue.clear();
    if(std::isnan(values.relativeThresholdValue))
        spinRelativeThresholdValue.clear();
    if(std::isnan(values.maximalLoudnessRange))
        spinMaximalLoudnessRange.clear();
    if(std::isnan(values.minimalLoudnessRange))
        spinMinimalLoudnessRange.clear();

    comboFrequencyTruePeak.setCurrentIndex(0);
}
