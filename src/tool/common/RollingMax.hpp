
#ifndef ROLLING_MAX_HPP
#define ROLLING_MAX_HPP

#include <boost/circular_buffer.hpp>

template<typename Sample>
class RollingMax
{
public:
	RollingMax( size_t windowSize );
	
	void operator()(Sample sample);

	Sample getMax();
	
private:
	boost::circular_buffer<Sample> cb;
	Sample max;
};

template<typename Sample>
RollingMax<Sample>::RollingMax( size_t windowSize ) :
	cb( windowSize ),
	max( -1.0 )
{
}

#include "RollingMax.tcc"

#endif
