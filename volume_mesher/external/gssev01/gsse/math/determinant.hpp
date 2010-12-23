/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha          philipp@gsse.at
     Copyright (c) 2005-2006 Michael Spevak

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#ifndef GSSE_DETERMINANT_HH
#define GSSE_DETERMINANT_HH

#include <boost/static_assert.hpp>
#include <vector>
#include <iostream>


namespace gsse
{

// this is a hard coded determinant class which provides fast
// determinant calculation for small matrices. 
//


template <typename MatrixT, typename ResultT, int i>
struct determinant_impl
{
  // BOOST_STATIC_ASSERT(false);
};


// determinant implementation for three dimensions
//
template <typename MatrixT, typename ResultT>
struct determinant_impl<MatrixT, ResultT, 3>
{
  ResultT operator()(MatrixT const& m)
  {
    return m(0,0) * m(1,1) * m(2,2) + m(1,0) * m(2,1) * m(0,2) + m(0,1) * m(1,2) * m(2,0) -
       (m(2,0) * m(1,1) * m(0,2) + m(0,0) * m(2,1) * m(1,2) + m(2,2) * m(1,0) * m(0,1));
  }
};

// determinant implementation for two dimensions
//
template <typename MatrixT, typename ResultT>
struct determinant_impl<MatrixT, ResultT, 2>
{
  ResultT operator()(MatrixT const& m)
  {
    return m(0,0) * m(1,1) - m(0,1) * m(1,0);
  }
};

// function object which encapsulates the partial specializations
// into one homogeneous class
//
template <typename MatrixT, typename ResultT, int Size>
struct determinant_base
{
  typedef determinant_impl<MatrixT, ResultT, Size> det;
  
  ResultT operator()(MatrixT const& m)
  {
    return det()(m);
  }
};

// object generator for the determinant_base
//
template <typename ResultT, int Size, typename MatrixT>
ResultT determinant (MatrixT const& m)
{
  return determinant_base<MatrixT, ResultT, Size>()(m);
}


///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////

template <typename MatrixT, int i>
struct inverse_impl
{
  // BOOST_STATIC_ASSERT(false);
};


// inverse implementation for two dimensions
//
template <typename MatrixT>
struct inverse_impl<MatrixT, 3>
{
   MatrixT operator()(MatrixT const& m)
  {
     MatrixT result(3, 3);

     typedef typeof(result(0, 0)) value_t;

     value_t det = determinant<typeof(result(0, 0)), 3>(m);

     result(0, 0) = (m(1, 1) * m(2, 2) - m(1, 2) * m(2, 1)) / det;
     result(1, 0) = (m(1, 2) * m(2, 0) - m(1, 0) * m(2, 2)) / det;
     result(2, 0) = (m(1, 0) * m(2, 1) - m(1, 1) * m(2, 0)) / det;
     result(0, 1) = (m(0, 2) * m(2, 1) - m(0, 1) * m(2, 2)) / det;
     result(1, 1) = (m(0, 0) * m(2, 2) - m(0, 2) * m(2, 0)) / det;
     result(2, 1) = (m(0, 1) * m(2, 0) - m(0, 0) * m(2, 1)) / det;
     result(0, 2) = (m(0, 1) * m(1, 2) - m(0, 2) * m(1, 1)) / det;
     result(1, 2) = (m(0, 2) * m(1, 0) - m(0, 0) * m(1, 2)) / det;
     result(2, 2) = (m(0, 0) * m(1, 1) - m(0, 1) * m(1, 0)) / det;

     return result;
  }
};

// inverse implementation for three dimensions
//
template <typename MatrixT>
struct inverse_impl<MatrixT, 2>
{
   MatrixT operator()(MatrixT const& m)
      {
         MatrixT result(2, 2);

         typedef typeof(result(0, 0)) value_t;
         
         value_t det = determinant<typeof(result(0, 0)), 2>(m);
         
         result(0, 0) = (m(1, 1))  / det;
         result(1, 0) = (-m(1, 0)) / det;
         result(0, 1) = (-m(0, 1)) / det;
         result(1, 1) = (m(0, 0))  / det;

         return result;
      }
};

// object generator for the determinant_base
//
template <int Size, typename MatrixT>
MatrixT inverse (MatrixT const& m)
{
   return inverse_impl<MatrixT, Size>()(m);
}

 
}
#endif
