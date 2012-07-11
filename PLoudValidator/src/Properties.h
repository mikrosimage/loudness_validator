#ifndef PROPERTIES_H
#define PROPERTIES_H

#include <QtGui/QtGui>

namespace Loudness
{
	struct LoudnessLevels;
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
	
signals:
	void            close( int norm );
	
public:
	int             normalisation;
	float           programLoudnessLongProgramMaxValue;
	float           programLoudnessLongProgramMinValue;
	float           programLoudnessLongProgramTargetLevel;
	float           programLoudnessLongProgramTargetMaxLevel;
	float           programLoudnessLongProgramTargetMinLevel;
	float           shortTermLoudnessLongProgramMaxValue;
	float           shortTermLoudnessLongProgramMinValue;
	float           programLoudnessShortProgramMaxValue;
	float           programLoudnessShortProgramMinValue;
	float           programLoudnessShortProgramTargetLevel;
	float           programLoudnessShortProgramTargetMaxLevel;
	float           programLoudnessShortProgramTargetMinLevel;
	float           shortTermLoudnessShortProgramMaxValue;
	float           truePeakMaxValue;
	float           truePeakTargetLevel;
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
	
	QGroupBox       groupCorrectLongProgram;
	QGroupBox       groupCorrectShortProgram;
	QGroupBox       groupCorrectAllsProgram;
	
	QGridLayout     gridCorrectLongProgram;
	QGridLayout     gridCorrectShortProgram;
	QGridLayout     gridCorrectAllsProgram;
	
	QComboBox       comboSelectNormalisation;
	
	QLabel          labelProgramLoudnessLongProgramMaxValue;
	QLabel          labelProgramLoudnessLongProgramMinValue;
	QLabel          labelProgramLoudnessLongProgramTargetLevel;
	QLabel          labelProgramLoudnessLongProgramTargetMaxLevel;
	QLabel          labelProgramLoudnessLongProgramTargetMinLevel;
	QLabel          labelShortTermLoudnessLongProgramMaxValue;
	QLabel          labelShortTermLoudnessLongProgramMinValue;
	QLabel          labelProgramLoudnessShortProgramMaxValue;
	QLabel          labelProgramLoudnessShortProgramMinValue;
	QLabel          labelProgramLoudnessShortProgramTargetLevel;
	QLabel          labelProgramLoudnessShortProgramTargetMaxLevel;
	QLabel          labelProgramLoudnessShortProgramTargetMinLevel;
	QLabel          labelShortTermLoudnessShortProgramMaxValue;
	QLabel          labelTruePeakMaxValue;
	QLabel          labelTruePeakTargetLevel;
	QLabel          labelAbsoluteThresholdValue;
	QLabel          labelRelativeThresholdValue;
	QLabel          labelMaximalLoudnessRange;
	QLabel          labelMinimalLoudnessRange;
	
	QDoubleSpinBox  spinProgramLoudnessLongProgramMaxValue;
	QDoubleSpinBox  spinProgramLoudnessLongProgramMinValue;
	QDoubleSpinBox  spinProgramLoudnessLongProgramTargetLevel;
	QDoubleSpinBox  spinProgramLoudnessLongProgramTargetMaxLevel;
	QDoubleSpinBox  spinProgramLoudnessLongProgramTargetMinLevel;
	QDoubleSpinBox  spinShortTermLoudnessLongProgramMaxValue;
	QDoubleSpinBox  spinShortTermLoudnessLongProgramMinValue;
	QDoubleSpinBox  spinProgramLoudnessShortProgramMaxValue;
	QDoubleSpinBox  spinProgramLoudnessShortProgramMinValue;
	QDoubleSpinBox  spinProgramLoudnessShortProgramTargetLevel;
	QDoubleSpinBox  spinProgramLoudnessShortProgramTargetMaxLevel;
	QDoubleSpinBox  spinProgramLoudnessShortProgramTargetMinLevel;
	QDoubleSpinBox  spinShortTermLoudnessShortProgramMaxValue;
	QDoubleSpinBox  spinTruePeakMaxValue;
	QDoubleSpinBox  spinTruePeakTargetLevel;
	QDoubleSpinBox  spinAbsoluteThresholdValue;
	QDoubleSpinBox  spinRelativeThresholdValue;
	QDoubleSpinBox  spinMaximalLoudnessRange;
	QDoubleSpinBox  spinMinimalLoudnessRange;
	
	QLabel          labelProgramLoudnessLongProgramMaxUnit;
	QLabel          labelProgramLoudnessLongProgramMinUnit;
	QLabel          labelProgramLoudnessLongProgramTargetLevelUnit;
	QLabel          labelProgramLoudnessLongProgramTargetMaxLevelUnit;
	QLabel          labelProgramLoudnessLongProgramTargetMinLevelUnit;
	QLabel          labelShortTermLoudnessLongProgramMaxUnit;
	QLabel          labelShortTermLoudnessLongProgramMinUnit;
	QLabel          labelProgramLoudnessShortProgramMaxUnit;
	QLabel          labelProgramLoudnessShortProgramMinUnit;
	QLabel          labelProgramLoudnessShortProgramTargetLevelUnit;
	QLabel          labelProgramLoudnessShortProgramTargetMaxLevelUnit;
	QLabel          labelProgramLoudnessShortProgramTargetMinLevelUnit;
	QLabel          labelShortTermLoudnessShortProgramMaxUnit;
	QLabel          labelTruePeakMaxUnit;
	QLabel          labelTruePeakTargetLevelUnit;
	QLabel          labelAbsoluteThresholdUnit;
	QLabel          labelRelativeThresholdUnit;
	QLabel          labelMaximalLoudnessRangeUnit;
	QLabel          labelMinimalLoudnessRangeUnit;
	
	QLabel          labelFrequencyTruePeak;
	QComboBox       comboFrequencyTruePeak;
	
	QPushButton     endOfEdit;
};

#endif // PROPERTIES_H
