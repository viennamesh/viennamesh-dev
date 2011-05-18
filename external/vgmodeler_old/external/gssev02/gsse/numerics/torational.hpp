/* ============================================================================
   Copyright (c) 2009-2010 Rene Heinzl                             rene@gsse.at
   Copyright (c) 2009-2010 Franz Stimpfl                          franz@gsse.at   
   Copyright (c) 2009-2010 Josef Weinbub                          josef@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#if !defined(GSSE_NUMERICS_TORATIONAL)
#define GSSE_NUMERICS_TORATIONAL

// *** system includes
// *** BOOST includes
 #include  <boost/type_traits/remove_const.hpp>
// *** GSSE includes
#include "gsse/util/common.hpp"

namespace gsse {  

// -----------------------------------------------------------
//
// FLOAT TO RATIONAL ALGORITHM:
//
//    note: http://www.algorithms.org/MoinMoin/wiki-moinmoin/moin.cgi/FareyRationalNumbers
//
// -----------------------------------------------------------
template<class Int, class Float>
Float
castBI2F(Int bigint)
{
	std::ostringstream ss1;
	ss1 << bigint;
	std::string s1 = ss1.str();
	Float testfloat = boost::lexical_cast<Float>(s1);
	return testfloat;
}

// 2*(lim-1) must be representable as Int


template< typename Rational, typename Float, typename Int > 
Rational
cast_float_2_rational( Float& v, const Int& lim)
{
	typedef Rational Result;
	
	if(v<0)
	{
	   Float vnew = -v;
		return -cast_float_2_rational<Rational, Float, Int>(vnew,lim);
	}

	Int lower0 = 0;
	Int lower1 = 1;
	Int upper0 = 1;
	Int upper1 = 0;

	while(1)
	{
		Int median0 = lower0+upper0;
		Int median1 = lower1+upper1;
		
		// [JW] TODO get rid of it ...
		typedef typename boost::remove_const< Float >::type   FloatNoConst;
		
		FloatNoConst fmedian0=castBI2F<Int,FloatNoConst>(median0);
		FloatNoConst fmedian1=castBI2F<Int,FloatNoConst>(median1);

		//Float fmedian0=castBI2F<Int,Float>(median0);
		//Float fmedian1=castBI2F<Int,Float>(median1);

		
		if(v*fmedian1>fmedian0)
		{
			if(lim<median1)
			{
				return Result(upper0,upper1);
			}
			else
			{
				lower0=median0;
				lower1=median1;
			}
		}
		else if(v*fmedian1<fmedian0)
		{
			if(lim<median1)
			{
				return Result(lower0,lower1);
			}
			else
			{
				upper0=median0;
				upper1=median1;
			}
		}
		else // equal
		{
			if(lim>=median1)
			{
				return Result(median0,median1);
			}
			else if(lower1<upper1)
			{
				return Result(lower0,lower1);
			}
			else
			{
				return Result(upper0,upper1);
			}
		}
	}
}

// -----------------------------------------------------------
//
// TO RATIONAL ALGORITHM 2:
//
//    note: this is the CGAL algorithm
//
// -----------------------------------------------------------
template <typename Rational, typename InputT>
Rational to_rational(InputT& x)
{
//    typedef Fraction_traits<Rational> FT;
//    typedef typename FT::Is_fraction Is_fraction;
//    typedef typename FT::Numerator_type Numerator_type;
//    typedef typename FT::Denominator_type Denominator_type;
//    typename FT::Compose compose;

//    BOOST_STATIC_ASSERT((::boost::is_same<Is_fraction,Tag_true>::value));
//    BOOST_STATIC_ASSERT((::boost::is_same<Numerator_type,Denominator_type>::value));
   

   typedef typename Rational::int_type Numerator_type;

   Numerator_type num(0),den(1);

   if (x != 0.0)      
   { 
      bool neg = (x < 0);
      if (neg) x = -x;
   
      const unsigned shift     = 15;    // a safe shift per step
      const int      shift_pow = 32768; // = 2^shift
      const InputT   width     = 32768; // = 2^shift
      const int      maxiter   = 20;    // ought not be necessary, but just
                                        // in case, max 300 bits of
                                        // precision
      int expt;
      InputT mantissa = std::frexp(x, &expt);
      long exponent = expt;
      InputT intpart;
      int k = 0;
   
      while (mantissa != 0.0 && k++ < maxiter)
      {
         mantissa *= width; // shift double mantissa
         mantissa = std::modf(mantissa, &intpart);
         num *= shift_pow;
         num += (int)intpart;
         exponent -= shift;
      }
      
      int expsign = (exponent>0 ? +1 : (exponent<0 ? -1 : 0));
      exponent *= expsign;
      
      Numerator_type twopot(2);
      Numerator_type exppot(1);

      while (exponent!=0) 
      {
         if (exponent & 1)
            exppot *= twopot;
         
         exponent >>= 1;
         twopot *= twopot;
      }

      if (expsign > 0)
         num *= exppot;
      else if (expsign < 0)
         den *= exppot;     

      if (neg)
         num = -num;
   }

//   return compose(num,den);
   return Rational(num, den);
}

// -----------------------------------------------------------
//
// GENERIC FUNCTORS
//
//    note: those functors wrap the previously defined
//          to rational algorithms
//
// -----------------------------------------------------------

template< typename RationalT, typename RationalTag>
struct torational { };

template<typename RationalT>
struct torational < RationalT, gsse::rational_farey >
{
   template< typename InputT> 
   RationalT
   operator()(InputT& v, long precisionmax = 10000)
   {
      return cast_float_2_rational<RationalT, InputT>(v, precisionmax);
   }
};


template<typename RationalT>
struct torational < RationalT, gsse::rational_cgal >
{
   template <typename InputT>
   RationalT
   operator()(InputT& x, long precisionmax = 0) // precisionmax not used ...
   {
      return to_rational<RationalT, InputT>(x);
   }
};

} // end namespace gsse

#endif
