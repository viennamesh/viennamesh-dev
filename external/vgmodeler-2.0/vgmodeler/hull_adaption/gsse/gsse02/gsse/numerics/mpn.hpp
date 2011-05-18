/* ============================================================================
   Copyright (c) 2009-2010 Josef Weinbub                          josef@gsse.at
   Copyright (c) 2003-2010 Rene Heinzl                             rene@gsse.at   
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#ifndef GSSE_MPN_HH_ID
#define GSSE_MPN_HH_ID 

// *** system includes
// *** BOOST includes
#include "boost/rational.hpp"
// *** GSSE includes
#include "gsse/numerics/conversion_mpn.hpp"
// *** GSSE EXTERNAL includes
#include "numerics/numerics.hpp"

namespace gsse {
namespace numerics {

// ----------------------------------------
namespace tag {

struct bigint           {};
struct cln              {};
struct gmp              {};
struct boost_rational   {};
struct ttmath           {};
struct apfloat          {};
struct bigfloat         {};
struct mpfr             {};
struct arprec           {};
struct qd               {};

} // end namespace tag
// ----------------------------------------


// ----------------------------------------
template <  typename Tag, 
            typename DataType = void, 
            int      Exponent = 8, 
            int      Mantissa = 4 >
struct mpn { };


template < >
struct mpn < gsse::numerics::tag::bigint >
{ 
  // [RH] .. more information external/numerics/bigint/install/bigint_2008.hpp
  //
  typedef boost::mp_math::mp_int<std::allocator<void>, 
				 boost::mp_math::mp_int_traits<unsigned int, long long unsigned int> > type;
};

template < >
struct mpn < gsse::numerics::tag::cln >
{ 
   typedef boost::numeric_adaptor::cln_value_type  type; 
};

template < >
struct mpn < gsse::numerics::tag::gmp >
{ 
   typedef boost::numeric_adaptor::gmp_value_type  type;   
};

template < >
struct mpn < gsse::numerics::tag::apfloat >
{ 
   typedef apfloat                                 type; 
};

template < >
struct mpn < gsse::numerics::tag::bigfloat >
{ 
   typedef BigFloat                                type; 
};

template < >
struct mpn < gsse::numerics::tag::arprec >
{ 
   typedef mp_real                                 type; 
};

template < >
struct mpn < gsse::numerics::tag::qd >
{
   //[JW] QD supports double-double(32 digits) and 
   // quad-double(64 digits)
   //typedef qd_real                                 type;  
   typedef dd_real                                 type; 
};


template < >
struct mpn < gsse::numerics::tag::mpfr >
{ 
   typedef mpfr::mpreal                            type; 
};


template < typename DataType >
struct mpn < gsse::numerics::tag::boost_rational, DataType >
{ 
   typedef boost::rational<DataType>               type;   
};


// [RH][TODO] .. check float
//
template < int Exponent, int Mantissa>
struct mpn < gsse::numerics::tag::ttmath, float, Exponent, Mantissa >
{ 
   // ttmat::Big < exponent, mantissa >.. this type holds floating point numbers
   // value = mantissa * 2^exponent	
   //
   typedef ttmath::Big<Exponent,Mantissa>          type;   
};
// ----------------------------------------
} // end namespace numerics
} // end namespace gsse

#endif
