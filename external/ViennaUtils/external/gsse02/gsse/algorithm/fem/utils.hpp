/* ============================================================================
   Copyright (c) 2008-2009 Rene Heinzl                             rene@gsse.at
   
   Distributed under the Boost Software License, Version 1.0. 
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
#if !defined(GSSE_FEM_UTILS_INCLUDE_HH)
#define GSSE_FEM_UTILS_INCLUDE_HH

// *** system includes
#include<vector>
// *** GSSE includes
#include "gsse/util/size.hpp"
#include "gsse/math/matrix_ops.hpp"

// ############################################################
//



namespace gsse { namespace fem{

template <typename ArrayType>
void delete_line(ArrayType & stencil, long line_nb)
{
   for (long ci = 0; ci < stencil.size_2(); ++ci)
   {
      stencil(line_nb, ci) = 0.0;
      if (line_nb == long(ci))
         stencil(line_nb, ci) = 1.0;
         
   }
}

// template <typename ArrayType, typename ArrayType2, typename IntArrayType, typename MatrixAccessor, typename RHSAccessor>
// void assemble_stencil(ArrayType & stencil, ArrayType2 rhs, IntArrayType & entry_list, MatrixAccessor & mat_add, RHSAccessor & rhs_add)
// {
//    for(size_t i=0; i<entry_list.size(); ++i)
//    {
//       long row = entry_list[i];
      
//       for(size_t j=0; j<entry_list.size(); ++j)
//       {
//          long column = entry_list[j];
//          mat_add(row, column, stencil(i, j));
//       }
//       rhs_add(row, rhs[i]);
//    }
// }

template <typename ArrayType, typename IntArrayType, typename MatrixAccessor>
void assemble_stencil(ArrayType & stencil, IntArrayType & entry_list, MatrixAccessor & mat_add)
{
#ifdef GSSE_DEBUG_FULLOUTPUT
   std::cout << "assemble stencil " << std::endl;
#endif

   for(size_t i=0; i < gsse::size(entry_list); ++i)
   {
      long row = entry_list[i];
      
      for(size_t j=0; j < gsse::size(entry_list); ++j)
      {
         long column = entry_list[j];
         mat_add(row, column, stencil(i, j));
      }
   }
}

template <typename ArrayType, typename IntArrayType, typename RHSAccessor>
void assemble_rhs(ArrayType & stencil, IntArrayType & entry_list, RHSAccessor& vec_add)
{
   for(size_t row_i=0; row_i < gsse::size(entry_list); ++row_i)
   {
      vec_add(entry_list[row_i], stencil(row_i));
//      vec_add(entry_list[row_i], stencil(row_i,0));
   }
}


// #####
// 



} // namespace fem
} // namespace gsse

#endif


