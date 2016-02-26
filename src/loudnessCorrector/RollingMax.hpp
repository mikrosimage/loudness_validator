#ifndef _LOUDNESS_CORRECTOR_ROLLING_MAX_HPP_
#define _LOUDNESS_CORRECTOR_ROLLING_MAX_HPP_

#include <boost/circular_buffer.hpp>
#include <boost/accumulators/numeric/functional.hpp>

namespace Loudness
{
namespace corrector
{

template <typename Sample>
class RollingMax
{
public:
    RollingMax(size_t windowSize);

    void operator()(Sample sample);

    Sample getMax();

private:
    boost::circular_buffer<Sample> cb;
    Sample max;
};

template <typename Sample>
RollingMax<Sample>::RollingMax(size_t windowSize)
    : cb(windowSize)
    , max(-1.0)
{
}

#include "RollingMax.tcc"
}
}

#endif
