
#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <loudnessLibrary.h>
#include <SoundFile.h>

bool showProgress = false;
bool showResults = false;

std::string convertValid( Loudness::ELoudnessResult result )
{
	switch( result )
	{
		case Loudness::eValidResult : return "status=\"valid\""; break;
		case Loudness::eNotValidResult : return "status=\"illegal\""; break;
		case Loudness::eNotValidResultButNotIllegal : return "status=\"not illegal\""; break;
		case Loudness::eNoImportance : return "status=\"\""; break;
	}
}

void writeResults( const char* filename, Loudness::LoudnessLibrary& analyser )
{
	std::ofstream stat;
	stat.precision( 1 );
	stat.setf( std::ios::fixed, std::ios::floatfield );
	stat.open( filename );
	stat << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n";
	stat << "<Program>\n";
	stat << "\t<ProgramLoudness " << convertValid( analyser.isIntegratedLoudnessValid() ) << ">" << analyser.getIntegratedLoudness() << "</ProgramLoudness>\n";
	stat << "\t<LRA " << convertValid( analyser.isIntegratedLoudnessRangeValid() ) << ">" << analyser.getIntegratedRange() << "</LRA>\n";
	stat << "\t<MaxMomentaryLoudness " << convertValid( analyser.isMomentaryLoudnessValid() ) << ">" << analyser.getMomentaryLoudness() << "</MaxMomentaryLoudness>\n";
	stat << "\t<MaxShortTermLoudness " << convertValid( analyser.isMaxShortTermLoudnessValid() ) << ">" << analyser.getMaxShortTermLoudness() << "</MaxShortTermLoudness>\n";
	stat << "\t<MinShortTermLoudness " << convertValid( analyser.isMinShortTermLoudnessValid() ) << ">" << analyser.getMinShortTermLoudness() << "</MinShortTermLoudness>\n";
	stat << "\t<TruePeak " << convertValid( analyser.isTruePeakValid() ) << ">" << analyser.getTruePeakInDbTP() << "</TruePeak>\n";
	stat << "</Program>\n";
	stat.close();
}

void progress( int p )
{
	if( showProgress )
		std::cout << "[" << p << "%]\r" << std::flush;
}

void processAnalyseFile( Loudness::LoudnessLibrary& analyser, SoundFile& audioFile, void (*callback)(int) )
{
	int cumulOfSamples = 0;
	int length = audioFile.size();
	size_t channelsInBuffer = std::min( 5, audioFile.chan() );
	int bufferSize = audioFile.rate () / 5;
	float* data [ channelsInBuffer ];
	float* inpb = new float [ audioFile.chan() * bufferSize ];
	
	for( size_t i = 0; i< channelsInBuffer; i++ )
		data [i] = new float [bufferSize];
	
	analyser.initAndStart( channelsInBuffer, audioFile.rate() );
	
	audioFile.seek( 0 );
	
	while (true)
	{
		int  samples = audioFile.read( inpb, bufferSize );
		if (samples == 0) break;

		cumulOfSamples += samples;
		callback( (float)cumulOfSamples / length * 100 );
		float* p = inpb;
		for (int i = 0; i < samples; i++)
		{
			for(size_t c = 0; c < channelsInBuffer; c++ )
				data [c][i] = (*p++);
		}
		analyser.processSamples( data, samples );
	}
	
	delete[] inpb;
	for( int i=0; i< audioFile.chan(); i++ )
		delete[] data[i];
}


int main( int argc, char** argv )
{
	bool validToProcess = false;
	int standard = 0;
	std::vector<std::string> filenames;
	
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
		if( strncmp ( argv[i],"standard=", 9 ) == 0 )
		{
			if( strncmp ( argv[i],"standard=ebu", 12 ) == 0 )
			{
				standard = 0;
			}
			else
			{
				if( strncmp ( argv[i],"standard=cst", 12 ) == 0 )
				{
					standard = 1;
				}
				else
				{
					if( strncmp ( argv[i],"standard=atsc", 13 ) == 0 )
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
			std::cout << filenames.at(i) << std::endl;
			
			std::string filename ( filenames.at(i) );
			if( filename.at( filename.length() - 4 ) == '.' )
				filename.erase( filename.length() - 4, 4 );
			else
				if( filename.at( filename.length() - 5 ) == '.' )
					filename.erase( filename.length() - 5, 5 );
			filename.append("_measured.xml");
			
			SoundFile audioFile;
			Loudness::LoudnessLevels levels =	standard == 0 ? Loudness::LoudnessLevels::Loudness_CST_R017() : 
												standard == 1 ? Loudness::LoudnessLevels::Loudness_EBU_R128() : 
																Loudness::LoudnessLevels::Loudness_ATSC_A85() ;

			Loudness::LoudnessLibrary analyser( levels );
			if( ! audioFile.open_read ( filenames.at( i ).c_str() ) )
			{
				processAnalyseFile( analyser, audioFile, progress );
				if( showResults )
					analyser.printPloudValues();
				audioFile.close();
				writeResults( filename.c_str(), analyser );
			}
		}
	}
	else
	{
		std::cout << "PLoud Analyser" << std::endl;
		std::cout << "Mikros Image - Marc-Antoine ARNAUD [mrn@mikrosimage.eu]" << std::endl << std::endl;
		std::cout << "Common usage :" << std::endl;
		std::cout << "\tloudness-analyser [options] filename.ext" << std::endl << std::endl;
		std::cout << "Options :" << std::endl;
		std::cout << "\tstandard=ebu/cst/atsc : select one standard to validate the Loudness" << std::endl;
		std::cout << "\t\t\tebu:  EBU R 128 (default)" << std::endl;
		std::cout << "\t\t\tcst:  CST RT 017" << std::endl;
		std::cout << "\t\t\tatsc: ATSC A/85" << std::endl;
		return -1;
	}
	return 0;
}
