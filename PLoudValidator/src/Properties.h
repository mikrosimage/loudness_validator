#ifndef PROPERTIES_H
#define PROPERTIES_H

#include <QtGui/QtGui>

namespace Loudness
{
  class LoudnessLevels;
}

class Properties : public QDialog
{
  Q_OBJECT

public:

  Properties ( QWidget* parent = 0, Qt::WindowFlags f = 0 );

public slots:
  void            updateValues();
  void            closeDialog();

  void            setDefaultValues ( int );

public:
  int             normalisation;
  float           programLoudnessLongProgramMaxValue;
  float           programLoudnessLongProgramMinValue;
  float           shortTermLoudnessLongProgramMaxValue;
  float           shortTermLoudnessLongProgramMinValue;
  float           programLoudnessShortProgramMaxValue;
  float           programLoudnessShortProgramMinValue;
  float           shortTermLoudnessShortProgramMaxValue;
  float           truePeakMaxValue;
  float           maximalLoudnessRange;
  float           minimalLoudnessRange;

  float           relativeThreshold;
  float           absoluteThreshold;

  float getFrequencyTruePeak()
  {
    return ( frequencyTruePeak == 0 ) ? 192000.0 : 384000.0;
  }

private:
  void            setEnableProperties( bool enable );
  void            setPropertiesValues( Loudness::LoudnessLevels values );

  float           frequencyTruePeak;

private:
  QGridLayout     layoutParameters;

  QTabWidget      tabProperties;

  QFrame          frameLongProgram;
  QFrame          frameShortProgram;
  QFrame          frameAllProgram;
  QFrame          frameProcessProperties;

  QGridLayout     layoutLongProgram;
  QGridLayout     layoutShortProgram;
  QGridLayout     layoutAllProgram;
  QGridLayout     layoutProcessProperties;

  QComboBox       comboSelectNormalisation;

  QLabel          labelProgramLoudnessLongProgramMaxValue;
  QLabel          labelProgramLoudnessLongProgramMinValue;
  QLabel          labelShortTermLoudnessLongProgramMaxValue;
  QLabel          labelShortTermLoudnessLongProgramMinValue;
  QLabel          labelProgramLoudnessShortProgramMaxValue;
  QLabel          labelProgramLoudnessShortProgramMinValue;
  QLabel          labelShortTermLoudnessShortProgramMaxValue;
  QLabel          labelTruePeakMaxValue;
  QLabel          labelAbsoluteThresholdValue;
  QLabel          labelRelativeThresholdValue;
  QLabel          labelMaximalLoudnessRange;
  QLabel          labelMinimalLoudnessRange;

  QDoubleSpinBox  spinProgramLoudnessLongProgramMaxValue;
  QDoubleSpinBox  spinProgramLoudnessLongProgramMinValue;
  QDoubleSpinBox  spinShortTermLoudnessLongProgramMaxValue;
  QDoubleSpinBox  spinShortTermLoudnessLongProgramMinValue;
  QDoubleSpinBox  spinProgramLoudnessShortProgramMaxValue;
  QDoubleSpinBox  spinProgramLoudnessShortProgramMinValue;
  QDoubleSpinBox  spinShortTermLoudnessShortProgramMaxValue;
  QDoubleSpinBox  spinTruePeakMaxValue;
  QDoubleSpinBox  spinAbsoluteThresholdValue;
  QDoubleSpinBox  spinRelativeThresholdValue;
  QDoubleSpinBox  spinMaximalLoudnessRange;
  QDoubleSpinBox  spinMinimalLoudnessRange;

  QLabel          labelProgramLoudnessLongProgramMaxUnit;
  QLabel          labelProgramLoudnessLongProgramMinUnit;
  QLabel          labelShortTermLoudnessLongProgramMaxUnit;
  QLabel          labelShortTermLoudnessLongProgramMinUnit;
  QLabel          labelProgramLoudnessShortProgramMaxUnit;
  QLabel          labelProgramLoudnessShortProgramMinUnit;
  QLabel          labelShortTermLoudnessShortProgramMaxUnit;
  QLabel          labelTruePeakMaxUnit;
  QLabel          labelAbsoluteThresholdUnit;
  QLabel          labelRelativeThresholdUnit;
  QLabel          labelMaximalLoudnessRangeUnit;
  QLabel          labelMinimalLoudnessRangeUnit;

  QLabel          labelFrequencyTruePeak;
  QComboBox       comboFrequencyTruePeak;

  QPushButton     endOfEdit;
};

#endif // PROPERTIES_H
