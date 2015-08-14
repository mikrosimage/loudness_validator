#include "AvSoudFile.hpp"

#include <loudnessAnalyser/LoudnessAnalyser.hpp>

#include <vector>
#include <utility>
#include <fstream>

std::vector<std::pair<std::string, size_t> > parseConfigFile( const std::string& configFilename )
{
	std::vector<std::pair<std::string, size_t> > result;

	std::ifstream configFile( configFilename.c_str(), std::ifstream::in );

	std::string line;
	while( std::getline( configFile, line ) )
	{
		std::istringstream is_line( line );
		std::string filename;
		if( std::getline( is_line, filename, '=' ) )
		{
			std::string streamId;
			if( std::getline( is_line, streamId, ':' ) )
			{
				std::string transcodeProfile;
				std::getline( is_line, transcodeProfile );

				std::stringstream ss( streamId );
				size_t streamIndex = 0;
				ss >> streamIndex;

				result.push_back(std::make_pair(filename, streamIndex));
			}
		}
	}

	configFile.close();

	return result;
}

int main( int argc, char** argv )
{
	std::vector< std::string > arguments;
	for( int argument = 1; argument < argc; ++argument )
	{
		arguments.push_back( argv[argument] );
	}

	for( size_t argument = 0; argument < arguments.size(); ++argument )
	{
		if( arguments.at( argument ) == "--help" )
		{
			std::string help;
			help += "Usage\n";
			help += "\tmedia-analyser CONFIG.TXT [--help]\n";
			help += "CONFIG.TXT\n";
			help += "\tEach line will be one audio stream analysed by the loudness library.\n";
			help += "\tPattern of each line is:\n";
			help += "\t[inputFile]=STREAM_ID\n";
			help += "Command line options\n";
			help += "\t--help: display this help\n";
			std::cout << help << std::endl;
			return 0;
		}
	}

	// Check required arguments
	if( argc < 2 )
	{
		std::cout << "completeAnalyser requires a media filename" << std::endl;
		std::cout << "Use option --help to display help" << std::endl; 
		return( -1 );
	}

	avtranscoder::preloadCodecsAndFormats();
	avtranscoder::Logger::setLogLevel(AV_LOG_QUIET);

	// Get list of files / streamIndex to analyse
	std::vector<std::pair<std::string, size_t> > arrayToAnalyse = parseConfigFile(arguments.at(0));
	AvSoudFile soudFile(arrayToAnalyse);

	// Analyse loudness according to EBU R-128
	Loudness::analyser::LoudnessLevels level = Loudness::analyser::LoudnessLevels::Loudness_EBU_R128();
	Loudness::analyser::LoudnessAnalyser analyser(level);
	soudFile.analyse(analyser);

	// Print analyse
	analyser.printPloudValues();
}
