/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#ifndef GSSE_ADAPTORS_HH
#define GSSE_ADAPTORS_HH

// *** system
//
#include<iostream>


// *** BOOST
//
#include <boost/mpl/if.hpp>
#include <boost/type_traits.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>
#include <boost/tuple/tuple_io.hpp>


namespace gsse
{

typedef boost::tuples::tuple<long,long>  boost_tuple_type;


struct mapped_datatypes
{

  // if both datatypes are the same, then
  // return the datatype without modification
  //
  template <typename FirstOp, typename SecondOp>
  struct apply
  {
    FirstOp operator()(SecondOp handle)
    {
      return handle;
    }
  };
  
  // partial specialization for boost tuples
  //
  template <typename FirstOp>
  struct apply<FirstOp, boost_tuple_type>
  {
    FirstOp operator()(boost_tuple_type handle)
    {
      return boost::tuples::get<0>(handle);
    }
  };
  
 };



template <typename DataT, typename Function>
int inner_function(DataT& data_t)
{
  typedef long numeric_t;

  typedef typename Function::template apply<numeric_t,numeric_t> function1_v;
  function1_v fv1;
  std::cout << "output1: " << fv1(data_t) << std::endl;

  // ===================

  typedef typename Function::template apply<numeric_t,boost_tuple_type> function2_v;
  function2_v fv2;
  std::cout << "output2: " << fv2(data_t) << std::endl;


  return 0;
}



template <typename DataT, typename Function>
int overall_function(DataT& data)
{
  inner_function<DataT,Function>(data);
  
   return 0;
}   

}


#endif
