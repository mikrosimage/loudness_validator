
#include <iostream>
#include <cstring>
#include <vector>
#include <cmath>

#include <loudnessLibrary.h>
#include <SoundFile.h>
#include <WriteXml.h>
#include <ProcessFile.h>
#include <WriteCorrectedFile.h>

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
	bool printLength = false;
	int standard = 0;
	std::vector<std::string> filenames;
	
	float gain = 1.0;
	
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
		if( strcmp ( argv[i],"--length" ) == 0 )
		{
			printLength = true;
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
			
			
			SoundFile audioFile;
			Loudness::LoudnessLevels levels =	standard == 0 ? Loudness::LoudnessLevels::Loudness_CST_R017() : 
												standard == 1 ? Loudness::LoudnessLevels::Loudness_EBU_R128() : 
																Loudness::LoudnessLevels::Loudness_ATSC_A85() ;

			Loudness::LoudnessLibrary analyser( levels );
			if( ! audioFile.open_read ( filenames.at( i ).c_str() ) )
			{
				if( printLength )
					std::cout << "\t length = " << (float) audioFile.size() / audioFile.rate() << "\t" << std::flush;
				processAnalyseFile( analyser, audioFile, progress );
				if( showResults )
					analyser.printPloudValues();
				
				std::string xmlFile = filename;
				xmlFile.append("_measured.xml");
				writeResults( xmlFile.c_str(), analyser );
				
				std::string outputFilename = filenames.at(i);
				int insertPoint = 4;
				if( outputFilename.at( outputFilename.length() - 5 ) == '.' )
					insertPoint = 5;
				outputFilename.insert( outputFilename.length() - insertPoint, "_corrected" );
				
				//std::cout << " => "<< outputFilename << std::endl;
				
				SoundFile outputAudioFile;
				
				if( ! outputAudioFile.open_write( outputFilename.c_str(), audioFile.type(), audioFile.form(), audioFile.rate(), audioFile.chan() ) );
				{
					gain = analyser.getCorrectionGain();
					std::cout << " => applying correction: " << gain;
					writeCorrectedFile( audioFile, outputAudioFile, gain, progress );
					outputAudioFile.close();
				}
				
				audioFile.close();
				
				if( analyseAfterCorrecting )
				{
					if( ! audioFile.open_read ( outputFilename.c_str() ) )
					{
						processAnalyseFile( analyser, audioFile, progress );
						if( showResults )
							analyser.printPloudValues();
	
						std::string xmlFile = filename;
						xmlFile.append("_corrected_measured.xml");
						writeResults( xmlFile.c_str(), analyser );
					}
				}
				result = analyser.isValidProgram();
			}
			std::cout << std::endl;
		}
	}
	else
	{
		std::cout << "PLoud Corrector" << std::endl;
		std::cout << "Mikros Image - Marc-Antoine ARNAUD [mrn@mikrosimage.eu]" << std::endl << std::endl;
		std::cout << "Common usage :" << std::endl;
		std::cout << "\tloudness-analyser [options] filename.ext" << std::endl << std::endl;
		std::cout << "Options :" << std::endl;
		std::cout << "\t--standard=ebu/cst/atsc : select one standard to validate the Loudness" << std::endl;
		std::cout << "\t\t\tebu:  EBU R 128 (default)" << std::endl;
		std::cout << "\t\t\tcst:  CST RT 017" << std::endl;
		std::cout << "\t\t\tatsc: ATSC A/85" << std::endl;
		std::cout << "\t--progress: show progress status" << std::endl;
		std::cout << "\t--verbose: show progress status and print values" << std::endl;
		std::cout << "\t--analyse-corrected: analyse corrected file after writing" << std::endl;
		return -1;
	}
	
	int returnValue;
	
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
