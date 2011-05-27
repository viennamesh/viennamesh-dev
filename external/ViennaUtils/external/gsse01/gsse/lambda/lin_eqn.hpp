/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha          philipp@gsse.at
     Copyright (c) 2005-2006 Michael Spevak

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#ifndef LINEAR_EQUATION_ACCESS_HH
#define LINEAR_EQUATION_ACCESS_HH

// *** BOOST includes
//
#include <boost/spirit/phoenix/core.hpp>
#include <boost/spirit/phoenix/function.hpp>

// *** GSSE includes
//
#include "gsse/math/linear_eqn.hpp"

namespace gsse
{
   // boost::phoenix function
   //
   struct linear_equation_access_impl
   {    
      template <typename Arg1, typename Arg2>
      struct result
      {
         typedef linear_equation<Arg1> type;
      };

      linear_equation_access_impl() {
         std::cout << "#### .. in equ construtor.. " << std::endl;}
      
      template <typename Arg1, typename Arg2>
      typename result<Arg1, Arg2>::type operator()(Arg1 const& arg1, Arg2 const& arg2) const
         {
            std::cout << "## lineq (arg1/arg2): " << arg1 << "/" << arg2 << std::endl;
            return typename result<Arg1, Arg2>::type(static_cast<long>(arg2), arg1);
         }
   };

   // simple boost::phoenix::object generator 
   //
   boost::phoenix::function<linear_equation_access_impl>
   lin_eqn_access()   
   {
      return boost::phoenix::function<linear_equation_access_impl>(linear_equation_access_impl());
   }
      
   // actor: provided as metafunction
   //
   struct lin_eqn_access_t
   {
      typedef boost::phoenix::function<linear_equation_access_impl> type;
   };

   lin_eqn_access_t::type lineqn = lin_eqn_access(); 

}

#endif
