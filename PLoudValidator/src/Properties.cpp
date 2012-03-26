#include "Properties.h"
#include "loudnessLibrary.h"

Properties::Properties(QWidget* parent, Qt::WindowFlags f):
  QDialog                                    ( parent, f ),
  normalisation                              ( 0 ),
  programLoudnessLongProgramMaxValue         ( 0 ),
  programLoudnessLongProgramMinValue         ( 0 ),
  shortTermLoudnessLongProgramMaxValue       ( 0 ),
  shortTermLoudnessLongProgramMinValue       ( 0 ),
  programLoudnessShortProgramMaxValue        ( 0 ),
  programLoudnessShortProgramMinValue        ( 0 ),
  shortTermLoudnessShortProgramMaxValue      ( 0 ),
  truePeakMaxValue                           ( 0 ),
  maximalLoudnessRange                       ( 0 ),
  minimalLoudnessRange                       ( 0 ),
  frequencyTruePeak                          ( 0 ),
  layoutParameters                           ( ),
  tabProperties                              ( ),
  frameLongProgram                           ( ),
  frameShortProgram                          ( ),
  frameAllProgram                            ( ),
  frameProcessProperties                     ( ),
  layoutLongProgram                          ( &frameLongProgram ),
  layoutShortProgram                         ( &frameShortProgram ),
  layoutAllProgram                           ( &frameAllProgram ),
  layoutProcessProperties                    ( &frameProcessProperties ),
  comboSelectNormalisation                   ( ),
  labelProgramLoudnessLongProgramMaxValue    ( tr( "<b><font color='red'>max</font></b> program loudness"       ) ),
  labelProgramLoudnessLongProgramMinValue    ( tr( "<b><font color='green'>min</font></b> program loudness"     ) ),
  labelShortTermLoudnessLongProgramMaxValue  ( tr( "<b><font color='red'>max</font></b> short-term loudness"    ) ),
  labelShortTermLoudnessLongProgramMinValue  ( tr( "<b><font color='green'>min</font></b> short-term loudness"  ) ),
  labelProgramLoudnessShortProgramMaxValue   ( tr( "<b><font color='red'>max</font></b> program loudness"       ) ),
  labelProgramLoudnessShortProgramMinValue   ( tr( "<b><font color='green'>min</font></b> program loudness"       ) ),
  labelShortTermLoudnessShortProgramMaxValue ( tr( "<b><font color='red'>max</font></b> short-term loudness"    ) ),
  labelTruePeakMaxValue                      ( tr( "<b><font color='red'>max</font></b> true peak value"        ) ),
  labelAbsoluteThresholdValue                ( tr( "<b><font color='red'>absolute</font></b> threshold value"   ) ),
  labelRelativeThresholdValue                ( tr( "<b><font color='red'>relative</font></b> threshold value"   ) ),
  labelMaximalLoudnessRange                  ( tr( "<b><font color='red'>max</font></b> loudness Range (LRA)"   ) ),
  labelMinimalLoudnessRange                  ( tr( "<b><font color='green'>min</font></b> loudness Range (LRA)" ) ),
  spinProgramLoudnessLongProgramMaxValue     ( ),
  spinProgramLoudnessLongProgramMinValue     ( ),
  spinShortTermLoudnessLongProgramMaxValue   ( ),
  spinShortTermLoudnessLongProgramMinValue   ( ),
  spinProgramLoudnessShortProgramMaxValue    ( ),
  spinProgramLoudnessShortProgramMinValue    ( ),
  spinShortTermLoudnessShortProgramMaxValue  ( ),
  spinTruePeakMaxValue                       ( ),
  spinAbsoluteThresholdValue                 ( ),
  spinRelativeThresholdValue                 ( ),
  spinMaximalLoudnessRange                   ( ),
  spinMinimalLoudnessRange                   ( ),
  labelProgramLoudnessLongProgramMaxUnit     ( tr( "LUFS"                                                       ) ),
  labelProgramLoudnessLongProgramMinUnit     ( tr( "LUFS"                                                       ) ),
  labelShortTermLoudnessLongProgramMaxUnit   ( tr( "LU"                                                         ) ),
  labelShortTermLoudnessLongProgramMinUnit   ( tr( "LU"                                                         ) ),
  labelProgramLoudnessShortProgramMaxUnit    ( tr( "LUFS"                                                       ) ),
  labelProgramLoudnessShortProgramMinUnit    ( tr( "LUFS"                                                       ) ),
  labelShortTermLoudnessShortProgramMaxUnit  ( tr( "LU"                                                         ) ),
  labelTruePeakMaxUnit                       ( tr( "dBTP"                                                       ) ),
  labelAbsoluteThresholdUnit                 ( tr( "LUFS"                                                       ) ),
  labelRelativeThresholdUnit                 ( tr( "LU"                                                         ) ),
  labelMaximalLoudnessRangeUnit              ( tr( "LU"                                                         ) ),
  labelMinimalLoudnessRangeUnit              ( tr( "LU"                                                         ) ),
  labelFrequencyTruePeak                     ( tr( "upsampling frequency for True Peak Meter"                   ) ),
  comboFrequencyTruePeak                     ( ),

  endOfEdit                                  ( QIcon(":/icons/dialog-close.png"), tr( "Close"                   ) )
{
  setWindowTitle   ( "PLoud Validator - Properties" );
  setMinimumWidth  ( 600 );
  setMinimumHeight ( 400 );

  setLayout( &layoutParameters );

  comboSelectNormalisation.addItem("France CST - RT 017", 0);
  comboSelectNormalisation.addItem("EBU R128", 1);
  comboSelectNormalisation.addItem("ATSC A85", 2);
  comboSelectNormalisation.addItem("custom", 3);

  tabProperties .addTab( &frameLongProgram, tr( "long program"              ) );
  tabProperties .addTab( &frameShortProgram, tr( "short program"             ) );
  tabProperties .addTab( &frameAllProgram, tr( "all programs"              ) );
  tabProperties .addTab( &frameProcessProperties, tr( "process properties"        ) );

  labelProgramLoudnessLongProgramMaxValue    .setTextFormat ( Qt::RichText );
  labelProgramLoudnessLongProgramMinValue    .setTextFormat ( Qt::RichText );
  labelShortTermLoudnessLongProgramMaxValue  .setTextFormat ( Qt::RichText );
  labelShortTermLoudnessLongProgramMinValue  .setTextFormat ( Qt::RichText );

  labelProgramLoudnessShortProgramMaxValue   .setTextFormat ( Qt::RichText );
  labelShortTermLoudnessShortProgramMaxValue .setTextFormat ( Qt::RichText );

  labelTruePeakMaxValue                      .setTextFormat ( Qt::RichText );
  labelAbsoluteThresholdValue                .setTextFormat ( Qt::RichText );
  labelRelativeThresholdValue                .setTextFormat ( Qt::RichText );
  labelMaximalLoudnessRange                  .setTextFormat ( Qt::RichText );
  labelMinimalLoudnessRange                  .setTextFormat ( Qt::RichText );

  spinProgramLoudnessLongProgramMaxValue     .setMinimum ( -200.0 );
  spinProgramLoudnessLongProgramMinValue     .setMinimum ( -200.0 );
  spinShortTermLoudnessLongProgramMaxValue   .setMinimum ( -200.0 );
  spinShortTermLoudnessLongProgramMinValue   .setMinimum ( -200.0 );
  spinProgramLoudnessShortProgramMaxValue    .setMinimum ( -200.0 );
  spinProgramLoudnessShortProgramMinValue    .setMinimum ( -200.0 );
  spinShortTermLoudnessShortProgramMaxValue  .setMinimum ( -200.0 );
  spinTruePeakMaxValue                       .setMinimum ( -200.0 );
  spinAbsoluteThresholdValue                 .setMinimum ( -200.0 );
  spinRelativeThresholdValue                 .setMinimum ( -200.0 );
  spinMaximalLoudnessRange                   .setMinimum ( -200.0 );
  spinMinimalLoudnessRange                   .setMinimum ( -200.0 );

  spinProgramLoudnessLongProgramMaxValue     .setDecimals( 1 );
  spinProgramLoudnessLongProgramMinValue     .setDecimals( 1 );
  spinShortTermLoudnessLongProgramMaxValue   .setDecimals( 1 );
  spinShortTermLoudnessLongProgramMinValue   .setDecimals( 1 );
  spinProgramLoudnessShortProgramMaxValue    .setDecimals( 1 );
  spinProgramLoudnessShortProgramMinValue    .setDecimals( 1 );
  spinShortTermLoudnessShortProgramMaxValue  .setDecimals( 1 );
  spinTruePeakMaxValue                       .setDecimals( 1 );
  spinAbsoluteThresholdValue                 .setDecimals( 1 );
  spinRelativeThresholdValue                 .setDecimals( 1 );
  spinMaximalLoudnessRange                   .setDecimals( 1 );
  spinMinimalLoudnessRange                   .setDecimals( 1 );

  comboFrequencyTruePeak.addItem( "192kHz", QVariant( 192000 ) );
  comboFrequencyTruePeak.addItem( "384kHz", QVariant( 384000 ) );

  setEnableProperties( false );

  int line = 0;
  int col  = 0;
  layoutLongProgram.addWidget( &labelProgramLoudnessLongProgramMaxValue    , line++, col );
  layoutLongProgram.addWidget( &labelProgramLoudnessLongProgramMinValue    , line++, col );
  layoutLongProgram.addWidget( &labelShortTermLoudnessLongProgramMaxValue  , line++, col );
  layoutLongProgram.addWidget( &labelShortTermLoudnessLongProgramMinValue  , line++, col );

  line = 0; col++;
  layoutLongProgram.addWidget( &spinProgramLoudnessLongProgramMaxValue     , line++, col );
  layoutLongProgram.addWidget( &spinProgramLoudnessLongProgramMinValue     , line++, col );
  layoutLongProgram.addWidget( &spinShortTermLoudnessLongProgramMaxValue   , line++, col );
  layoutLongProgram.addWidget( &spinShortTermLoudnessLongProgramMinValue   , line++, col );

  line = 0; col++;
  layoutLongProgram.addWidget( &labelProgramLoudnessLongProgramMaxUnit     , line++, col );
  layoutLongProgram.addWidget( &labelProgramLoudnessLongProgramMinUnit     , line++, col );
  layoutLongProgram.addWidget( &labelShortTermLoudnessLongProgramMaxUnit   , line++, col );
  layoutLongProgram.addWidget( &labelShortTermLoudnessLongProgramMinUnit   , line++, col );


  line = 0; col = 0;
  layoutShortProgram.addWidget( &labelProgramLoudnessShortProgramMaxValue   , line++, col );
  layoutShortProgram.addWidget( &labelProgramLoudnessShortProgramMinValue   , line++, col );
  layoutShortProgram.addWidget( &labelShortTermLoudnessShortProgramMaxValue , line++, col );

  line = 0; col++;
  layoutShortProgram.addWidget( &spinProgramLoudnessShortProgramMaxValue    , line++, col );
  layoutShortProgram.addWidget( &spinProgramLoudnessShortProgramMinValue    , line++, col );
  layoutShortProgram.addWidget( &spinShortTermLoudnessShortProgramMaxValue  , line++, col );

  line = 0; col++;
  layoutShortProgram.addWidget( &labelProgramLoudnessShortProgramMaxUnit    , line++, col );
  layoutShortProgram.addWidget( &labelProgramLoudnessShortProgramMinUnit    , line++, col );
  layoutShortProgram.addWidget( &labelShortTermLoudnessShortProgramMaxUnit  , line++, col );

  line = 0; col = 0;
  layoutAllProgram.addWidget( &labelTruePeakMaxValue                      , line++, col );
  layoutAllProgram.addWidget( &labelMaximalLoudnessRange                  , line++, col );
  layoutAllProgram.addWidget( &labelMinimalLoudnessRange                  , line++, col );

  line = 0; col++;
  layoutAllProgram.addWidget( &spinTruePeakMaxValue                       , line++, col );
  layoutAllProgram.addWidget( &spinMaximalLoudnessRange                   , line++, col );
  layoutAllProgram.addWidget( &spinMinimalLoudnessRange                   , line++, col );

  line = 0; col++;
  layoutAllProgram.addWidget( &labelTruePeakMaxUnit                       , line++, col );
  layoutAllProgram.addWidget( &labelMaximalLoudnessRangeUnit              , line++, col );
  layoutAllProgram.addWidget( &labelMinimalLoudnessRangeUnit              , line++, col );

  line = 0; col = 0;
  layoutProcessProperties.addWidget( &labelAbsoluteThresholdValue         , line++, col );
  layoutProcessProperties.addWidget( &labelRelativeThresholdValue         , line++, col );
  layoutProcessProperties.addWidget( &labelFrequencyTruePeak              , line++, col );

  line = 0; col++;
  layoutProcessProperties.addWidget( &spinAbsoluteThresholdValue          , line++, col );
  layoutProcessProperties.addWidget( &spinRelativeThresholdValue          , line++, col );
  layoutProcessProperties.addWidget( &comboFrequencyTruePeak              , line++, col );

  line = 0; col++;
  layoutProcessProperties.addWidget( &labelAbsoluteThresholdUnit          , line++, col );
  layoutProcessProperties.addWidget( &labelRelativeThresholdUnit          , line++, col );

  line = 0; col = 0;
  layoutParameters.addWidget( &comboSelectNormalisation                   , line++, col );
  layoutParameters.addWidget( &tabProperties                              , line++, col );

  layoutParameters.addWidget( &endOfEdit                                  , line++, col );

  updateValues();

  setDefaultValues( comboSelectNormalisation.currentIndex() );

  connect( &endOfEdit               , SIGNAL( pressed() )               , this, SLOT( closeDialog() ) ) ;

  connect( &endOfEdit               , SIGNAL( pressed() )               , this, SLOT( closeDialog() ) ) ;
  connect( &comboSelectNormalisation, SIGNAL( currentIndexChanged(int) ), this, SLOT( setDefaultValues(int) ));
}

void Properties::updateValues()
{
  programLoudnessLongProgramMaxValue     = spinProgramLoudnessLongProgramMaxValue    .value();
  programLoudnessLongProgramMinValue     = spinProgramLoudnessLongProgramMinValue    .value();
  shortTermLoudnessLongProgramMaxValue   = spinShortTermLoudnessLongProgramMaxValue  .value();
  shortTermLoudnessLongProgramMinValue   = spinShortTermLoudnessLongProgramMinValue  .value();
  programLoudnessShortProgramMaxValue    = spinProgramLoudnessShortProgramMaxValue   .value();
  shortTermLoudnessShortProgramMaxValue  = spinShortTermLoudnessShortProgramMaxValue .value();
  truePeakMaxValue                       = spinTruePeakMaxValue                      .value();
  absoluteThreshold                      = spinAbsoluteThresholdValue                .value();
  relativeThreshold                      = spinRelativeThresholdValue                .value();
  maximalLoudnessRange                   = spinMaximalLoudnessRange                  .value();
  minimalLoudnessRange                   = spinMinimalLoudnessRange                  .value();
  frequencyTruePeak                      = comboFrequencyTruePeak                    .currentIndex();
}

void Properties::closeDialog()
{
  updateValues();
  accept();
}


void Properties::setDefaultValues( int norm )
{
    normalisation = norm;
    switch( normalisation )
    {
	    case 0:
	    {
		setEnableProperties( false );
		Loudness::LoudnessLevels levels = Loudness::LoudnessLevels::Loudness_CST_R017();
		setPropertiesValues( levels );
		break;
	    }
	    case 1:
	    {
		setEnableProperties( false );
		Loudness::LoudnessLevels levels = Loudness::LoudnessLevels::Loudness_EBU_R128();
		setPropertiesValues( levels );
		break;
	    }
	    case 2:
	    {
		setEnableProperties( false );
		Loudness::LoudnessLevels levels = Loudness::LoudnessLevels::Loudness_ATSC_A85();
		setPropertiesValues( levels );
		break;
	    }
	    case 3: setEnableProperties( true ); break;
	    default: setEnableProperties( false ); break;
    }
}


void Properties::setEnableProperties( bool enable )
{
    spinProgramLoudnessLongProgramMinValue.setEnabled    ( enable );
    spinProgramLoudnessLongProgramMaxValue.setEnabled    ( enable );
    spinShortTermLoudnessLongProgramMinValue.setEnabled  ( enable );
    spinShortTermLoudnessLongProgramMaxValue.setEnabled  ( enable );
    spinProgramLoudnessShortProgramMaxValue.setEnabled   ( enable );
    spinProgramLoudnessShortProgramMinValue.setEnabled   ( enable );
    spinShortTermLoudnessShortProgramMaxValue.setEnabled ( enable );
    spinTruePeakMaxValue.setEnabled       ( enable );
    spinAbsoluteThresholdValue.setEnabled ( enable );
    spinRelativeThresholdValue.setEnabled ( enable );
    spinMaximalLoudnessRange.setEnabled   ( enable );
    spinMinimalLoudnessRange.setEnabled   ( enable );
    comboFrequencyTruePeak.setEnabled     ( enable );
}

void Properties::setPropertiesValues( Loudness::LoudnessLevels values )
{
    spinProgramLoudnessLongProgramMaxValue.setValue   ( values.programLoudnessLongProgramMaxValue );
    spinProgramLoudnessLongProgramMinValue.setValue   ( values.programLoudnessLongProgramMinValue );
    spinShortTermLoudnessLongProgramMaxValue.setValue ( values.shortTermLoudnessLongProgramMaxValue );
    spinShortTermLoudnessLongProgramMinValue.setValue ( values.shortTermLoudnessLongProgramMinValue );
    spinProgramLoudnessShortProgramMaxValue.setValue  ( values.programLoudnessShortProgramMaxValue );
    spinProgramLoudnessShortProgramMinValue.setValue  ( values.programLoudnessShortProgramMinValue );
    spinShortTermLoudnessShortProgramMaxValue.setValue ( values.shortTermLoudnessShortProgramMaxValue );
    spinTruePeakMaxValue.setValue          ( values.truePeakMaxValue );
    spinAbsoluteThresholdValue.setValue    ( values.absoluteThresholdValue );
    spinRelativeThresholdValue.setValue    ( values.relativeThresholdValue );
    spinMaximalLoudnessRange.setValue      ( values.maximalLoudnessRange );
    spinMinimalLoudnessRange.setValue      ( values.minimalLoudnessRange );

    if( std::isnan( values.programLoudnessLongProgramMaxValue ) )
	spinProgramLoudnessLongProgramMaxValue.clear();
    if( std::isnan( values.programLoudnessLongProgramMinValue ) )
	spinProgramLoudnessLongProgramMinValue.clear();
    if( std::isnan( values.shortTermLoudnessLongProgramMaxValue ) )
	spinShortTermLoudnessLongProgramMaxValue.clear();
    if( std::isnan( values.shortTermLoudnessLongProgramMinValue ) )
	spinShortTermLoudnessLongProgramMinValue.clear();
    if( std::isnan( values.programLoudnessShortProgramMaxValue ) )
	spinProgramLoudnessShortProgramMaxValue.clear();
    if( std::isnan( values.programLoudnessShortProgramMinValue ) )
	spinProgramLoudnessShortProgramMinValue.clear();
    if( std::isnan( values.shortTermLoudnessShortProgramMaxValue ) )
	spinShortTermLoudnessShortProgramMaxValue.clear();

    if( std::isnan( values.truePeakMaxValue ) )
	spinTruePeakMaxValue.clear();
    if( std::isnan( values.absoluteThresholdValue ) )
	spinAbsoluteThresholdValue.clear();
    if( std::isnan( values.relativeThresholdValue ) )
	spinRelativeThresholdValue.clear();
    if( std::isnan( values.maximalLoudnessRange ) )
	spinMaximalLoudnessRange.clear();
    if( std::isnan( values.minimalLoudnessRange ) )
	spinMinimalLoudnessRange.clear();

    comboFrequencyTruePeak.setCurrentIndex ( 0 );
}

