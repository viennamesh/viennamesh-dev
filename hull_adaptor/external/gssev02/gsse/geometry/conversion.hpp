/* ============================================================================
   Copyright (c) 2009-2010 Rene Heinzl                             rene@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#if !defined(GSSE_GEOM_CONVERSION)
#define GSSE_GEOM_CONVERSION

// *** system includes
// *** BOOST includes
#include <boost/mpl/if.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/size.hpp>
#include <boost/type_traits/is_float.hpp>
//#include <boost/type_traits.hpp>
#include <boost/utility/enable_if.hpp>

// *** GSSE includes
#include "gsse/util/common.hpp"


#define GSSE_PRECISION_MAX 10000

// ############################################################
//
namespace gsse {  


// ################################################################
//     cast operators
// ################################################################
template<class Int, class Float>
Float  castBI2F(Int bigint)
{
	std::ostringstream ss1;
	ss1 << bigint;
	std::string s1 = ss1.str();
	
	Float testfloat = boost::lexical_cast<Float>(s1);
	return testfloat;
}

// 2*(lim-1) must be representable as Int


template<class Int, class Float> 
boost::rational<Int> cast_float_2_rational(const Float& v, const Int& lim)
{
	typedef boost::rational<Int> Result;
	// cf. http://www.algorithms.org/MoinMoin/wiki-moinmoin/moin.cgi/FareyRationalNumbers
	if(v<0)
	{
		return -cast_float_2_rational(-v,lim);
	}

	Int lower0 = 0;
	Int lower1 = 1;
	Int upper0 = 1;
	Int upper1 = 0;

	while(1)
	{
		Int median0 = lower0+upper0;
		Int median1 = lower1+upper1;
		Float fmedian0=castBI2F<Int,Float>(median0);
		Float fmedian1=castBI2F<Int,Float>(median1);
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




// template<typename Rational, typename Numerics>
// void 
// convert_to(Rational& rat, const Numerics& num)
// {
//   //  long default_precision = 10000000;    // max for runtime
//   long default_precision = 10;
//
//   for (size_t i = 0; i < rat.size(); ++i)
//     {
//       rat[i] = cast_float_2_rational<   typename Rational::value_type::int_type,
//                                  	typename Numerics::value_type>(num[i], default_precision);
//     }
// }


// template<typename NumericTSink, typename NumericTSource, typename EnableT=void>
// struct conversion_actor
// {
//   void operator()(NumericTSink& rat, const NumericTSource& num)
//   {


//   //  long default_precision = 10000000;    // max for runtime
//   long default_precision = GSSE_PRECISION_MAX;

//   for (size_t i = 0; i < rat.size(); ++i)
//     {
//       rat[i] = cast_float_2_rational<   typename NumericTSink::value_type::int_type,
//                                  	typename NumericTSource::value_type>(num[i], default_precision);
//     }

//   }
// };
  

// ##################################################################
//  direct conversion
//
// struct conversion_actor<NumericTSink,NumericTSource,
// 			typename boost::enable_if< boost::is_float< typename NumericTSink::value_type> >::type  >

template<typename NumericTSink, typename NumericTSource, typename EnableT=void>
struct conversion_actor
{
  void operator()(NumericTSink& rat, const NumericTSource& num)
  {

    for (size_t i = 0; i < static_cast<size_t> (rat.size()); ++i)
      {
	rat[i] = num[i];
      }
  }
};

// template <template <typename ElementType> class Cont>
// template <typename T, template <typename> class Cont = Deque>
// class Stack {




template <typename NumericT>
struct is_rational
{
   enum { value = false };  
};

template<>
struct is_rational<gsse::tag_rational>
{
   enum { value = true };  
};


//	typename boost::enable_if<gsse::is_traversable<typename gsse::traits::tag_of<TestObject>::container_type> >::type >


template<typename NumericTSink, typename NumericTSource>
struct conversion_actor<NumericTSink, NumericTSource, typename boost::enable_if< gsse::is_rational< typename gsse::traits::tag_of<typename NumericTSink::value_type>::numeric_type > >::type  >


{
  void operator()(NumericTSink& rat, const NumericTSource& num)
  {
    long default_precision = GSSE_PRECISION_MAX;

    for (size_t i = 0; i < rat.size(); ++i)
    {
      rat[i] = cast_float_2_rational<   typename NumericTSink::value_type::int_type,
                                 	typename NumericTSource::value_type>(num[i], default_precision);
    }
  }
};




struct numerical_conversion
{
  template<typename NumericTSink, typename NumericTSource>
  void 
  operator()(NumericTSink& rat, const NumericTSource& num)
  {
    conversion_actor<NumericTSink, NumericTSource>  actor;
    actor(rat, num);
  }
};


// #################################################################
template<typename NumericTSink, typename NumericTSource, typename EnableT=void>
struct conversion_actor_simple
{
  void operator()(NumericTSink& rat, const NumericTSource& num)
  {
   
    rat = num;
  }
};

template<typename NumericTSink, typename NumericTSource>
struct conversion_actor_simple<NumericTSink, NumericTSource, 
			       typename boost::enable_if< gsse::is_rational< typename gsse::traits::tag_of<NumericTSink>::numeric_type > >::type  >
{
  void operator()(NumericTSink& rat, const NumericTSource& num)
  {
    long default_precision = GSSE_PRECISION_MAX;
 
    rat = cast_float_2_rational<   typename NumericTSink::int_type,
                                            NumericTSource>(num, default_precision);

  }
};



struct numerical_conversion_simple
{
  template<typename NumericTSink, typename NumericTSource>
  void 
  operator()(NumericTSink& rat, const NumericTSource& num)
  {
    conversion_actor_simple<NumericTSink, NumericTSource>  actor;
    actor(rat, num);
  }
};



} // namespace gsse


#endif




