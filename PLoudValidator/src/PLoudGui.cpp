#include "PLoudGui.h"

#include <iostream>
#include <cstring>
#include <cmath>
#include <climits>

#include <QtGui/QtGui>
#include "LoudnessMeter.h"
#include "QtVectorViewer.h"
#include "QtVectorHistogram.h"
#include <SoundFile.h>

#include "loudnessLibrary.h"

#include <HelpDialog.h>

#define BUFFER_SIZE 4096
#include "AudioFile.h"

#include "license.h"

const char* channelName [6]=
{
	"Left",
	"Right",
	"Center",
	"Surround Left",
	"Surround Right",
	"LFE"
};

enum ChannelId
{
	idLeft = 0,
	idRight,
	idCenter,
	idSurroundLeft,
	idSurroundRight,
	idLfe
};

PLoudGui::PLoudGui( QWidget* parent ) :
	QMainWindow                ( parent ),
	ploudProc                  ( NULL ),
	/// main component (Frame ,Grid)
	mainFrame                  ( this ),
	mainGrid                   ( &mainFrame ),
	/// dock widget
	dockDigitResults           ( tr( "Numericals results"  ), this ),
	dockTemporalViewer         ( tr( "Max Short-Term Loudness + True Peak" ), this ),
	dockHistogramViewer        ( tr( "Short-Term histogram")),
	/// dock component (Frame ,Grid and Tabulations)
	mainTabulation             ( ),
	frameTab1                  ( ),
	frameTab2                  ( ),
	frameResults               ( ),
	frameParameters            ( ),
	gridTab1                   ( &frameTab1       ),
	gridTab2                   ( &frameTab2       ),
	layoutResults              ( &frameResults    ),
	layoutParameters           ( &frameParameters ),
	/// menu
	menuFile                   ( tr( "&File"     ) ),
	menuSettings               ( tr( "&Settings" ) ),
	menuHelp                   ( tr( "&Help"     ) ),
	/// actions in menu bar
	aOpen                      ( QIcon( ":/icons/document-open.png"    ), tr( "&Open"                 ), this ),
	aSave                      ( QIcon( ":/icons/document-save.png"    ), tr( "&Save"                 ), this ),
	aClose                     ( QIcon( ":/icons/dialog-close.png"     ), tr( "&Close"                ), this ),
	aQuit                      ( QIcon( ":/icons/application-exit.png" ), tr( "&Quit"                 ), this ),
	aOpenPropertiesDialog      ( QIcon( ":/icons/configure.png"        ), tr( "&Properties"           ), this ),
	aHelp                      ( QIcon( ":/icons/help-contents.png"    ), tr( "PloudValidator &Help"  ), this ),
	aAbout                     ( QIcon( ":/icons/help-about.png"       ), tr( "&About PloudValidator" ), this ),
	/// Propeties
	properties                 ( ),
	/// Progress bar
	progressMsg( ),
	progressBar( ),
	/// labels for results
	labelStandard              ( tr( "Standard" ) ),
	labelStandardUsed          ( tr( "<h1><font color='green'>France CST RT 017</font><font color='gray'><br/>EBU R 128<br/>ATSC A/85<br/>custom </font></h1>" ) ),
	labelProgramType           ( tr( "Program" ) ),
	labelProgramShortLong      ( tr( "<h1><font color='gray'>Short / Long</font></h1>" ) ),
	labelProgramLoudness       ( tr( "Program Loudness"        ) ),
	labelProgramRange          ( tr( "Loudness Range - LRA"    ) ),
	labelMaxShortTermLoudness  ( tr( "Max Short-Term Loudness" ) ),
	labelMinShortTermLoudness  ( tr( "Min Short-Term Loudness" ) ),
	labelMomentaryLoudness     ( tr( "Max Momentary Loudness"  ) ),
	labelTruePeak              ( tr( "True Peak Value"         ) ),
	/// initialize QLCDNumber with 10 digits
	programLoudness            ( 10 ),
	programRange               ( 10 ),
	maxShortTermLoudness       ( 10 ),
	minShortTermLoudness       ( 10 ),
	momentaryLoudness          ( 10 ),
	truePeak                   ( 10 ),
	labelFinalResult           ( tr( "Result" ) ),
	labelFinalResultResponse   ( tr( "<h1><font color='gray'>Valid / Invalid</font></h1>" ) ),
	/// process buttons
	processSeparatedFiles      ( QIcon( ":/icons/view-refresh.png" ), tr( "Process" ) ),
	processMultiChannelFile    ( QIcon( ":/icons/view-refresh.png" ), tr( "Process" ) ),
	/// correction buttons
	correctSeparatedFiles      ( QIcon( ":/icons/view-refresh.png" ), tr( "Correct" ) ),
	correctMultiChannelFile    ( QIcon( ":/icons/view-refresh.png" ), tr( "Correct" ) ),
	/// AudioFile
	multichannelFile           ( tr( "" ), statusBar(), ":/icons/multichannel.png", 100 ),
	/// DropAudio button
	dropAudio                  ( QIcon( ":/iupdateInterfacecons/list-add.png" ), tr( "Add files (drop here)" ), statusBar() ),
	correctedFile              ( false )
{
	setMinimumWidth  ( 600 );
	setMinimumHeight ( 600 );
	setWindowTitle   ( tr( "PLoud Validator - MikrosImage" ) );
	setWindowIcon    ( QIcon( ":/icons/logomikros.jpg" ) );
	
	processSeparatedFiles.setEnabled   ( false );
	processMultiChannelFile.setEnabled ( false );
	
	correctSeparatedFiles.setEnabled   ( false );
	correctMultiChannelFile.setEnabled ( false );
	
	processSeparatedFiles  .setMinimumHeight ( 40 );
	processMultiChannelFile.setMinimumHeight ( 40 );
	
	correctSeparatedFiles  .setMinimumHeight ( 40 );
	correctMultiChannelFile.setMinimumHeight ( 40 );
	
	progressMsg.setAlignment( Qt::AlignHCenter );
	
	temporalViewer  = new QtVectorViewer();
	histogramViewer = new QtVectorHistogram();
	
	mainTabulation.addTab( &frameTab1, tr( "separated files"   ) );
	mainTabulation.addTab( &frameTab2, tr( "multichannel file" ) );
	
	audioFiles[0] = new AudioFile( tr(channelName[0]), statusBar(), ":/icons/left.png" );
	audioFiles[1] = new AudioFile( tr(channelName[1]), statusBar(), ":/icons/right.png" );
	audioFiles[2] = new AudioFile( tr(channelName[2]), statusBar(), ":/icons/center.png" );
	audioFiles[3] = new AudioFile( tr(channelName[3]), statusBar(), ":/icons/surLeft.png" );
	audioFiles[4] = new AudioFile( tr(channelName[4]), statusBar(), ":/icons/surRight.png" );
	audioFiles[5] = new AudioFile( tr(channelName[5]), statusBar(), ":/icons/lfe.png" );
	
	std::size_t line = 0;
	gridTab1.addWidget( &dropAudio             , line++, 0, 1, 3 );
	
	gridTab1.addWidget( audioFiles[0]          , line, 0 ); // left
	gridTab1.addWidget( audioFiles[1]          , line, 2 ); // right
	gridTab1.addWidget( audioFiles[2]          , line, 1); // center
	gridTab1.addWidget( audioFiles[3]          , line + 2, 0 ); // surround left
	gridTab1.addWidget( audioFiles[4]          , line + 2, 2 ); // surround right
	gridTab1.addWidget( audioFiles[5]          , line + 1, 1 ); // LFE
	line += 3;
	gridTab1.addWidget( &processSeparatedFiles   , line++, 0, 1, 3 );
	gridTab1.addWidget( &correctSeparatedFiles   , line++, 0, 1, 3 );
	
	line = 0;
	gridTab2.addWidget( &multichannelFile        , line++, 0 );
	gridTab2.addWidget( &processMultiChannelFile , line++, 0 );
	gridTab2.addWidget( &correctMultiChannelFile , line++, 0 );
	
	mainGrid.addWidget( &mainTabulation );
	mainGrid.addWidget( &progressMsg );
	mainGrid.addWidget( &progressBar );
	
	setCentralWidget( &mainFrame );
	
	labelStandardUsed.setTextFormat ( Qt::RichText );
	labelStandardUsed.setAlignment( Qt::AlignCenter );
	labelStandardUsed.setObjectName("result");
			
	labelProgramShortLong.setTextFormat ( Qt::RichText );
	labelProgramShortLong.setAlignment( Qt::AlignCenter );
	labelProgramShortLong.setObjectName("result");
	
	labelFinalResultResponse.setAlignment( Qt::AlignCenter );
	labelFinalResultResponse.setTextFormat ( Qt::RichText );
	labelFinalResultResponse.setObjectName("result");
	
	programLoudness      .setSegmentStyle( QLCDNumber::Flat );
	programRange         .setSegmentStyle( QLCDNumber::Flat );
	maxShortTermLoudness .setSegmentStyle( QLCDNumber::Flat );
	minShortTermLoudness .setSegmentStyle( QLCDNumber::Flat );
	momentaryLoudness    .setSegmentStyle( QLCDNumber::Flat );
	truePeak             .setSegmentStyle( QLCDNumber::Flat );
	
	paletteGreen = programLoudness.palette();
	paletteGreen.setColor( QPalette::Active,   QPalette::WindowText, QColor(40, 150, 15) );
	paletteGreen.setColor( QPalette::Active,   QPalette::Window,     QColor(40, 150, 15) );
	paletteGreen.setColor( QPalette::Inactive, QPalette::WindowText, QColor(40, 150, 15) );
	paletteGreen.setColor( QPalette::Inactive, QPalette::Window,     QColor(40, 150, 15) );
	
	paletteOrange = programLoudness.palette();
	paletteOrange.setColor( QPalette::Active,   QPalette::WindowText, QColor(200, 140, 40) );
	paletteOrange.setColor( QPalette::Active,   QPalette::Window,     QColor(200, 140, 40) );
	paletteOrange.setColor( QPalette::Inactive, QPalette::WindowText, QColor(200, 140, 40) );
	paletteOrange.setColor( QPalette::Inactive, QPalette::Window,     QColor(200, 140, 40) );
	
	paletteRed = programLoudness.palette();
	paletteRed.setColor( QPalette::Active,   QPalette::WindowText, QColor(200, 50, 40) );
	paletteRed.setColor( QPalette::Active,   QPalette::Window,     QColor(200, 50, 40) );
	paletteRed.setColor( QPalette::Inactive, QPalette::WindowText, QColor(200, 50, 40) );
	paletteRed.setColor( QPalette::Inactive, QPalette::Window,     QColor(200, 50, 40) );
	
	paletteGray = programLoudness.palette();
	paletteGray.setColor( QPalette::Active,   QPalette::WindowText, QColor(128, 128, 128) );
	paletteGray.setColor( QPalette::Active,   QPalette::Window,     QColor(128, 128, 128) );
	paletteGray.setColor( QPalette::Inactive, QPalette::WindowText, QColor(128, 128, 128) );
	paletteGray.setColor( QPalette::Inactive, QPalette::Window,     QColor(128, 128, 128) );
	
	programLoudness      .setPalette( paletteGreen );
	programRange         .setPalette( paletteGreen );
	maxShortTermLoudness .setPalette( paletteGreen );
	minShortTermLoudness .setPalette( paletteGreen );
	momentaryLoudness    .setPalette( paletteGreen );
	truePeak             .setPalette( paletteGreen );
	
	line = 0;
	layoutResults.addWidget( &labelStandard            , line++, 0 );
	layoutResults.addWidget( &labelProgramType         , line++, 0 );
	layoutResults.addWidget( &labelProgramLoudness     , line++, 0 );
	layoutResults.addWidget( &labelProgramRange        , line++, 0 );
	layoutResults.addWidget( &labelMaxShortTermLoudness, line++, 0 );
	layoutResults.addWidget( &labelMinShortTermLoudness, line++, 0 );
	layoutResults.addWidget( &labelMomentaryLoudness   , line++, 0 );
	layoutResults.addWidget( &labelTruePeak            , line++, 0 );
	layoutResults.addWidget( &labelFinalResult         , line++, 0 );
	
	line = 0;
	layoutResults.addWidget( &labelStandardUsed        , line++, 1 );
	layoutResults.addWidget( &labelProgramShortLong    , line++, 1 );
	layoutResults.addWidget( &programLoudness          , line++, 1 );
	layoutResults.addWidget( &programRange             , line++, 1 );
	layoutResults.addWidget( &maxShortTermLoudness     , line++, 1 );
	layoutResults.addWidget( &minShortTermLoudness     , line++, 1 );
	layoutResults.addWidget( &momentaryLoudness        , line++, 1 );
	layoutResults.addWidget( &truePeak                 , line++, 1 );
	layoutResults.addWidget( &labelFinalResultResponse , line++, 1 );
	
	layoutResults.setColumnStretch( 0, 0 );
	layoutResults.setColumnStretch( 1, 100 );
	
	programLoudness      .display( "---.- LUFS" );
	programRange         .display( "---.- LU  " );
	maxShortTermLoudness .display( "---.- LUFS" );
	minShortTermLoudness .display( "---.- LUFS" );
	momentaryLoudness    .display( "---.- LUFS" );
	truePeak             .display( "---.- dbFS" );
	
	dockDigitResults   .setWidget( &frameResults );
	dockTemporalViewer .setWidget( temporalViewer );
	dockHistogramViewer.setWidget( histogramViewer );
	
	dockDigitResults.setMinimumWidth( 500 );
	
	addDockWidget( Qt::RightDockWidgetArea , &dockDigitResults  );
	addDockWidget( Qt::RightDockWidgetArea, &dockTemporalViewer );
	addDockWidget( Qt::RightDockWidgetArea, &dockHistogramViewer );
	
	tabifyDockWidget( &dockDigitResults, &dockTemporalViewer );
	tabifyDockWidget( &dockTemporalViewer, &dockHistogramViewer );
	dockDigitResults.show( );
	dockDigitResults.raise( );
	
	/*menuFile.addAction( &aOpen );
	menuFile.addAction( &aSave );
	menuFile.addAction( &aClose );
	menuFile.addSeparator();*/
	menuFile.addAction( &aQuit );
	
	menuSettings.addAction ( &aOpenPropertiesDialog );
	
	//menuHelp.addAction ( &aHelp );
	menuHelp.addAction ( &aAbout );
	
	menuBar()->addMenu( &menuFile );
	menuBar()->addMenu( &menuSettings );
	menuBar()->addMenu( &menuHelp );
	
	connect( &aQuit                   , SIGNAL( triggered() ), SLOT( close()                ) );
	connect( &aOpenPropertiesDialog   , SIGNAL( triggered() ), SLOT( openPropetiesDialog()  ) );
	
	connect( &aHelp                   , SIGNAL( triggered() ), SLOT( openHelpDialog()       ) );
	connect( &aAbout                  , SIGNAL( triggered() ), SLOT( openAboutDialog()      ) );
	
	connect( &processSeparatedFiles   , SIGNAL( clicked( )  ), SLOT( openSeparatedFiles  () ) );
	connect( &processMultiChannelFile , SIGNAL( clicked( )  ), SLOT( openMultichannelFile() ) );

	//connect( &correctSeparatedFiles   , SIGNAL( clicked( )  ), SLOT( correctionProcessing() ) );
	connect( &correctMultiChannelFile , SIGNAL( clicked( )  ), SLOT( correctionProcessing() ) );
	
	connect( &properties, SIGNAL( close( int ) ), SLOT( closePropetiesDialog( int ) ) );
	
	connect( &dropAudio               , SIGNAL( leftChannelWasDropped          ( QString ) ), audioFiles[0], SLOT( setFilename( QString ) ) );
	connect( &dropAudio               , SIGNAL( rightChannelWasDropped         ( QString ) ), audioFiles[1], SLOT( setFilename( QString ) ) );
	connect( &dropAudio               , SIGNAL( centerChannelWasDropped        ( QString ) ), audioFiles[2], SLOT( setFilename( QString ) ) );
	connect( &dropAudio               , SIGNAL( leftSurroundChannelWasDropped  ( QString ) ), audioFiles[3], SLOT( setFilename( QString ) ) );
	connect( &dropAudio               , SIGNAL( rightSurroundChannelWasDropped ( QString ) ), audioFiles[4], SLOT( setFilename( QString ) ) );
	connect( &dropAudio               , SIGNAL( lfeChannelWasDropped           ( QString ) ), audioFiles[5], SLOT( setFilename( QString ) ) );
	
	for( size_t i=0; i<6; i++)
	{
		connect( audioFiles[i], SIGNAL( fileWasSelected() ), SLOT( openNewFile() ) );
	}
	connect ( &multichannelFile, SIGNAL( fileWasSelected() ), SLOT( openNewFile() ) );
	
	if( rlmSesame() )
	{
		processSeparatedFiles.setEnabled   ( true );
		processMultiChannelFile.setEnabled ( true );
	}
}

PLoudGui::~PLoudGui()
{
	if ( ploudProc != NULL )
	{
		delete ploudProc;
		ploudProc = NULL;
	}
	
	for( size_t i=0; i<6; i++)
	{
		delete audioFiles[i];
	}
	
	delete temporalViewer;
	delete histogramViewer;
}

void PLoudGui::callbackProgress( void* object, int value )
{
	QProgressBar* pb = static_cast<QProgressBar*>(object);
	pb->setValue( value );
}

void analyseFiles( SoundFile* audioFile, size_t channels, Loudness::LoudnessLibrary* ploudMeter, QProgressBar& progressBar, double gain = 1.0 )
{
	int cumulOfSamples = 0;
	int bufferSize = audioFile[0].rate() / 5;
	
	float *data [ channels ];

	for( size_t i = 0; i< channels; i++ )
		data [i] = new float [bufferSize];
	
	ploudMeter->initAndStart( channels, audioFile[0].rate() );
	
	while (true)
	{
		int  samples = 0;
		
		for( size_t i = 0; i < channels; i++ )
		{
			samples = audioFile[i].read( data[i], bufferSize );
		}
		if (samples == 0) break;

		for (int i = 0; i < samples; i++)
		{
			for(size_t c = 0; c < channels; c++ )
				data [c][i] = data [c][i] * gain;
		}
		
		cumulOfSamples += samples;
		progressBar.setValue( cumulOfSamples );
		ploudMeter->processSamples( data, samples );
	}
	
	for( size_t i=0; i< channels; i++ )
		delete[] data[i];
}

void PLoudGui::openSeparatedFiles( )
{
	processSeparatedFiles.setText( "Wait ..." );
	processSeparatedFiles.setIcon( QIcon( ":/icons/document-encrypt.png" ) );
	std::cout << "Processing..." << std::endl;
	
	Loudness::LoudnessLevels levels (
		properties.programLoudnessLongProgramMaxValue,
		properties.programLoudnessLongProgramMinValue,
		properties.programLoudnessLongProgramTargetLevel,
		properties.programLoudnessLongProgramTargetMaxLevel,
		properties.programLoudnessLongProgramTargetMinLevel,
		properties.shortTermLoudnessLongProgramMaxValue,
		properties.shortTermLoudnessLongProgramMinValue,
		properties.programLoudnessShortProgramMaxValue,
		properties.programLoudnessShortProgramMinValue,
		properties.programLoudnessShortProgramTargetLevel,
		properties.programLoudnessShortProgramTargetMaxLevel,
		properties.programLoudnessShortProgramTargetMinLevel,
		properties.shortTermLoudnessShortProgramMaxValue,
		properties.truePeakMaxValue,
		properties.truePeakTargetLevel,
		properties.truePeakTargetMaxLevel,
		properties.absoluteThreshold,
		properties.relativeThreshold,
		properties.maximalLoudnessRange,
		properties.minimalLoudnessRange
	);
	
	if ( ploudProc != NULL )
	{
		delete ploudProc;
		ploudProc = NULL;
	}
	
	ploudProc = new PLoudProcess( levels, properties.getFrequencyTruePeak() );
	
	std::vector<std::string> files;
	
	for(size_t i=0; i<6; i++)
	{
		if( audioFiles[i]->getFilename().toLatin1().size() != 0 )
		{
			files.push_back( audioFiles[i]->getFilename().toLatin1().constData() );
		}
	}
	
	if( ! ploudProc->openAudioFiles( files ) )
	{
		QString msg  = "Error to open files\n";

		QMessageBox::critical( this, "Error in files", msg );
		processMultiChannelFile.setText( "Process" );
		return;
	}
	
	
	SoundFile audioInputFile [5];
	size_t    numberOfChannels = 0;

	for(size_t i=0; i<5; i++)
	{
		if( audioFiles[i]->getFilename().toLatin1().size() != 0 )
		{
			if ( audioInputFile[i].open_read ( audioFiles[i]->getFilename().toLatin1() ) != 0 )
			{
				QString msg  = "Error to open file :\n";
				msg += multichannelFile.getFilename();
				msg += "\n";

				QMessageBox::critical( this, "Error in files", msg );
				processMultiChannelFile.setText( "Process" );
			}
			else
			{
				numberOfChannels++;
			}
		}
	}

	if( numberOfChannels == 0 )
	{
		QString msg  = "No input file was loaded.\n";

		QMessageBox::critical( this, "Error in files", msg );
		processSeparatedFiles.setText( "Process" );
		return;
	}

	bool rateIsEqual  = true;
	bool sizeIsEqual  = true;
	bool monoChannels = true;
	int rate    = audioInputFile[0].rate();
	size_t size = audioInputFile[0].size();

	for(size_t i=0; i < numberOfChannels; i++)
	{
		if( rate != audioInputFile[i].rate() )
		{
			rateIsEqual = false;
		}
		if( size != audioInputFile[i].size() )
		{
			sizeIsEqual = false;
		}
		if( audioInputFile[i].chan() != 1 )
		{
			monoChannels = false;
		}
	}

	if( rateIsEqual == false || sizeIsEqual == false || monoChannels == false )
	{
		for( size_t i=0; i<numberOfChannels; i++ )
		{
			audioInputFile[i].close();
		}

		QString msg  = "One or more file isn't supported with these mode.\n";
			msg += "Only mono files, with same sampling rate, and with same duration are supported.\n";
			msg += "Error:\n";
			if (  rateIsEqual == false ) msg += "- Sampling rate are not equals\n";
			if (  sizeIsEqual == false ) msg += "- Duration is not equal\n";
			if ( monoChannels == false ) msg += "- Files are not mono audio\n";


		QMessageBox::critical( this, "Error in files", msg );
		processSeparatedFiles.setText( "Process" );
		return;
	}

	//size_t channelsInBuffer = std::min((size_t) 5, numberOfChannels );
	
	//ploudMeter->initAndStart( channelsInBuffer, rate );
	
	progressBar.setMaximum ( audioInputFile[0].size() ),
	progressBar.setMinimum ( 0 );
	
	//analyseFiles( audioInputFile, channelsInBuffer, ploudMeter, progressBar );

	programDuration = audioInputFile[0].size() / audioInputFile[0].rate() ;

	for( size_t i=0; i<numberOfChannels; i++ )
	{
		audioInputFile[i].close();
	}

	this->updateInterface();
}

void PLoudGui::openMultichannelFile( )
{
	processMultiChannelFile.setText("Wait ...");
	processMultiChannelFile.setIcon( QIcon( ":/icons/document-encrypt.png" ) );
	
	if( multichannelFile.getFilename().toLatin1().size() == 0 )
	{
		QString msg  = "No input file was loaded.\n";

		QMessageBox::critical( this, "Error in files", msg );
		processMultiChannelFile.setText( "Process" );
		return;
	}
	
	Loudness::LoudnessLevels levels (
		properties.programLoudnessLongProgramMaxValue,
		properties.programLoudnessLongProgramMinValue,
		properties.programLoudnessLongProgramTargetLevel,
		properties.programLoudnessLongProgramTargetMaxLevel,
		properties.programLoudnessLongProgramTargetMinLevel,
		properties.shortTermLoudnessLongProgramMaxValue,
		properties.shortTermLoudnessLongProgramMinValue,
		properties.programLoudnessShortProgramMaxValue,
		properties.programLoudnessShortProgramMinValue,
		properties.programLoudnessShortProgramTargetLevel,
		properties.programLoudnessShortProgramTargetMaxLevel,
		properties.programLoudnessShortProgramTargetMinLevel,
		properties.shortTermLoudnessShortProgramMaxValue,
		properties.truePeakMaxValue,
		properties.truePeakTargetLevel,
		properties.truePeakTargetMaxLevel,
		properties.absoluteThreshold,
		properties.relativeThreshold,
		properties.maximalLoudnessRange,
		properties.minimalLoudnessRange
	);

	if ( ploudProc != NULL )
	{
		delete ploudProc;
		ploudProc = NULL;
	}
	
	ploudProc = new PLoudProcess( levels, properties.getFrequencyTruePeak() );
	
	std::vector<std::string> files;
	files.push_back( multichannelFile.getFilename().toLatin1().constData() );

	if( ! ploudProc->openAudioFiles( files ) )
	{
		QString msg  = "Error to open file :\n";
		msg += multichannelFile.getFilename();
		msg += "\n";

		QMessageBox::critical( this, "Error in files", msg );
		processMultiChannelFile.setText( "Process" );
		return;
	}
	
	progressBar.setMaximum ( ploudProc->getProgramLength() );
	progressBar.setMinimum ( 0 );
	progressMsg.setText( "Analyse" );

	ploudProc->processAnalyseFile( callbackProgress, (void*)&progressBar );
	
	programDuration = ploudProc->getProgramDuration() ;
	ploudProc->closeAudioFiles();
	
	this->updateInterface( );
}

bool PLoudGui::rlmSesame()
{
	const char *license = ".";
	int stat;
	RLM_LICENSE _RLMlic;
	RLM_HANDLE  _RLMHandle = rlm_init( license, 0, (char *) NULL );
	stat = rlm_stat( _RLMHandle );

	if (stat)
	{
		char errString [ RLM_ERRSTRING_MAX ];
		rlm_errstring( ( RLM_LICENSE ) NULL, _RLMHandle, errString );
		QString msg  = "Error initializing license system :\n";
		msg += errString;
		msg += "\n";

		QMessageBox::critical( this, "License Error", msg );
		std::cerr << "Error initializing license system : " << errString << std::endl;
		return false;
	}


	bool sesame = false;
	const char *product = "ploudvalidator";
	const char *version = "1.0";
	int count = 1;

	_RLMlic = rlm_checkout( _RLMHandle, product, version, count );

	stat = rlm_license_stat( _RLMlic );
	if ( stat == 0 ){
		sesame = true;
	}
	else
	{
		char errString[ RLM_ERRSTRING_MAX ];
		std::cerr << "Error checking out " << product << " license : ";
		rlm_errstring( _RLMlic, _RLMHandle, errString );
		std::cerr << errString << std::endl;
		QString msg  = "Error  checking out ";
		msg += product;
		msg += " license :\n";
		msg += errString;
		msg += "\n";

		QMessageBox::critical( this, "License Error", msg );
	}

	return sesame;
}

void PLoudGui::updateInterface( )
{
	std::vector<float> shortTermValues;

	ploudProc->printPloudValues();

	ploudProc->isShortProgram( ) ? labelProgramShortLong.setText( tr( "<h1><font color='green'>Short</font><font color='gray'> / Long</font></h1>" ) ) :
	labelProgramShortLong.setText( tr( "<h1><font color='gray'>Short / </font><font color='green'>Long</font></h1>" ) );

	programLoudness       .display( QString::number( ploudProc->getIntegratedLoudness( ), 'f', 1 ) + " LUFS" );
	programRange          .display( QString::number( ploudProc->getIntegratedRange( ), 'f', 1 ) + " LU  " );
	maxShortTermLoudness  .display( QString::number( ploudProc->getMaxShortTermLoudness( ), 'f', 1 ) + " LUFS" );
	minShortTermLoudness  .display( QString::number( ploudProc->getMinShortTermLoudness( ), 'f', 1 ) + " LUFS" );
	momentaryLoudness     .display( QString::number( ploudProc->getMomentaryLoudness( ), 'f', 1 ) + " LUFS" );
	truePeak              .display( QString::number( ploudProc->getTruePeakInDbTP( ), 'f', 1 ) + " dBFS" );

	( ploudProc->isIntegratedLoudnessValid() == Loudness::eValidResult ) ? programLoudness.setPalette( paletteGreen ) :
		( ploudProc->isIntegratedLoudnessValid() == Loudness::eNotValidResult ) ? programLoudness.setPalette( paletteRed ) :
			( ploudProc->isIntegratedLoudnessValid() == Loudness::eNotValidResultButNotIllegal ) ? programLoudness.setPalette( paletteOrange) : programLoudness.setPalette( paletteGray ) ;

	( ploudProc->isIntegratedLoudnessRangeValid() == Loudness::eValidResult ) ? programRange.setPalette( paletteGreen ) :
		( ploudProc->isIntegratedLoudnessRangeValid() == Loudness::eNotValidResult ) ? programRange.setPalette( paletteRed ) :
			( ploudProc->isIntegratedLoudnessRangeValid() == Loudness::eNotValidResultButNotIllegal ) ? programRange.setPalette( paletteOrange ) : programRange.setPalette( paletteGray ) ;

	( ploudProc->isMaxShortTermLoudnessValid() == Loudness::eValidResult ) ? maxShortTermLoudness.setPalette( paletteGreen ) :
		( ploudProc->isMaxShortTermLoudnessValid() == Loudness::eNotValidResult ) ? maxShortTermLoudness.setPalette( paletteRed ) :
			( ploudProc->isMaxShortTermLoudnessValid() == Loudness::eNotValidResultButNotIllegal ) ? maxShortTermLoudness.setPalette( paletteOrange ) : maxShortTermLoudness.setPalette( paletteGray ) ;

	( ploudProc->isMinShortTermLoudnessValid() == Loudness::eValidResult ) ? minShortTermLoudness.setPalette( paletteGreen ) :
		( ploudProc->isMinShortTermLoudnessValid() == Loudness::eNotValidResult ) ? minShortTermLoudness.setPalette( paletteRed ) :
			( ploudProc->isMinShortTermLoudnessValid() == Loudness::eNotValidResultButNotIllegal ) ? minShortTermLoudness.setPalette( paletteOrange ) : minShortTermLoudness.setPalette( paletteGray ) ;

	( ploudProc->isMomentaryLoudnessValid() == Loudness::eValidResult ) ? momentaryLoudness.setPalette( paletteGreen ) :
		( ploudProc->isMomentaryLoudnessValid() == Loudness::eNotValidResult ) ? momentaryLoudness.setPalette( paletteRed ) :
			( ploudProc->isMomentaryLoudnessValid() == Loudness::eNotValidResultButNotIllegal ) ? momentaryLoudness.setPalette( paletteOrange ) : momentaryLoudness.setPalette( paletteGray ) ;

	( ploudProc->isTruePeakValid() == Loudness::eValidResult ) ? truePeak.setPalette( paletteGreen ) :
		( ploudProc->isTruePeakValid() == Loudness::eNotValidResult ) ? truePeak.setPalette( paletteRed ) :
			( ploudProc->isTruePeakValid() == Loudness::eNotValidResultButNotIllegal ) ? truePeak.setPalette( paletteOrange ) : truePeak.setPalette( paletteGray ) ;

	switch( ploudProc->isValidProgram() )
	{
		case Loudness::eValidResult: labelFinalResultResponse.setText( tr( "<h1><font color='green'>Valid</font><font color='gray'> / Invalid</font></h1>" ) ); break;
		case Loudness::eNotValidResult: labelFinalResultResponse.setText( tr( "<h1><font color='gray'>Valid / </font><font color='red'>Invalid</font></h1>" ) ); break;
		case Loudness::eNotValidResultButNotIllegal: labelFinalResultResponse.setText( tr( "<h1><font color='orange'>Valid</font><font color='gray'> / Invalid</font></h1>" ) ); break;
		case Loudness::eNoImportance	: labelFinalResultResponse.setText( tr( "<h1><font color='gray'>Valid / Invalid</font></h1>" ) );break;
	}
	
	shortTermValues = ploudProc->getShortTermValues();

	QFile f("/homes/mrn/logPLoudValidator.txt");
	f.open( QIODevice::WriteOnly );

	for( size_t i=0; i< shortTermValues.size(); i++ )
	{
		f.write( QString::number( shortTermValues.at( i ), 'f', 1 ).toLatin1().constData() );
		f.write( "\n" );
	}
	f.close();

	std::vector<int> tmpHist = ploudProc->getShortTermHistogram();
	std::vector<int> tmpHist750;

	for( int i=0; i< 750; i++ )
	{
		int tmpSum = 0.0;
		for( int y = 0; y < 10; y++)
		{
			tmpSum += tmpHist.at( i * 10 + y );
		}
		tmpHist750.push_back(tmpSum);
	}

	temporalViewer->clear();
	histogramViewer->clear();


	histogramViewer->addData( tmpHist750,  84, 160, 207, 190 );
	temporalViewer->addData( ploudProc->getTruePeakValues()     , 255,   0,   0, 128 );
	temporalViewer->addData( ploudProc->getShortTermValues()    ,  84, 160, 207, 190 );
	temporalViewer->update();
	histogramViewer->update();

	processMultiChannelFile.setText("Process");
	processSeparatedFiles.setText("Process");
	processMultiChannelFile.setIcon( QIcon(":/icons/view-refresh.png") );
	processSeparatedFiles.setIcon( QIcon(":/icons/view-refresh.png") );
	
	correctSeparatedFiles.setEnabled   ( true );
	correctMultiChannelFile.setEnabled ( true );
}

void PLoudGui::openNewFile          ( )
{
	correctSeparatedFiles.setEnabled   ( false );
	correctMultiChannelFile.setEnabled ( false );
	correctedFile = false;
}

void PLoudGui::openPropetiesDialog()
{
	properties.open();
}

void PLoudGui::closePropetiesDialog( int standard )
{
	switch ( standard )
	{
		case 0:labelStandardUsed.setText( tr( "<h1><font color='green'>France CST RT 017</font><font color='gray'><br/>EBU R 128<br/>ATSC A/85<br/>custom</font></h1>" ) ); break;
		case 1:labelStandardUsed.setText( tr( "<h1><font color='gray'>France CST RT 017<br/></font><font color='green'>EBU R 128</font><font color='gray'><br/>ATSC A/85<br/>custom</font></h1>" ) ); break;
		case 2:labelStandardUsed.setText( tr( "<h1><font color='gray'>France CST RT 017<br/>EBU R 128<br/></font><font color='green'>ATSC A/85</font><font color='gray'><br/>custom</font></h1>" ) ); break;
		case 3:labelStandardUsed.setText( tr( "<h1><font color='gray'>France CST RT 017<br/>EBU R 128<br/>ATSC A/85<br/></font><font color='green'>custom</font></h1>" ) ); break;
		default : labelStandardUsed.setText( tr( "<h1><font color='gray'>France CST RT 017<br/>EBU R 128<br/>ATSC A/85<br/>custom </font></h1>" ) ); break;
	}
}

void PLoudGui::openHelpDialog ( )
{
	QMessageBox msgBox;
	msgBox.setText("PLoud Validator Help");
	msgBox.setIcon( QMessageBox::Information );
	msgBox.setTextFormat( Qt::RichText );
	msgBox.setInformativeText(
				"<b>What's for:</b>\n"
				"PLoud Validator process your audio file, and mesure Loudness levels.\n<br/>"
				"Values was calculated in reference whit the R128 recommendation provided by the EBU."
				
				);
	
	msgBox.setDetailedText(
				"ITU-R BS. 1770/2 compliant\n"
				"EBU R128 compliant\n"
				);
	
	msgBox.setStandardButtons( QMessageBox::Close );
	msgBox.exec();
}

void PLoudGui::openAboutDialog ( )
{
	HelpDialog h(this);
	h.exec();
}

void PLoudGui::correctionProcessing( )
{
	if( correctedFile )
		return;
	
	std::string filename = multichannelFile.getFilename().toStdString();

	std::vector<std::string> files;
	files.push_back( filename );
	
        if( ! ploudProc->openAudioFiles( files ) )
	{
		QString msg  = "Error to open file :\n";
		msg += multichannelFile.getFilename();
		msg += "\n";

		QMessageBox::critical( this, "Error in files", msg );
		processMultiChannelFile.setText( "Process" );
		return;
	}
	
	progressMsg.setText( "Multi pass analysis for correction" );
	double newGain = 0;
	//ploudProc->analyseToFindCorrectionGain( callbackProgress, (void*)&progressBar, newGain );
	newGain = ploudProc->getCorrectionGain();
	std::cout.precision(6);
	std::cout << "writing with gain = " << newGain << " (" << 20.0 * std::log10( newGain ) << ")" << std::endl;
	std::cout.precision(1);
	
	// copy input and write with a new gain
	progressMsg.setText( "Wrinting file" );
	
	ploudProc->writeFile( callbackProgress, (void*)&progressBar, newGain );
	ploudProc->closeAudioFiles();
	
	progressMsg.setText( "Finish" );
	correctedFile = true;
}



