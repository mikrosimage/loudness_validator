#include "WriteXml.hpp"
#include <ctime>

namespace Loudness {
namespace tools {

WriteXml::WriteXml( const char* filename, const char* srcAudioFilename )
	: srcAudioFilename( srcAudioFilename )
{
	xmlFile.precision( 1 );
	xmlFile.setf( std::ios::fixed, std::ios::floatfield );
	xmlFile.open( filename );
	xmlFile << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n";
	xmlFile << "<loudness>\n";
}

WriteXml::~WriteXml()
{
	xmlFile << "</loudness>\n";
	xmlFile.close();
}

void WriteXml::writeResults( const char* channelType, Loudness::analyser::LoudnessAnalyser& analyser )
{
	xmlFile << "<Program filename=\"" << srcAudioFilename << "\" " 
			<< printStandard( analyser.getStandard() ) << " " 
			<< convertValid( analyser.isValidProgram() ) 
			<< "channelsType=\"" << channelType << "\" " 
			<< "date=\"" << getDate() << "\">\n";
	xmlFile << "\t<ProgramLoudness "      << convertValid( analyser.isIntegratedLoudnessValid() )      << ">" << analyser.getIntegratedLoudness()   << "</ProgramLoudness>\n";
	xmlFile << "\t<LRA "                  << convertValid( analyser.isIntegratedLoudnessRangeValid() ) << ">" << analyser.getIntegratedRange()      << "</LRA>\n";
	xmlFile << "\t<MaxMomentaryLoudness " << convertValid( analyser.isMomentaryLoudnessValid() )       << ">" << analyser.getMomentaryLoudness()    << "</MaxMomentaryLoudness>\n";
	xmlFile << "\t<MaxShortTermLoudness " << convertValid( analyser.isMaxShortTermLoudnessValid() )    << ">" << analyser.getMaxShortTermLoudness() << "</MaxShortTermLoudness>\n";
	xmlFile << "\t<MinShortTermLoudness " << convertValid( analyser.isMinShortTermLoudnessValid() )    << ">" << analyser.getMinShortTermLoudness() << "</MinShortTermLoudness>\n";
	xmlFile << "\t<TruePeak "             << convertValid( analyser.isTruePeakValid() )                << ">" << analyser.getTruePeakInDbTP()       << "</TruePeak>\n";
	xmlFile << "\t<MaxShortTermValues>"   <<  writeValues( analyser.getShortTermValues() ) << "</MaxShortTermValues>\n";
	xmlFile << "\t<TruePeakValues>"       <<  writeValues( analyser.getTruePeakValues() )  << "</TruePeakValues>\n";
	xmlFile << "</Program>\n";
}

std::string WriteXml::convertValid( Loudness::analyser::ELoudnessResult result )
{
	std::string st = "";
	switch( result )
	{
		case Loudness::analyser::eValidResult                 : st = "valid\" ";       break;
		case Loudness::analyser::eNotValidResult              : st = "illegal\" ";     break;
		case Loudness::analyser::eNotValidResultButNotIllegal : st = "not illegal\" "; break;
		case Loudness::analyser::eNoImportance                : break;
	}
	return " status=\'" + st + "'";
}

std::string WriteXml::printStandard( Loudness::analyser::EStandard standard )
{
	std::string st = "";
	switch( standard )
	{
		case Loudness::analyser::eStandardCST_R017 : st = "CST-R017"; break;
		case Loudness::analyser::eStandardEBU_R128 : st = "EBU-R128"; break;
		case Loudness::analyser::eStandardATSC_A85 : st = "ATSC-A85"; break;
		case Loudness::analyser::eStandardUnknown  : st = "Unknown";  break;
	}
	return " standard=\'" + st + "'";
}

std::string WriteXml::writeValues( std::vector<float> datas )
{
	std::ostringstream ss;
	std::vector<float>::iterator it;
	for ( it = datas.begin() ; it < datas.end() - 1; it++ )
		ss << *it << ", ";
	ss << datas.at( datas.size() - 1 );
	return ss.str();
}

std::string WriteXml::getDate()
{
	std::string date = "";
	time_t now;
	struct tm *timeInfo;
	char buffer[32];

	time( &now );
	timeInfo = localtime( &now );
	if( std::strftime( buffer, 32, "%a, %d.%m.%Y %H:%M:%S", timeInfo ) != 0 )
		date.assign( buffer );
	return date;
}

}
}