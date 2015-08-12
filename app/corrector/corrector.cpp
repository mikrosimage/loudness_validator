
#include <iostream>
#include <cstring>
#include <vector>
#include <cmath>

#include <loudnessAnalyser/LoudnessAnalyser.hpp>
#include <loudnessTools/ProcessFile.hpp>
#include <loudnessTools/io/SoundFile.hpp>
#include <loudnessTools/WriteXml.hpp>

bool showProgress = false;
bool showResults = false;

void progress( int p )
{
	if( showProgress )
		std::cout << "[" << p << "%]\r" << std::flush;
}

int main( int argc, char** argv )
{
	bool validToProcess = false;
	bool analyseAfterCorrecting = false;
	bool enableLimiter = false;
	bool printLength = false;
	bool enableOptimization = true;
	int standard = 1;
	std::vector<std::string> filenames;
	
	float gain = 1.0;
	float lookaheadTime = 60.0;
	
	
	Loudness::ELoudnessResult result = Loudness::eNoImportance;
	
	for(int i = 1; i < argc; i++)
	{
		if( strcmp ( argv[i],"--progress" ) == 0 )
		{
			showProgress = true;
		}
		if( strcmp ( argv[i],"--verbose" ) == 0 )
		{
			showProgress = true;
			showResults = true;
		}
		if( strcmp ( argv[i],"--analyse-corrected" ) == 0 )
		{
			analyseAfterCorrecting = true;
		}
		if( strcmp ( argv[i],"--enable-limiter" ) == 0 )
		{
			enableLimiter = true;
		}
		if( strcmp ( argv[i],"--length" ) == 0 )
		{
			printLength = true;
		}
		if( strncmp ( argv[i],"--lookahead-time=", 17 ) == 0 )
		{
			std::string time = argv[i];
			time.erase( 0, 17 ); // removing 17 first caracters.
			std::stringstream ss;
			float t;
			ss << time;
			ss >> t;
			
			if ( ss.fail() )
			{
				std::cout << "Error: lookahead-time parameter take only a float into value. example: --lookahead-time=20.5" << std::endl;
				return -101;
			}
			if( t != 0 )
			{
				lookaheadTime = t;
			}
		}
		if( strcmp ( argv[i], "--disable-optimization" ) == 0 )
		{
			enableOptimization = false;
		}
		if( strncmp ( argv[i],"--standard=", 11 ) == 0 )
		{
			if( strcmp ( argv[i],"--standard=cst" ) == 0 )
			{
				standard = 0;
			}
			else
			{
				if( strcmp ( argv[i],"--standard=ebu" ) == 0 )
				{
					standard = 1;
				}
				else
				{
					if( strcmp ( argv[i],"--standard=atsc" ) == 0 )
					{
						standard = 2;
					}
					else
					{
						std::cout << "Error: unknown standard specified in command line" << std::endl;
						return -100;
					}
				}
			}
		}
		std::string ext ( argv[i] );
		ext.erase( 0, ext.length() - 5 );
		std::string ext4 = ext; 
		ext4.erase( 0, 1 );
		if( strcmp( ext.c_str(), ".aiff" ) == 0 || strcmp( ext4.c_str(), ".aif" ) == 0 || strcmp( ext4.c_str(), ".wav" ) == 0 )
		{
			filenames.push_back( argv[i] );
			validToProcess = true;
		}
	}
	if( validToProcess )
	{
		for( size_t i=0; i < filenames.size(); i++ )
		{
			std::cout << filenames.at(i) << std::flush;
			
			std::string filename ( filenames.at(i) );
			if( filename.at( filename.length() - 4 ) == '.' )
				filename.erase( filename.length() - 4, 4 );
			else
				if( filename.at( filename.length() - 5 ) == '.' )
					filename.erase( filename.length() - 5, 5 );
			
			
			Loudness::io::SoundFile audioFile;
			Loudness::LoudnessLevels levels =	standard == 0 ? Loudness::LoudnessLevels::Loudness_CST_R017() : 
												standard == 1 ? Loudness::LoudnessLevels::Loudness_EBU_R128() : 
																Loudness::LoudnessLevels::Loudness_ATSC_A85() ;

			Loudness::LoudnessAnalyser loudness( levels );
			if( ! audioFile.open_read ( filenames.at( i ).c_str() ) )
			{
				if( printLength )
					std::cout << "\t length = " << (float) audioFile.getNbSamples() / audioFile.getSampleRate() << "\t" << std::flush;

				Loudness::tool::AnalyseFile analyser( loudness, audioFile );
				analyser.enableOptimization( enableOptimization );
				analyser( progress );

				if( showResults )
					loudness.printPloudValues();
				
				std::string xmlFile = filename;
				xmlFile.append("_measured.xml");
				Loudness::tool::WriteXml writerXml ( xmlFile.c_str(), filenames.at(i).c_str() );
				writerXml.writeResults( "unknown", loudness );
				
				std::string outputFilename = filenames.at(i);
				int insertPoint = 4;
				if( outputFilename.at( outputFilename.length() - 5 ) == '.' )
					insertPoint = 5;
				outputFilename.insert( outputFilename.length() - insertPoint, "_corrected" );
				
				Loudness::io::SoundFile outputAudioFile;
				Loudness::LoudnessAnalyser loudnessAfterCorrection( levels );
				
				if( ! outputAudioFile.open_write( outputFilename.c_str(), audioFile.getAudioCodec(), audioFile.getBitDepth(), audioFile.getSampleRate(), audioFile.getNbChannels() ) )
				{
					gain = loudness.getCorrectionGain( enableLimiter );
					std::cout << " => applying correction: " << gain << std::endl;
					float threshold = std::pow ( 10, ( levels.truePeakTargetLevel ) / 20 );
					
					if( enableLimiter )
					{
						Loudness::tool::CorrectFileWithCompressor corrector( loudnessAfterCorrection, audioFile, outputAudioFile, gain, lookaheadTime, threshold );
						corrector( progress );
					}
					else
					{
						Loudness::tool::CorrectFile corrector( loudnessAfterCorrection, audioFile, outputAudioFile, gain );
						corrector( progress );
					}
					outputAudioFile.close();
				}
				
				audioFile.close();
				
				if( analyseAfterCorrecting )
				{
					if( showResults )
						loudnessAfterCorrection.printPloudValues();
				}
				
				std::string xmlFileCorrected = filename;
				xmlFileCorrected.append("_corrected_measured.xml");
				
				Loudness::tool::WriteXml writerXmlCorrected ( xmlFileCorrected.c_str(), outputFilename.c_str() );
				writerXmlCorrected.writeResults( "unknown", loudnessAfterCorrection );
				result = loudnessAfterCorrection.isValidProgram();
			}
			std::cout << std::endl;
		}
	}
	else
	{
		std::cout << "Loudness Corrector " << std::endl;
		std::cout << "Author: Marc-Antoine ARNAUD" << std::endl << std::endl;
		std::cout << "Common usage :" << std::endl;
		std::cout << "\tloudness-corrector [options] filename.ext" << std::endl << std::endl;
		std::cout << "Options :" << std::endl;
		std::cout << "\t--standard=ebu/cst/atsc : select one standard to validate the Loudness" << std::endl;
		std::cout << "\t\t\tebu:  EBU R 128 (default)" << std::endl;
		std::cout << "\t\t\tcst:  CST RT 017" << std::endl;
		std::cout << "\t\t\tatsc: ATSC A/85" << std::endl;
		std::cout << "\t--progress: show progress status" << std::endl;
		std::cout << "\t--length: print program length" << std::endl;
		std::cout << "\t--verbose: show progress status and print values" << std::endl;
		std::cout << "\t--analyse-corrected: analyse corrected file after writing" << std::endl;
		std::cout << "\t--enable-limiter: activate brick wall look ahead limiter" << std::endl;
		std::cout << "\t--lookahead-time: specify the look-ahead time for the limiter (default is 60ms)" << std::endl;
		return -1;
	}
	
	int returnValue = 0;
	
	switch( result )
	{
		case Loudness::eValidResult: returnValue = 0; break;
		case Loudness::eNotValidResult: returnValue = 2; break;
		case Loudness::eNotValidResultButNotIllegal: returnValue = 1; break;
		case Loudness::eNoImportance: returnValue = 10;break;
	}
	
	if( std::isnan( gain ) )
	{
		returnValue = 5;
	}
	
	
	return returnValue;
}
