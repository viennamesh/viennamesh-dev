/* ============================================================================
   Copyright (c) 2003-2010 Rene Heinzl                             rene@gsse.at
   Copyright (c) 2008-2010 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2004-2009 Philipp Schwaha                      philipp@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#ifndef GSSE_DETERMINANT_HH
#define GSSE_DETERMINANT_HH

#include <boost/static_assert.hpp>
#include <vector>
#include <iostream>


namespace gsse {
namespace detail {

template <typename MatrixT, typename ResultT, int i>
struct determinant_impl
{
  // BOOST_STATIC_ASSERT(false);
};


template <typename MatrixT, typename ResultT>
struct determinant_impl<MatrixT, ResultT, 3>
{
  ResultT operator()(MatrixT const& m)
  {
    return m(0,0) * m(1,1) * m(2,2) + m(1,0) * m(2,1) * m(0,2) + m(0,1) * m(1,2) * m(2,0) -
       (m(2,0) * m(1,1) * m(0,2) + m(0,0) * m(2,1) * m(1,2) + m(2,2) * m(1,0) * m(0,1));
  }
};

template <typename MatrixT, typename ResultT>
struct determinant_impl<MatrixT, ResultT, 2>
{
  ResultT operator()(MatrixT const& m)
  {
    return m(0,0) * m(1,1) - m(0,1) * m(1,0);
  }
};



template <typename MatrixT, typename ResultT, int Size>
struct determinant_base
{
  typedef determinant_impl<MatrixT, ResultT, Size> det;
  
  ResultT operator()(MatrixT const& m)
  {
    return det()(m);
  }
};

} // namespace detail



template <typename ResultT, int Size, typename MatrixT>
ResultT determinant (MatrixT const& m)
{
   return detail::determinant_base<MatrixT, ResultT, Size>()(m);
}

template <typename ResultT, typename MatrixT>
ResultT determinant_rt (MatrixT & m)
{
   if( m.row_size() == 2 )
      return gsse::determinant< ResultT, 2 >( m );
   else
   if( m.row_size() == 3 )
      return gsse::determinant< ResultT, 3 >( m );
   else
   {
      std::cout << "ERROR::determinant_rt: row size - " << m.row_size() << " - is not supported .. " << std::endl;
      return -1.0;
   }
}


template <typename ResultT, typename MatrixT, typename Size>
ResultT determinant_rt (MatrixT & m, Size size)
{
   if( size == 2 )
      return gsse::determinant< ResultT, 2 >( m );
   else
   if( size == 3 )
      return gsse::determinant< ResultT, 3 >( m );
   else
   {
      std::cout << "ERROR::determinant_rt: row size - " << size << " - is not supported .. " << std::endl;
      return -1.0;
   }
}



// ########################################################


namespace detail {

template <typename MatrixT, int i>
struct inverse_impl
{
  // BOOST_STATIC_ASSERT(false);
};


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


} //namespace detail





template <int Size, typename MatrixT>
MatrixT inverse (MatrixT const& m)
{
   return detail::inverse_impl<MatrixT, Size>()(m);
}

 
}
#endif
