///////////////////////////////////////////////////////////////////////////////
// RollingMax.hpp
//
// Marc-Antoine ARNAUD 2012

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
	cb( windowSize )
{
}

template<typename Sample>
void RollingMax<Sample>::operator()(Sample sample)
{
	
	Sample removedSample( 0.0 );
	if( cb.full() )
		removedSample = cb.front();
	
	cb.push_back( sample );
	
	if( removedSample == max && sample < max )
	{
		max = 0;
		for( size_t i = 0; i < cb.size(); i++ )
		{
			boost::numeric::max_assign( max, cb.at(i));
		}
	}
	boost::numeric::max_assign( max, sample );
}

template<typename Sample>
Sample RollingMax<Sample>::getMax()
{
	return max;
}

#endif
