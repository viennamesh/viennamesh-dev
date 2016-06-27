/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha          philipp@gsse.at
     Copyright (c) 2005-2006 Michael Spevak

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#ifndef FUNCTIONS_HH
#define FUNCTIONS_HH

// *** BOOST includes
//
// [RH] adaptation to boost >= 1.39
#include <boost/spirit/home/phoenix/core.hpp>   
#include <boost/spirit/home/phoenix/operator.hpp>   
#include <boost/spirit/home/phoenix/function.hpp>   
#include <boost/spirit/home/phoenix/scope.hpp>   

// [RH] adaptation to boost >= 1.37
//#include <boost/spirit/home/phoenix.hpp>   

// old boost < 1.37
// #include <boost/spirit/phoenix/core.hpp>
// #include <boost/spirit/phoenix/core/composite.hpp>
// #include <boost/spirit/phoenix/core/compose.hpp>
// #include <boost/spirit/phoenix/operator.hpp>
// #include <boost/spirit/phoenix/scope/local_variable.hpp>
// #include <boost/spirit/phoenix/core/nothing.hpp>

#include "gsse/math/linear_eqn.hpp"
#include "gsse/debug_meta.hpp"

#include<cmath>


namespace gsse
{

  namespace detail_math
  {

    struct norm_impl
    {

      template <typename Arg>
      struct result
      { 
         //typedef typename Arg::numeric_t type; 
         typedef double type;
      };
       
       norm_impl() 
       	{
	//	std::cout << "AN INSTANCE OF NORM HAS BEEN CREATED" << std::endl;
	}
    
      template <typename Arg>
      typename result<Arg>::type 
      operator()(Arg vec) const
      { 
	// *** concept requirement : assignable (double)
	typename result<Arg>::type res = 0.0;        

	// *** concept requirement: += addable (double)

        
        std::for_each(vec.begin(), vec.end(), boost::phoenix::ref(res) += boost::phoenix::arg_names::arg1 * boost::phoenix::arg_names::arg1);

        
	return std::sqrt(res);
      }
    };

    struct sqrt_impl
    {

      template <typename Arg>
      struct result
      { 
	typedef  Arg type; 
      };
    
      template <typename Arg>
      Arg operator()(Arg arg) const
      { 
	// *** concept requirement : assignable (double)

	if (arg < 0 ) arg = 0;

	return std::sqrt(arg);
      }
    };


    struct sign_impl
    {

      template <typename Arg>
      struct result
      { 
	typedef  Arg type; 
      };
    
      template <typename Arg>
      Arg operator()(Arg arg) const
      { 
	return (arg < 0) ? -1.0 : 1.0;
      }
    };


    struct abs_impl
    {

      template <typename Arg>
      struct result 
      { 
	typedef  Arg type; 
      };
    
      template <typename Arg>
      Arg operator()(Arg arg) const
      { 
	return (arg < 0) ? -arg : arg;
      }
    };

    struct sinus_impl
    {

      template <typename Arg>
      struct result 
      { 
	typedef  Arg type; 
      };
    
      template <typename Arg>
      Arg operator()(Arg arg) const
      { 
	return sin(arg);
      }
    };

    struct cosinus_impl
    {

      template <typename Arg>
      struct result 
      { 
	typedef  Arg type; 
      };
    
      template <typename Arg>
      Arg operator()(Arg arg) const
      { 
	return cos(arg);
      }
    };

    struct exponent_impl
    {
      template <typename Arg>
      struct result 
      { 
	typedef Arg type; 
      };
    
      template <typename Arg>
      Arg operator()(Arg arg) const
      { 
	return exp(arg);
      }
    };

    struct bernoulli_impl
    {

      template <typename Arg>
      struct result 
      { 
	typedef Arg type; 
      };

      template <typename Arg>
      //       typename boost::phoenix::detail::unwrap_local_reference<Arg>::type operator()(Arg arg) const
      Arg operator()(Arg arg) const
      { 
         typedef typename boost::phoenix::detail::unwrap_local_reference<Arg>::type  arg_type;
//	  dump_type<Arg>();
//	  dump_type<arg_type>();
         arg_type lq_b =  bern(arg);
         return boost::phoenix::detail::local_reference<arg_type>(lq_b);
      }
    };

//   struct bernoulli_impl2
//    {
//
//      template <typename Arg>
//      struct result 
//      { 
//	typedef Arg type; 
//      };
//    
//      template <typename Arg>
//      Arg operator()(Arg arg) const
//      { 
//	return bern(arg)();
//      }
//    };
//

    struct residual_impl
    {
      template <typename Arg>
      struct result 
      { 
	typedef double type; 
      };
    
      template <typename Arg>
      double operator()(Arg arg) const
      { 
         return arg();
      }
    };


   // added for HD [PS]
   struct log_impl
   {
      template <typename Arg>
      struct result 
      { 
         typedef  Arg type; 
      };
      
      template <typename Arg>
      Arg operator()(Arg arg) const
      { 
//         std::cout << "##### functions.hpp.. log arg: "<< arg << std::endl;
         return log(arg);
      }
   };

  }

  // definition of stateless lambda functions (second order functionals)

   boost::phoenix::function<detail_math::abs_impl>         Abs;
   boost::phoenix::function<detail_math::sqrt_impl>        Sqrt;
   boost::phoenix::function<detail_math::norm_impl>        Norm;
   boost::phoenix::function<detail_math::sign_impl>        Sign;
   boost::phoenix::function<detail_math::sinus_impl>       Sin;
   boost::phoenix::function<detail_math::cosinus_impl>     Cos;
   boost::phoenix::function<detail_math::exponent_impl>    Exp;
   boost::phoenix::function<detail_math::bernoulli_impl>   Bern;
   boost::phoenix::function<detail_math::residual_impl>    Residual;

   // added for HD [PS]
   boost::phoenix::function<detail_math::log_impl>   Log;

}
#endif
