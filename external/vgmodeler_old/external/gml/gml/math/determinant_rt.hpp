/* ============================================================================
   Copyright (c) 2008-2009 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2003-2009 Rene Heinzl                             rene@gsse.at       

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
   
#ifndef GML_MATH_DETERMINANTRT
#define GML_MATH_DETERMINANTRT
//
// ===================================================================================
//
//
// *** GSSE includes
//
#include <gsse/math/determinant.hpp>
//
// ===================================================================================
//
namespace gml
{

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


} // end namespace: gml

#endif
