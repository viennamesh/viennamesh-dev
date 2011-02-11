/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha          philipp@gsse.at
     Copyright (c) 2005-2006 Michael Spevak

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#ifndef GSSE_ORIENT_LAMBDA_HH_ID
#define GSSE_ORIENT_LAMBDA_HH_ID 

// *** system includes
//
#include <iostream>
#include <set>
#include <vector>

// *** BOOST includes
//
// [RH] adaptation to boost >= 1.37
#include <boost/spirit/home/phoenix.hpp>   

// old boost < 1.37
// #include <boost/spirit/phoenix/core.hpp>
// #include <boost/spirit/phoenix/core/composite.hpp>
// #include <boost/spirit/phoenix/core/compose.hpp>
// #include <boost/spirit/phoenix/operator.hpp>
// #include <boost/spirit/phoenix/scope/local_variable.hpp>
// #include <boost/spirit/phoenix/core/nothing.hpp>


// *** GSSE includes
//
#include "gsse/topology.hpp"
#include "gsse/topology/orientation.hpp"
//#include "gsse/debug_meta.hpp"


namespace gsse
{

  namespace detail_math
  {

    struct orient_impl
    {

      template <typename Arg1, typename Arg2>
      struct result
      { 
	typedef double type;
      };
       
      orient_impl() {}
    
      template <typename Arg1, typename Arg2>
      typename result<Arg1, Arg2>::type operator()(Arg1 arg1, Arg2 arg2) const
      { 
//          dump_type<Arg1> ();
//          dump_type<Arg2> ();

         typename boost::phoenix::detail::unwrap_local_reference<Arg1>::type arg1_unw(arg1);
         typename boost::phoenix::detail::unwrap_local_reference<Arg2>::type arg2_unw(arg2);
         
	double return_value = (orient_i(arg1_unw, arg2_unw)?1.0:-1.0);


//	std::cout << "### orient: "<< return_value << std::endl;
	return return_value;
//         return orient_i(arg1_unw, arg2_unw  )?1.0:-1.0;
//         return 1.0;
      }
    };
  }

  boost::phoenix::function<detail_math::orient_impl>  orient;



}

#endif
