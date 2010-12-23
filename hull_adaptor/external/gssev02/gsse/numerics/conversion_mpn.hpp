/* ============================================================================
   Copyright (c) 2009-2010 Josef Weinbub                          josef@gsse.at   
   Copyright (c) 2009-2010 Rene Heinzl                             rene@gsse.at
   Copyright (c) 2009-2010 Franz Stimpfl                          franz@gsse.at   
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#if !defined(GSSE_NUMERICS_CONVERSION_MPN)
#define GSSE_NUMERICS_CONVERSION_MPN

// *** system includes
// *** BOOST includes
#include <boost/lexical_cast.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_function.hpp>
// *** GSSE includes
#include "gsse/util/common.hpp"
#include "gsse/numerics/torational.hpp"
#include "gsse/util/resize.hpp"
#include "gsse/util/size.hpp"
#include "gsse/numerics/conversion.hpp"
// *** GSSE external includes
#include "numerics/mpfr/install/include/mpfr.h"
#include "numerics/mpfrC++/install/mpreal.h"
// ############################################################
//
namespace gsse {  

namespace detail {
//
// -----------------------------------------------------------
//
template<typename T>
struct numerical_conversion_scalar_impl <boost::rational<T> >
{ 
   template< typename Source, typename Sink >
   static void eval(Source& source, Sink& sink)
   {
//      std::cout << "## numerical_conversion_scalar_impl::rational spec .. " << std::endl;
      
      // [JW] TODO using the farey algorithm for now .. 
      // we need a more generic approach, though. 
      //
      //typedef gsse::torational< Sink, gsse::rational_cgal >    torational_type;
      typedef gsse::torational< Sink, gsse::rational_farey >   torational_type;      
      torational_type ToRational;
      sink = ToRational(source);
   }

   template< typename Source, typename Sink >
   static void eval(Source const& source, Sink& sink)
   {
//      std::cout << "## numerical_conversion_scalar_impl::rational spec .. " << std::endl;
      
      // [JW] TODO using the farey algorithm for now .. 
      // we need a more generic approach, though. 
      //
      //typedef gsse::torational< Sink, gsse::rational_cgal >    torational_type;
      typedef gsse::torational< Sink, gsse::rational_farey >   torational_type;      
      torational_type ToRational;
      sink = ToRational(source);
   }   
};

template<>
struct numerical_conversion_scalar_impl <mpfr::mpreal >
{ 
   template< typename Source, typename Sink >
   static void eval(Source& source, Sink& sink)
   {
//      std::cout << "## numerical_conversion_scalar_impl::mpreal spec .. " << std::endl;
      
      std::string sink_str = boost::lexical_cast<std::string>(source);
      sink = sink_str.c_str();
   }
   
   template< typename Source, typename Sink >
   static void eval(Source const& source, Sink& sink)
   {
//      std::cout << "## numerical_conversion_scalar_impl::mpreal spec .. " << std::endl;
      
      std::string sink_str = boost::lexical_cast<std::string>(source);
      sink = sink_str.c_str();
   }   
};

} // end namespace detail

} // end namespace gsse

#endif
