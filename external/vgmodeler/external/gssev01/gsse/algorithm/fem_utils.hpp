/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 René Heinzl         rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha
     Copyright (c) 2005-2006 Michael Spevak
     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#ifndef GSSE_FEM_UTILS_2006
#define GSSE_FEM_UTILS_2006

namespace gsse
{

template <typename SegType, typename IterType, typename IntArrayType>
void fem_neighborhood(IntArrayType & entry_list, SegType & seg, std::string entry_quan, IterType iter, int start = 0)
{
   iter.reset();
   for(int l = start; iter.valid(); ++iter, ++l)
   {
      entry_list(0, l) = static_cast<int>(seg(*iter, entry_quan)(0, 0));
//       std::cout << "## === entry value: " << entry_list(0,l) << std::endl;
   }
}   

template <typename VecType, typename IterType, typename IntArrayType>
void fem_neighborhood(IntArrayType & entry_list, VecType & vec, IterType iter, int start = 0)
{
   iter.reset();
   for(int l = start; iter.valid(); ++iter, ++l)
   {
      entry_list(0, l) = vec[(*iter).handle()];
   }
}   



template <typename MatrixAccessor, typename RHSAccessor, typename ArrayType, typename IntArrayType>
void assemble_stencil(ArrayType & stencil, IntArrayType & entry_list, MatrixAccessor & mat_add, RHSAccessor & rhs_add)
{
//   std::cout << "next ... " << std::endl;
   for(int i=0; i<entry_list.size(); ++i)
   {
     //   long line = entry_list(i);
//       std::cout << ".. " << entry_list(i) ;
   }
//    std::cout << std::endl  << "========" << std::endl;;
   for(int i=0; i<entry_list.size(); ++i)
   {
      long line = entry_list(i);
      
      for(int j=0; j<entry_list.size(); ++j)
      {
         long column = entry_list(0, j);
         mat_add(line, column, stencil(i, j));
// 	 std::cout << "  " << stencil(i,j);
      }
      int rhs_pos = entry_list.size();
      rhs_add(line, stencil(i, rhs_pos));
//        std::cout << "\t\t" << stencil(i,rhs_pos)<< std::endl;
     
   }
//    std::cout << std::endl;
}

template <typename Iterator, typename Segment, typename ArrayType>
void delete_boundary_line(Iterator iter, Segment & seg, 
			  std::string key_typ, ArrayType & stencil)
{
   iter.reset();
   for (int i=0; iter.valid(); ++iter, ++i)
   {
      if (seg(*iter, key_typ)(0, 0) == 2)
         for (int j=0; j<4; ++j)
         {
            stencil(i, j) = 0.0;
         }
   }
}

template <int size, typename Iterator, typename Segment, typename ArrayType>
void dirichlet_boundary_line(Iterator iter, Segment & seg, 
                             std::string key_typ, std::string key_bnd, 
                             ArrayType & stencil)
{
   iter.reset();
   for (int i=0; iter.valid(); ++iter, ++i)
   {
      if (seg(*iter, key_typ)(0, 0) == 2.0)
      {
         for (int j=0; j<size; ++j)
         {
            stencil(i, j) = 0.0;
            if (i == j) 
               stencil(i, j) = 1.0;
         }
         stencil(i, size) = seg(*iter, key_bnd)(0, 0);
      }
   }
}


}

#endif
