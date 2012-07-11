#ifndef WRITE_XML_H
#define WRITE_XML_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <loudnessLibrary.h>

std::string convertValid( Loudness::ELoudnessResult result )
{
	switch( result )
	{
		case Loudness::eValidResult : return " status=\"valid\" "; break;
		case Loudness::eNotValidResult : return " status=\"illegal\" "; break;
		case Loudness::eNotValidResultButNotIllegal : return " status=\"not illegal\" "; break;
		case Loudness::eNoImportance : return " status=\"\" "; break;
	}
}

std::string printStandard( Loudness::EStandard standard )
{
	switch( standard )
	{
		case Loudness::eStandardCST_R017 : return " standard=\"CST-R017\" "; break;
		case Loudness::eStandardEBU_R128 : return " standard=\"EBU-R128\" "; break;
		case Loudness::eStandardATSC_A85 : return " standard=\"ATSC-A85\" "; break;
		case Loudness::eStandardUnknown : return " standard=\"Unknown\" "; break;
	}
}

std::string writeValues( std::vector<float> datas )
{
	std::ostringstream ss;
	std::vector<float>::iterator it;
	for ( it = datas.begin() ; it < datas.end() - 1; it++ )
		ss << *it << ", ";
	ss << datas.at( datas.size() - 1 );
	return ss.str();
}

void writeResults( const char* filename, const char* audioFilename, const char* channelType, Loudness::LoudnessLibrary& analyser )
{
	std::ofstream stat;
	stat.precision( 1 );
	stat.setf( std::ios::fixed, std::ios::floatfield );
	stat.open( filename );
	stat << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n";
	stat << "<Program filename=\"" << audioFilename << "\" " << printStandard( analyser.getStandard() ) << " " << convertValid( analyser.isValidProgram() ) << " >\n";
	stat << "<Channels type=\"" << channelType << "\">\n";
	stat << "\t<ProgramLoudness " << convertValid( analyser.isIntegratedLoudnessValid() ) << ">" << analyser.getIntegratedLoudness() << "</ProgramLoudness>\n";
	stat << "\t<LRA " << convertValid( analyser.isIntegratedLoudnessRangeValid() ) << ">" << analyser.getIntegratedRange() << "</LRA>\n";
	stat << "\t<MaxMomentaryLoudness " << convertValid( analyser.isMomentaryLoudnessValid() ) << ">" << analyser.getMomentaryLoudness() << "</MaxMomentaryLoudness>\n";
	stat << "\t<MaxShortTermLoudness " << convertValid( analyser.isMaxShortTermLoudnessValid() ) << ">" << analyser.getMaxShortTermLoudness() << "</MaxShortTermLoudness>\n";
	stat << "\t<MinShortTermLoudness " << convertValid( analyser.isMinShortTermLoudnessValid() ) << ">" << analyser.getMinShortTermLoudness() << "</MinShortTermLoudness>\n";
	stat << "\t<TruePeak " << convertValid( analyser.isTruePeakValid() ) << ">" << analyser.getTruePeakInDbTP() << "</TruePeak>\n";
	stat << "\t<MaxShortTermValues>" << writeValues( analyser.getShortTermValues() ) << "</MaxShortTermValues>\n";
	stat << "\t<TruePeakValues>" << writeValues( analyser.getTruePeakValues() ) << "</TruePeakValues>\n";
	stat << "</Channels>\n";
	stat << "</Program>\n";
	stat.close();
}

#endif
