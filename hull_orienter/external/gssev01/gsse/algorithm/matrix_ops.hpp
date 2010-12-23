/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha          philipp@gsse.at
     Copyright (c) 2005-2006 Michael Spevak

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */


#ifndef GSSE_MATRIX_OPS
#define GSSE_MATRIX_OPS

template<int size1, typename MatrixT>
bool is_symmetric(MatrixT & mat)
{

   for (int l=0; l<size1; ++l)
      for (int m=l+1; m<size1; ++m)
      {
         if (mat(l, m) != mat(m, l)) 
         {
            std::cout << "l = " << l << " m = " << m << std::endl;
            return false;
         }
      }
   return true;
}

template<int size1, typename MatrixT>
bool is_diagonal_dominant(MatrixT & mat)
{
   for (int l=0; l<size1; ++l)
   {
      double sum = 0.0;
      for (int m=0; m<size1; ++m)
      {
         sum += fabs(mat(l, m));
      }
      if (mat(l, l)*2.0 <= sum) return false;
   }

   return true;
}

template<int size1, typename MatrixT>
bool row_sum_zero(MatrixT & mat)
{
   for (int l=0; l<size1; ++l)
   {
      double sum = 0.0;
      for (int m=0; m<size1; ++m)
      {
         sum += mat(l, m);
      }
      if (fabs(sum) > 0.0001) return false;
   }

   return true;
}


template<int size1, typename MatrixT>
bool is_positive_definit(MatrixT & mat)
{
   if (mat(0, 0) < 0) return false;

   if (mat(0, 0) * mat(1, 1) - mat(1, 0) * mat(0, 1) < 0) return false;

   if (mat(0, 0) * mat(1, 1) * mat(2, 2) + 
       mat(0, 1) * mat(1, 2) * mat(2, 0) +
       mat(0, 2) * mat(1, 0) * mat(2, 1) -
       mat(0, 0) * mat(1, 2) * mat(2, 1) -
       mat(1, 0) * mat(2, 2) * mat(0, 1) -
       mat(2, 0) * mat(0, 2) * mat(1, 1) < 0) return false;

   return true;
}


#endif
