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
  progressBar                ( ),
  /// labels for results
  labelProgramType           ( tr( "Program" ) ),
  labelProgramShortLong      ( tr( "<font color='gray'>Short / Long</font>" ) ),
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
  /// process buttons
  processSeparatedFiles      ( QIcon( ":/icons/view-refresh.png" ), tr( "Process" ) ),
  processMultiChannelFile    ( QIcon( ":/icons/view-refresh.png" ), tr( "Process" ) ),
  /// AudioFile
  multichannelFile           ( tr( "File" ), statusBar() ),
  /// DropAudio button
  dropAudio                  ( QIcon( ":/icons/list-add.png" ), tr( "Add files" ), statusBar() )
{
  setMinimumWidth  ( 600 );
  setMinimumHeight ( 600 );
  setWindowTitle   ( tr( "PLoud Validator - MikrosImage" ) );
  setWindowIcon    ( QIcon( ":/icons/logomikros.jpg" ) );

  processSeparatedFiles.setEnabled   ( false );
  processMultiChannelFile.setEnabled ( false );

  processSeparatedFiles  .setMinimumHeight ( 30 );
  processMultiChannelFile.setMinimumHeight ( 30 );

  temporalViewer  = new QtVectorViewer();
  histogramViewer = new QtVectorHistogram();

  mainTabulation.addTab( &frameTab1, tr( "separated files"   ) );
  mainTabulation.addTab( &frameTab2, tr( "multichannel file" ) );

  for( size_t i=0; i<6; i++)
  {
    audioFiles[i] = new AudioFile( tr(channelName[i]), statusBar(), this );
  }

  std::size_t line = 0;
  gridTab1.addWidget( &dropAudio               , line++, 1 );
  for( size_t i=0; i<6; i++)
  {
    gridTab1.addWidget( audioFiles[i]          , line++, 1 );
  }
  gridTab1.addWidget( &processSeparatedFiles   , line++, 1 );

  gridTab2.addWidget( &multichannelFile        , 0, 0 );
  gridTab2.addWidget( &processMultiChannelFile , 1, 0 );

  mainGrid.addWidget( &mainTabulation );
  mainGrid.addWidget( &progressBar );

  setCentralWidget( &mainFrame );

  labelProgramShortLong.setTextFormat ( Qt::RichText );

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
  layoutResults.addWidget( &labelProgramType         , line++, 0 );
  layoutResults.addWidget( &labelProgramLoudness     , line++, 0 );
  layoutResults.addWidget( &labelProgramRange        , line++, 0 );
  layoutResults.addWidget( &labelMaxShortTermLoudness, line++, 0 );
  layoutResults.addWidget( &labelMinShortTermLoudness, line++, 0 );
  layoutResults.addWidget( &labelMomentaryLoudness   , line++, 0 );
  layoutResults.addWidget( &labelTruePeak            , line++, 0 );

  line = 0;
  layoutResults.addWidget( &labelProgramShortLong    , line++, 1 );
  layoutResults.addWidget( &programLoudness          , line++, 1 );
  layoutResults.addWidget( &programRange             , line++, 1 );
  layoutResults.addWidget( &maxShortTermLoudness     , line++, 1 );
  layoutResults.addWidget( &minShortTermLoudness     , line++, 1 );
  layoutResults.addWidget( &momentaryLoudness        , line++, 1 );
  layoutResults.addWidget( &truePeak                 , line++, 1 );

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

  connect( &dropAudio               , SIGNAL( leftChannelWasDropped          ( QString ) ), audioFiles[0], SLOT( setFilename( QString ) ) );
  connect( &dropAudio               , SIGNAL( rightChannelWasDropped         ( QString ) ), audioFiles[1], SLOT( setFilename( QString ) ) );
  connect( &dropAudio               , SIGNAL( centerChannelWasDropped        ( QString ) ), audioFiles[2], SLOT( setFilename( QString ) ) );
  connect( &dropAudio               , SIGNAL( leftSurroundChannelWasDropped  ( QString ) ), audioFiles[3], SLOT( setFilename( QString ) ) );
  connect( &dropAudio               , SIGNAL( rightSurroundChannelWasDropped ( QString ) ), audioFiles[4], SLOT( setFilename( QString ) ) );
  connect( &dropAudio               , SIGNAL( lfeChannelWasDropped           ( QString ) ), audioFiles[5], SLOT( setFilename( QString ) ) );

  if( rlmSesame() )
  {
	processSeparatedFiles.setEnabled   ( true );
	processMultiChannelFile.setEnabled ( true );
  }
}

PLoudGui::~PLoudGui()
{
  for( size_t i=0; i<6; i++)
  {
    delete audioFiles[i];
  }

  delete temporalViewer;
  delete histogramViewer;
}

void PLoudGui::openSeparatedFiles( )
{
	processSeparatedFiles.setText( "Wait ..." );
	processSeparatedFiles.setIcon( QIcon( ":/icons/document-encrypt.png" ) );
	std::cout << "Processing..." << std::endl;

	switch( properties.normalisation )
	{
	    case 0: ploudMeter = new Loudness::LoudnessLibrary( Loudness::LoudnessLevels::Loudness_CST_R017( ) ); break;
	    case 1: ploudMeter = new Loudness::LoudnessLibrary( Loudness::LoudnessLevels::Loudness_EBU_R128( ) ); break;
	    case 2: ploudMeter = new Loudness::LoudnessLibrary( Loudness::LoudnessLevels::Loudness_ATSC_A85( ) ); break;
	    case 3:
	    {
		Loudness::LoudnessLevels levels (
			    properties.programLoudnessLongProgramMaxValue,
			    properties.programLoudnessLongProgramMinValue,
			    properties.shortTermLoudnessLongProgramMaxValue,
			    properties.shortTermLoudnessLongProgramMinValue,
			    properties.programLoudnessShortProgramMaxValue,
			    properties.programLoudnessShortProgramMinValue,
			    properties.shortTermLoudnessShortProgramMaxValue,
			    properties.truePeakMaxValue,
			    properties.maximalLoudnessRange,
			    properties.minimalLoudnessRange,
			    properties.relativeThreshold,
			    properties.absoluteThreshold
			    );
		ploudMeter = new Loudness::LoudnessLibrary( levels ); break;
	    }
	    default: ploudMeter = new Loudness::LoudnessLibrary( Loudness::LoudnessLevels::Loudness_CST_R017( ) ); break;
	}

	ploudMeter->setUpsamplingFrequencyForTruePeak( properties.getFrequencyTruePeak() );

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
		delete ploudMeter;
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
		delete ploudMeter;
		return;
	}

	size_t channelsInBuffer = std::min((size_t) 5, numberOfChannels );

	int bufferSize = rate / 5;
	float *data [ channelsInBuffer ];
	for( size_t i=0; i<channelsInBuffer; i++ )
	  data [i] = new float [bufferSize];

	progressBar.setMaximum ( audioInputFile[0].size() ),
	progressBar.setMinimum ( 0 );

	ploudMeter->initAndStart( channelsInBuffer, rate );
	int cumulOfSamples = 0;
	while (true)
	{
		int samples = 0;
		for( size_t i=0; i<channelsInBuffer; i++ )
		{
			samples = audioInputFile[i].read ( data[i], bufferSize );
		}

		if (samples == 0) break;
		cumulOfSamples += samples;
		progressBar.setValue( cumulOfSamples );
		ploudMeter->processSamples( data, samples );
	}

	programDuration = audioInputFile[0].size() / audioInputFile[0].rate() ;

	for( size_t i=0; i<numberOfChannels; i++ )
	{
		audioInputFile[i].close();
		delete[] data[i];
	}

	this->updateInterface();
}

void PLoudGui::openMultichannelFile( )
{
	processMultiChannelFile.setText("Wait ...");
	processMultiChannelFile.setIcon( QIcon( ":/icons/document-encrypt.png" ) );
	std::cout << "Processing..." << std::endl;

	if( multichannelFile.getFilename().toLatin1().size() == 0 )
	{
		QString msg  = "No input file was loaded.\n";

		QMessageBox::critical( this, "Error in files", msg );
		processMultiChannelFile.setText( "Process" );
		return;
	}

	switch( properties.normalisation )
	{
	    case 0: ploudMeter = new Loudness::LoudnessLibrary( Loudness::LoudnessLevels::Loudness_CST_R017( ) ); break;
	    case 1: ploudMeter = new Loudness::LoudnessLibrary( Loudness::LoudnessLevels::Loudness_EBU_R128( ) ); break;
	    case 2: ploudMeter = new Loudness::LoudnessLibrary( Loudness::LoudnessLevels::Loudness_ATSC_A85( ) ); break;
	    case 3:
	    {
		Loudness::LoudnessLevels levels (
			    properties.programLoudnessLongProgramMaxValue,
			    properties.programLoudnessLongProgramMinValue,
			    properties.shortTermLoudnessLongProgramMaxValue,
			    properties.shortTermLoudnessLongProgramMinValue,
			    properties.programLoudnessShortProgramMaxValue,
			    properties.programLoudnessShortProgramMinValue,
			    properties.shortTermLoudnessShortProgramMaxValue,
			    properties.truePeakMaxValue,
			    properties.maximalLoudnessRange,
			    properties.minimalLoudnessRange,
			    properties.relativeThreshold,
			    properties.absoluteThreshold
			    );
		ploudMeter = new Loudness::LoudnessLibrary( levels ); break;
	    }
	    default: ploudMeter = new Loudness::LoudnessLibrary( Loudness::LoudnessLevels::Loudness_CST_R017( ) ); break;
	}

	ploudMeter->setUpsamplingFrequencyForTruePeak( properties.getFrequencyTruePeak() );

	SoundFile audioInputFile;

	if ( audioInputFile.open_read ( multichannelFile.getFilename().toLatin1() ) != 0 )
	{
		QString msg  = "Error to open file :\n";
		msg += multichannelFile.getFilename();
		msg += "\n";

		QMessageBox::critical( this, "Error in files", msg );
		processMultiChannelFile.setText( "Process" );
		return;
	}

	int bufferSize = audioInputFile.rate () / 5;
	size_t channelsInBuffer = std::min( 5, audioInputFile.chan() );

	float *data [ channelsInBuffer ];
	float* inpb = new float [audioInputFile.chan() * bufferSize];

	for( size_t i=0; i<channelsInBuffer; i++ )
		data [i] = new float [bufferSize];

	ploudMeter->initAndStart( channelsInBuffer, audioInputFile.rate() );

	progressBar.setMaximum ( audioInputFile.size() ),
	progressBar.setMinimum ( 0 );

	int cumulOfSamples = 0;
	while (true)
	{
		int  samples = audioInputFile.read (inpb, bufferSize);
		if (samples == 0) break;

		cumulOfSamples += samples;
		progressBar.setValue( cumulOfSamples );
		float* p = inpb;
		for (int i = 0; i < samples; i++)
		{
			for(size_t c = 0; c < channelsInBuffer; c++ )
				data [c][i] = *p++;
		}
		ploudMeter->processSamples( data, samples );
	}
	programDuration = audioInputFile.size() / audioInputFile.rate() ;

	audioInputFile.close();
	for( int i=0; i<audioInputFile.chan(); i++ )
		delete[] data[i];

	this->updateInterface();
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

void PLoudGui::updateInterface()
{
	bool   isShortProgram;
	double integratedLoudnessValue;
	double integratedRange;
	double maxShortTermLoudnessValue;
	double minShortTermLoudnessValue;
	double momentaryLoudnessValue;
	double truePeakValue;
	double truePeakValueInDb;

	std::vector<float> shortTermValues;
	ploudMeter -> getIsShortProgram       ( isShortProgram            );
	ploudMeter -> getIntegratedLoudness   ( integratedLoudnessValue   );
	ploudMeter -> getIntegratedRange      ( integratedRange           );
	ploudMeter -> getMinShortTermLoudness ( minShortTermLoudnessValue );
	ploudMeter -> getMaxShortTermLoudness ( maxShortTermLoudnessValue );
	ploudMeter -> getMomentaryLoudness    ( momentaryLoudnessValue    );
	ploudMeter -> getTruePeakValue        ( truePeakValue             );
	ploudMeter -> getTruePeakInDbTP       ( truePeakValueInDb         );

	ploudMeter -> printPloudValues();

	isShortProgram ? labelProgramShortLong.setText( tr( "<font color='green'>Short</font><font color='gray'> / Long</font>" ) ) :
	labelProgramShortLong.setText( tr( "<font color='gray'>Short / </font><font color='green'>Long</font>" ) );

	programLoudness       .display( QString::number( integratedLoudnessValue   , 'f', 1 ) + " LUFS" );
	programRange          .display( QString::number( integratedRange           , 'f', 1 ) + " LU  " );
	maxShortTermLoudness  .display( QString::number( maxShortTermLoudnessValue , 'f', 1 ) + " LUFS" );
	minShortTermLoudness  .display( QString::number( minShortTermLoudnessValue , 'f', 1 ) + " LUFS" );
	momentaryLoudness     .display( QString::number( momentaryLoudnessValue    , 'f', 1 ) + " LUFS" );
	truePeak              .display( QString::number( truePeakValueInDb         , 'f', 1 ) + " dBFS" );

	( ploudMeter -> isIntegratedLoudnessValid() == Loudness::eValidResult ) ? programLoudness.setPalette( paletteGreen ) :
	  ( ploudMeter -> isIntegratedLoudnessValid() == Loudness::eNotValidResult ) ? programLoudness.setPalette( paletteRed ) :
	    ( ploudMeter -> isIntegratedLoudnessValid() == Loudness::eNotValidResultButNotIllegal ) ? programLoudness.setPalette( paletteOrange) : programLoudness.setPalette( paletteGray ) ;

	( ploudMeter -> isIntegratedLoudnessRangeValid() == Loudness::eValidResult ) ? programRange.setPalette( paletteGreen ) :
	  ( ploudMeter -> isIntegratedLoudnessRangeValid() == Loudness::eNotValidResult ) ? programRange.setPalette( paletteRed ) :
	    ( ploudMeter -> isIntegratedLoudnessRangeValid() == Loudness::eNotValidResultButNotIllegal ) ? programRange.setPalette( paletteOrange ) : programRange.setPalette( paletteGray ) ;

	( ploudMeter -> isMaxShortTermLoudnessValid() == Loudness::eValidResult ) ? maxShortTermLoudness.setPalette( paletteGreen ) :
	  ( ploudMeter -> isMaxShortTermLoudnessValid() == Loudness::eNotValidResult ) ? maxShortTermLoudness.setPalette( paletteRed ) :
	    ( ploudMeter -> isMaxShortTermLoudnessValid() == Loudness::eNotValidResultButNotIllegal ) ? maxShortTermLoudness.setPalette( paletteOrange ) : maxShortTermLoudness.setPalette( paletteGray ) ;

	( ploudMeter -> isMinShortTermLoudnessValid() == Loudness::eValidResult ) ? minShortTermLoudness.setPalette( paletteGreen ) :
	  ( ploudMeter -> isMinShortTermLoudnessValid() == Loudness::eNotValidResult ) ? minShortTermLoudness.setPalette( paletteRed ) :
	    ( ploudMeter -> isMinShortTermLoudnessValid() == Loudness::eNotValidResultButNotIllegal ) ? minShortTermLoudness.setPalette( paletteOrange ) : minShortTermLoudness.setPalette( paletteGray ) ;

	( ploudMeter -> isMomentaryLoudnessValid() == Loudness::eValidResult ) ? momentaryLoudness.setPalette( paletteGreen ) :
	  ( ploudMeter -> isMomentaryLoudnessValid() == Loudness::eNotValidResult ) ? momentaryLoudness.setPalette( paletteRed ) :
	    ( ploudMeter -> isMomentaryLoudnessValid() == Loudness::eNotValidResultButNotIllegal ) ? momentaryLoudness.setPalette( paletteOrange ) : momentaryLoudness.setPalette( paletteGray ) ;

	( ploudMeter -> isTruePeakValid() == Loudness::eValidResult ) ? truePeak.setPalette( paletteGreen ) :
	  ( ploudMeter -> isTruePeakValid() == Loudness::eNotValidResult ) ? truePeak.setPalette( paletteRed ) :
	    ( ploudMeter -> isTruePeakValid() == Loudness::eNotValidResultButNotIllegal ) ? truePeak.setPalette( paletteOrange ) : truePeak.setPalette( paletteGray ) ;

	shortTermValues = ploudMeter->getShortTermValues();

	QFile f("/homes/mrn/logPLoudValidator.txt");
	f.open( QIODevice::WriteOnly );

	for( size_t i=0; i< shortTermValues.size(); i++ )
	{
	  f.write( QString::number( shortTermValues.at( i ), 'f', 1 ).toLatin1().constData() );
	  f.write( "\n" );
	}
	f.close();

	std::vector<int> tmpHist = ploudMeter->getShortTermHistogram();
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
	temporalViewer->addData( ploudMeter->getTruePeakValues()     , 255,   0,   0, 128 );
	temporalViewer->addData( ploudMeter->getShortTermValues()    ,  84, 160, 207, 190 );
	temporalViewer->update();
	histogramViewer->update();

	delete ploudMeter;
	processMultiChannelFile.setText("Process");
	processSeparatedFiles.setText("Process");
	processMultiChannelFile.setIcon( QIcon(":/icons/view-refresh.png") );
	processSeparatedFiles.setIcon( QIcon(":/icons/view-refresh.png") );
}

void PLoudGui::openPropetiesDialog()
{
  properties.open();
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

