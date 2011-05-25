/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 René Heinzl         rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha
     Copyright (c) 2005-2006 Michael Spevak
     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#ifndef GSSE_FEM_MASS_2006
#define GSSE_FEM_MASS_2006

#include "gsse/topology.hpp"

namespace gsse
{

template <typename ArrayType, typename Archetype, int dim>
struct mass {};


template <typename ArrayType>
struct mass<ArrayType, simplex_topology_tag, 3>
{
   ArrayType M;

   mass() : M(4, 5)
      {
         for(int i=0; i<4; ++i)
            for(int j=0; j<4; ++j)
               if (i == j) 
                  M(i, j) = 1./60.;
               else
                  M(i, j) = 1./120.;
      }

   ArrayType operator()(ArrayType & det)
      {
         ArrayType result(4, 5);
         result = M * det(0, 0);
         return result;
      }
};


template <typename ArrayType>
struct mass<ArrayType, simplex_topology_tag, 2>
{
   ArrayType M;

   mass() : M(3, 4)
      {
         for(int i=0; i<3; ++i)
            for(int j=0; j<3; ++j)
               if (i == j) 
                  M(i, j) = 1./12.;
               else
                  M(i, j) = 1./24.;
      }

   ArrayType operator()(ArrayType & det)
      {
         ArrayType result(3, 4);
         result = M * det(0, 0);
         return result;
      }
};

template <typename ArrayType>
struct mass<ArrayType, cuboid_topology_tag, 2>
{
   ArrayType M;

   mass() : M(4, 5)
      {
         M(0, 0) =  4.0/36.0;
         M(0, 1) =  2.0/36.0;
         M(0, 2) =  2.0/36.0;
         M(0, 3) =  1.0/36.0;
         M(1, 0) =  2.0/36.0;
         M(1, 1) =  4.0/36.0;
         M(1, 2) =  1.0/36.0;
         M(1, 3) =  2.0/36.0;
         M(2, 0) =  2.0/36.0;
         M(2, 1) =  1.0/36.0;
         M(2, 2) =  4.0/36.0;
         M(2, 3) =  2.0/36.0;
         M(3, 0) =  1.0/36.0;
         M(3, 1) =  2.0/36.0;
         M(2, 2) =  2.0/36.0;
         M(3, 3) =  4.0/36.0;
      }

   ArrayType operator()(ArrayType & g)
      {
         ArrayType result(3, 4);
         result = M * g(0, 0);
         return result;
      }
};



template <typename ArrayType>
struct mass<ArrayType, cuboid_topology_tag, 3>
{
   ArrayType M;

   mass() : M(8, 9)
      {
         M(0, 0) =  8.0/216.0;
         M(0, 1) =  4.0/216.0;
         M(0, 2) =  4.0/216.0;
         M(0, 3) =  2.0/216.0;
         M(0, 4) =  4.0/216.0;
         M(0, 5) =  2.0/216.0;
         M(0, 6) =  2.0/216.0;
         M(0, 7) =  1.0/216.0;

         M(1, 0) =  4.0/216.0;
         M(1, 1) =  8.0/216.0;
         M(1, 2) =  2.0/216.0;
         M(1, 3) =  4.0/216.0;
         M(1, 4) =  2.0/216.0;
         M(1, 5) =  4.0/216.0;
         M(1, 6) =  1.0/216.0;
         M(1, 7) =  2.0/216.0;

         M(2, 0) =  4.0/216.0;
         M(2, 1) =  2.0/216.0;
         M(2, 2) =  8.0/216.0;
         M(2, 3) =  4.0/216.0;
         M(2, 4) =  2.0/216.0;
         M(2, 5) =  1.0/216.0;
         M(2, 6) =  4.0/216.0;
         M(2, 7) =  2.0/216.0;

         M(3, 0) =  2.0/216.0;
         M(3, 1) =  4.0/216.0;
         M(3, 2) =  4.0/216.0;
         M(3, 3) =  8.0/216.0;
         M(3, 4) =  1.0/216.0;
         M(3, 5) =  2.0/216.0;
         M(3, 6) =  2.0/216.0;
         M(3, 7) =  4.0/216.0;

         M(4, 0) =  4.0/216.0;
         M(4, 1) =  2.0/216.0;
         M(4, 2) =  2.0/216.0;
         M(4, 3) =  1.0/216.0;
         M(4, 4) =  8.0/216.0;
         M(4, 5) =  4.0/216.0;
         M(4, 6) =  4.0/216.0;
         M(4, 7) =  2.0/216.0;

         M(5, 0) =  2.0/216.0;
         M(5, 1) =  4.0/216.0;
         M(5, 2) =  1.0/216.0;
         M(5, 3) =  2.0/216.0;
         M(5, 4) =  4.0/216.0;
         M(5, 5) =  8.0/216.0;
         M(5, 6) =  2.0/216.0;
         M(5, 7) =  4.0/216.0;

         M(6, 0) =  2.0/216.0;
         M(6, 1) =  1.0/216.0;
         M(6, 2) =  4.0/216.0;
         M(6, 3) =  2.0/216.0;
         M(6, 4) =  4.0/216.0;
         M(6, 5) =  2.0/216.0;
         M(6, 6) =  8.0/216.0;
         M(6, 7) =  4.0/216.0;

         M(7, 0) =  1.0/216.0;
         M(7, 1) =  2.0/216.0;
         M(7, 2) =  2.0/216.0;
         M(7, 3) =  4.0/216.0;
         M(7, 4) =  2.0/216.0;
         M(7, 5) =  4.0/216.0;
         M(7, 6) =  4.0/216.0;
         M(7, 7) =  8.0/216.0;
      }

   ArrayType operator()(ArrayType & g)
      {
         double J = g(0, 0);

         ArrayType result(8, 9);

         if (!is_symmetric<8>(M)) 
         {
            std::cout << "M" << std::endl;
            std::cout << M << std::endl;
         }

         result = M * J;
         
         //std::cout << result << std::endl;

         return result;
      }
};


template <typename ArrayType>
struct mass<ArrayType, cuboid_topology_tag, 1>
{
   ArrayType M;

   mass() : M(2, 3)
      {
         M(0, 0) =  2.0/6.0;
         M(0, 1) =  1.0/6.0;
         M(1, 0) =  1.0/6.0;
         M(1, 1) =  2.0/6.0;
         M(0, 2) =  0.0/6.0;
         M(1, 2) =  0.0/6.0;
      }

   ArrayType operator()(ArrayType & g)
      {
         double J = g(0, 0);
         ArrayType result(2, 3);
         result = M * J;
         
         //std::cout << result << std::endl;

         return result;
      }
};



}

#endif
