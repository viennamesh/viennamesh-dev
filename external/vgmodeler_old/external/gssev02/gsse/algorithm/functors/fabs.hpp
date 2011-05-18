/* ============================================================================
   Copyright (c) 2008-2010 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2003-2010 Rene Heinzl                             rene@gsse.at       

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/

#ifndef GSSE_FUNCTORS_FABS
#define GSSE_FUNCTORS_FABS
//
// ===================================================================================
//
// *** SYSTEM includes
//
#include <algorithm>
//
// *** BOOST includes
//
#include <boost/spirit/home/phoenix/core.hpp>  
#include <boost/spirit/home/phoenix/function.hpp> 
//
// ===================================================================================
//
namespace gsse
{
namespace functional
{
// ===========================================================================================
//
// FUNCTOR
//
// ===========================================================================================
struct fabs_lazy_impl
{    
   template <typename NumericT>
   struct result
   { 
      typedef NumericT  type;
   };
      
   template <typename NumericT>
   typename result<NumericT>::type 
   operator()(NumericT& val)  const
   {
      return fabs(val);
   }
};
// ===========================================================================================
//
// LAZY FUNCTION
//
// ===========================================================================================
boost::phoenix::function<fabs_lazy_impl> const  fabs = fabs_lazy_impl();
} // end namespace: functional
} // end namespace: gsse
#endif
