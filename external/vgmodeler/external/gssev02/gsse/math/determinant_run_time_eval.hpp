/* ============================================================================
   Copyright (c) 2010 Philipp Schwaha                           philipp@gsse.at        
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/

#ifndef DETERMINANT_RUN_TIME_HPP
#define DETERMINANT_RUN_TIME_HPP

// boost::mpl includes
//
#include "boost/mpl/for_each.hpp"

namespace gsse
{

// run time evaluaton for determinant
//

template<typename MatrixAccessorType, typename NumericType>
struct multiplication_sequence_eval
{
   const MatrixAccessorType& matrix;
   NumericType& return_value;
   multiplication_sequence_eval(const MatrixAccessorType& matrix, 
                                NumericType& return_value) : 
      matrix(matrix), return_value(return_value) {};

   template< typename U >
   inline void operator()(U x)
   {
      typedef typename U::first first;
      typedef typename U::second second;

      std::cout << " * (" << first::value << " | " << second::value << ") [" 
                << matrix(first::value, second::value) << "] ";// << std::endl;

      return_value *= matrix(first::value, second::value);
   }
};

template<typename MatrixAccessorType, typename NumericType>
struct determinant_structure_eval
{
   const MatrixAccessorType& matrix;
   NumericType& result;

   determinant_structure_eval(const MatrixAccessorType& matrix, NumericType& result) : 
      matrix(matrix), result(result) {};

   template<typename SignedSequenceType>
   inline void operator()(SignedSequenceType X)
   {
      typedef typename SignedSequenceType::first  sign_type;
      typedef typename SignedSequenceType::second sequence_type;

      NumericType local_result(1);

      std::cout << "\t" << sign_type::value << " ";

      boost::mpl::for_each<sequence_type>(multiplication_sequence_eval<MatrixAccessorType, 
                                          NumericType>(matrix, local_result));

      std::cout << " -> \t" << sign_type::value * local_result << std::endl;

      result += sign_type::value * local_result; 

      // return result;
   }
};

template<typename DeterminantStructure>
struct determinant_interface
{

   template<typename MatrixAccessorType, typename NumericType>
   NumericType operator()(const MatrixAccessorType& matrix_access)
   {
      NumericType result(0);
      boost::mpl::for_each<DeterminantStructure>( 
         determinant_structure_eval<MatrixAccessorType, NumericType>(matrix_access, result) 
      );
      return result;
   }

   template<typename MatrixAccessorType, typename NumericType>
   inline void operator()(const MatrixAccessorType& matrix_access, NumericType result)
   {
      result = NumericType(0);
      boost::mpl::for_each<DeterminantStructure>( 
         determinant_structure_eval<MatrixAccessorType, NumericType>(matrix_access, result) 
      );
   }
};

}
#endif // DETERMINANT_RUN_TIME_HPP
