#ifndef PLOUDGUI_H
#define PLOUDGUI_H

#include <QtGui/QtGui>
#include <Properties.h>
#include <AudioFile.h>
#include <DropAudio.h>

class QtVectorViewer;
class QtVectorHistogram;

namespace Loudness{
	class LoudnessLibrary;
}


class PLoudGui : public QMainWindow
{
	Q_OBJECT
public:
	PLoudGui( QWidget* parent = 0 );
	virtual ~PLoudGui();
	
private:
	bool rlmSesame();
	
private slots:
	void openSeparatedFiles   ( );
	void openMultichannelFile ( );
	void updateInterface      ( );
	
	void openPropetiesDialog  ( );
	void openHelpDialog       ( );
	void openAboutDialog      ( );
	
	void correctionProcessing ( );
	
private:
	
	Loudness::LoudnessLibrary *ploudMeter;
	
	float           programDuration;
	bool            channelIsActive  [6];
	
	QPalette        paletteGreen;
	QPalette        paletteOrange;
	QPalette        paletteRed;
	QPalette        paletteGray;
	
	QtVectorViewer *temporalViewer;
	QtVectorHistogram *histogramViewer;
	
	
	QFrame          mainFrame;
	QGridLayout     mainGrid;
	
	QDockWidget     dockDigitResults;
	QDockWidget     dockTemporalViewer;
	QDockWidget     dockHistogramViewer;
	
	QTabWidget      mainTabulation;
	
	QFrame          frameTab1;
	QFrame          frameTab2;
	
	QFrame          frameResults;
	QFrame          frameParameters;
	QGridLayout     gridTab1;
	QGridLayout     gridTab2;
	QGridLayout     layoutResults;
	QGridLayout     layoutParameters;
	
	QMenu           menuFile;
	QMenu           menuSettings;
	QMenu           menuHelp;
	
	QAction         aOpen;
	QAction         aSave;
	QAction         aClose;
	QAction         aQuit;
	
	QAction         aOpenPropertiesDialog;
	QAction         aHelp;
	QAction         aAbout;
	
	Properties      properties;
	
	QProgressBar    progressBar;
	
	QLabel          labelProgramType;
	QLabel          labelProgramShortLong;
	
	QLabel          labelProgramLoudness;
	QLabel          labelProgramRange;
	QLabel          labelMaxShortTermLoudness;
	QLabel          labelMinShortTermLoudness;
	QLabel          labelMomentaryLoudness;
	QLabel          labelTruePeak;
	
	QLCDNumber      programLoudness;
	QLCDNumber      programRange;
	QLCDNumber      maxShortTermLoudness;
	QLCDNumber      minShortTermLoudness;
	QLCDNumber      momentaryLoudness;
	QLCDNumber      truePeak;
	
	QPushButton     processSeparatedFiles;
	QPushButton     processMultiChannelFile;
	
	QPushButton     correctSeparatedFiles;
	QPushButton     correctMultiChannelFile;
	
	AudioFile       multichannelFile;
	
	AudioFile      *audioFiles       [6];
	
	DropAudio       dropAudio;
};

#endif // PLoudGui_H
