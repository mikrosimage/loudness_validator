#include <iostream>
#include <fstream>
#include <loudnessLibrary.h>

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


