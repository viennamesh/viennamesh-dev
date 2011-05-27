/* =============================================================================
   Copyright (c) 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                 ViennaUtils - The Vienna Device Simulator
                             -----------------

   authors:    Josef Weinbub                         weinbub@iue.tuwien.ac.at
               Karl Rupp                                rupp@iue.tuwien.ac.at


   license:    see file LICENSE in the base directory
============================================================================= */


#ifndef VIENNAUTILS_CONVERTER_HPP
#define VIENNAUTILS_CONVERTER_HPP

#include <sstream>
#include <string>

#include <boost/type_traits/is_same.hpp>
#include <boost/utility/enable_if.hpp>

namespace viennautils {

template<typename TargetT>
struct convert
{
   template<typename SourceT>
   TargetT operator()(SourceT const& source, typename boost::enable_if<boost::is_same<SourceT,TargetT> >::type* dummy = 0)
   {
      return source;
   }

   template<typename SourceT>
   TargetT operator()(SourceT const& source, typename boost::disable_if<boost::is_same<SourceT,TargetT> >::type* dummy = 0)
   {
      TargetT target;
      std::stringstream sstr;
      sstr << source;
      sstr >> target;
      return target;
   }
};

} // end namespace viennautils

#endif




