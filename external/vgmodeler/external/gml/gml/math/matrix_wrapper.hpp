/* ============================================================================
   Copyright (c) 2008-2009 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2003-2009 Rene Heinzl                             rene@gsse.at    
   Copyright (c) 2007-2009 Franz Stimpfl                          franz@gsse.at    

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
   
#ifndef GML_MATH_MATRIXWRAPPER
#define GML_MATH_MATRIXWRAPPER
//
// ===================================================================================
//
// *** BOOST includes
//
#include <boost/type_traits/is_pointer.hpp>
#include <boost/type_traits/remove_pointer.hpp>
//
// ===================================================================================
//
namespace gml
{
template< typename Matrix >
struct MatrixWrapper
{
   typedef typename Matrix::numeric_type  matrix_element;

   MatrixWrapper( Matrix & matrix ) : _matrix(matrix) {}
   
   inline 
   typename boost::remove_pointer< matrix_element >::type 
   operator()(int index1, int index2) const
   {
      return access( index1, index2, boost::is_pointer<matrix_element>() );
   }   
   inline 
   typename boost::remove_pointer< matrix_element >::type 
   access(int index1, int index2, boost::true_type const&)  const
   {
      return *_matrix(index1, index2);
   }
   inline 
   typename boost::remove_pointer< matrix_element >::type 
   access(int index1, int index2, boost::false_type const&)  const
   {
      return _matrix(index1, index2);   
   }   

   unsigned int row_size()
   {
      return _matrix.size_1();  // [JW]TODO remove .. gsse::matrix dependency .. !
   }   
   unsigned int col_size()
   {
      return _matrix.size_2();
   }
   
   Matrix & _matrix;
};

} // end namespace: gml
#endif
