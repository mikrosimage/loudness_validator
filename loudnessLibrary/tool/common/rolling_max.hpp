///////////////////////////////////////////////////////////////////////////////
// rolling_max.hpp
//
// Marc-Antoine ARNAUD 2012
// copy from rolling_max.hpp
// Copyright 2008 Eric Niebler. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_ACCUMULATORS_STATISTICS_ROLLING_MAX_HPP_EAN_26_12_2008
#define BOOST_ACCUMULATORS_STATISTICS_ROLLING_MAX_HPP_EAN_26_12_2008

#include <boost/mpl/placeholders.hpp>
#include <boost/accumulators/framework/accumulator_base.hpp>
#include <boost/accumulators/framework/extractor.hpp>
#include <boost/accumulators/numeric/functional.hpp>
#include <boost/accumulators/framework/parameters/sample.hpp>
#include <boost/accumulators/framework/depends_on.hpp>
#include <boost/accumulators/statistics_fwd.hpp>
#include <boost/accumulators/statistics/rolling_window.hpp>

#include <algorithm>

namespace boost { namespace accumulators
{

namespace impl
{
///////////////////////////////////////////////////////////////////////////////
// rolling_max_impl
//    returns the max of the samples in the rolling window
template<typename Sample>
struct rolling_max_impl
		: accumulator_base
{
	typedef Sample result_type;
	
	template<typename Args>
	rolling_max_impl(Args const &args)
		: max_(args[sample | Sample()])
	{}
	
	template<typename Args>
	void operator ()(Args const &args)
	{
		if(is_rolling_window_plus1_full(args))
		{
			this->max_ -= rolling_window_plus1(args).front();
		}
		this->max_ += args[sample];
		//this->max_ = std::max( this->max_, args[sample]);
	}
	
	template<typename Args>
	result_type result(Args const &args) const
	{
		return this->max_;
	}
	
private:
	Sample max_;
};
} // namespace impl

///////////////////////////////////////////////////////////////////////////////
// tag::rolling_max
//
namespace tag
{
struct rolling_max
		: depends_on< rolling_window_plus1 >
{
	/// INTERNAL ONLY
	///
	typedef accumulators::impl::rolling_max_impl< mpl::_1 > impl;
	
#ifdef BOOST_ACCUMULATORS_DOXYGEN_INVOKED
	/// tag::rolling_window::window_size named parameter
	static boost::parameter::keyword<tag::rolling_window_size> const window_size;
#endif
};
} // namespace tag

///////////////////////////////////////////////////////////////////////////////
// extract::rolling_max
//
namespace extract
{
	extractor<tag::rolling_max> const rolling_max = {};

	BOOST_ACCUMULATORS_IGNORE_GLOBAL(rolling_max)
}

using extract::rolling_max;

}} // namespace boost::accumulators

#endif
