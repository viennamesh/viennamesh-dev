/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha          philipp@gsse.at
     Copyright (c) 2005-2006 Michael Spevak

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#ifndef GSSE_FEM_TRANSFORMATION_2006
#define GSSE_FEM_TRANSFORMATION_2006

#include "gsse/topology.hpp"
#include "gsse/math/determinant.hpp"

namespace gsse
{

template <typename ArrayType, typename Archetype, int dimension> struct g_matrix{};
template <typename ArrayType, typename Archetype, int dimension> struct transform{};

template <typename ArrayType>
struct transform<ArrayType, simplex_topology_tag, 3>
{
   transform() {}

   ArrayType operator()(ArrayType & points) const
      {
         ArrayType result(3, 3);

         result(0, 0) = points(1, 0) - points(0, 0);
         result(0, 1) = points(1, 1) - points(0, 1);
         result(0, 2) = points(1, 2) - points(0, 2);
         result(1, 0) = points(2, 0) - points(0, 0);
         result(1, 1) = points(2, 1) - points(0, 1);
         result(1, 2) = points(2, 2) - points(0, 2);
         result(2, 0) = points(3, 0) - points(0, 0);
         result(2, 1) = points(3, 1) - points(0, 1);
         result(2, 2) = points(3, 2) - points(0, 2);
         
         return result;
      }
};

template <typename ArrayType>
struct transform<ArrayType, cuboid_topology_tag, 3>
{
   transform() {}

   ArrayType operator()(ArrayType & points) const
      {
         ArrayType result(3, 3);

         result(0, 0) = points(1, 0) - points(0, 0);
         result(0, 1) = points(1, 1) - points(0, 1);
         result(0, 2) = points(1, 2) - points(0, 2);

         result(1, 0) = points(2, 0) - points(0, 0);
         result(1, 1) = points(2, 1) - points(0, 1);
         result(1, 2) = points(2, 2) - points(0, 2);

         result(2, 0) = points(4, 0) - points(0, 0);
         result(2, 1) = points(4, 1) - points(0, 1);
         result(2, 2) = points(4, 2) - points(0, 2);         

         return result;
      }
};

template <typename ArrayType, typename TopologyTag>
struct transform<ArrayType, TopologyTag, 2>
{
   transform() {}

   ArrayType operator()(ArrayType & points) const
      {
         ArrayType result(2, 2);

         result(0, 0) = points(1, 0) - points(0, 0);
         result(0, 1) = points(1, 1) - points(0, 1);
         result(1, 0) = points(2, 0) - points(0, 0);
         result(1, 1) = points(2, 1) - points(0, 1);

         return result;
      }
};


template <typename ArrayType>
struct transform<ArrayType, cuboid_topology_tag, 1>
{
   transform() {}

   ArrayType operator()(ArrayType & points) const
      {
         ArrayType result(1, 1);

         result(0, 0) = points(1, 0) - points(0, 0);

         return result;
      }
};

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////


template <typename ArrayType, typename TopologyTag, int dimension>
struct backtransform
{
   ArrayType operator()(ArrayType & points) const
      {
         ArrayType mat(dimension, dimension);
         ArrayType result(dimension, dimension);

         mat = transform<ArrayType, TopologyTag, dimension>()(points);
         result = inverse<dimension> (mat);
         
         return result;
      }
};


////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////


template <typename ArrayType>
struct g_matrix<ArrayType, simplex_topology_tag, 3>
{
   g_matrix() {}

   ArrayType det(ArrayType & array)
      {
         double J11 = array(1, 0)  - array(0, 0);
         double J12 = array(1, 1)  - array(0, 1);
         double J13 = array(1, 2)  - array(0, 2);
         
         double J21 = array(2, 0)  - array(0, 0);
         double J22 = array(2, 1)  - array(0, 1);
         double J23 = array(2, 2)  - array(0, 2);
         
         double J31 = array(3, 0)  - array(0, 0);
         double J32 = array(3, 1)  - array(0, 1);
         double J33 = array(3, 2)  - array(0, 2);

         double detJ = J11 * J22 * J33 + J21 * J32 * J13 + J12 * J23 * J31 
            - J11 * J23 * J32 - J13 * J22 * J31 - J12 * J21 * J33;

         ArrayType det(1, 1);
         det(0, 0) = detJ;

         return det;
      }

   
   ArrayType operator()(ArrayType & array)
      {
         double J11 = array(1, 0)  - array(0, 0);
         double J12 = array(1, 1)  - array(0, 1);
         double J13 = array(1, 2)  - array(0, 2);
         
         double J21 = array(2, 0)  - array(0, 0);
         double J22 = array(2, 1)  - array(0, 1);
         double J23 = array(2, 2)  - array(0, 2);
         
         double J31 = array(3, 0)  - array(0, 0);
         double J32 = array(3, 1)  - array(0, 1);
         double J33 = array(3, 2)  - array(0, 2);
         
         double K11 = J22 * J33 - J23 * J32;
         double K21 = J23 * J31 - J21 * J33;
         double K31 = J21 * J32 - J22 * J31;
         double K12 = J13 * J32 - J12 * J33;
         double K22 = J11 * J33 - J13 * J31;
         double K32 = J12 * J31 - J11 * J32;
         double K13 = J12 * J23 - J13 * J22;
         double K23 = J13 * J21 - J11 * J23;
         double K33 = J11 * J22 - J12 * J21;
                        
         double detJ = J11 * J22 * J33 + J21 * J32 * J13 + J12 * J23 * J31 
            - J11 * J23 * J32 - J13 * J22 * J31 - J12 * J21 * J33;
         
         detJ = fabs(detJ);       
                  
         double ga = (K11 * K11 + K21 * K21 + K31 * K31) / detJ;
         double gb = (K12 * K12 + K22 * K22 + K32 * K32) / detJ;
         double gc = (K13 * K13 + K23 * K23 + K33 * K33) / detJ;
         
         double gd = (K11 * K12 + K21 * K22 + K31 * K32) / detJ;
         double ge = (K12 * K13 + K22 * K23 + K32 * K33) / detJ;
         double gf = (K13 * K11 + K23 * K21 + K33 * K31) / detJ;
         
         ArrayType result(3, 3);
         
         result(0, 0) = ga;
         result(1, 1) = gb;
         result(2, 2) = gc;
         
         result(1, 0) = gd;
         result(0, 1) = gd;
         result(1, 2) = ge;
         result(2, 1) = ge;
         result(0, 2) = gf;
         result(2, 0) = gf;
   
         return result;
      }
};


template <typename ArrayType>
struct g_matrix<ArrayType, cuboid_topology_tag, 3>
{
   g_matrix() 
      { 
         //std::cout << "INIT 3D Cuboid G_Matrix" << std::endl;                 
      }

   ArrayType det(ArrayType & array)
      {
         double J11 = array(1, 0)  - array(0, 0);
         double J12 = array(1, 1)  - array(0, 1);
         double J13 = array(1, 2)  - array(0, 2);
         
         double J21 = array(2, 0)  - array(0, 0);
         double J22 = array(2, 1)  - array(0, 1);
         double J23 = array(2, 2)  - array(0, 2);
         
         double J31 = array(4, 0)  - array(0, 0);
         double J32 = array(4, 1)  - array(0, 1);
         double J33 = array(4, 2)  - array(0, 2);

         double detJ = J11 * J22 * J33 + J21 * J32 * J13 + J12 * J23 * J31 
            - J11 * J23 * J32 - J13 * J22 * J31 - J12 * J21 * J33;

         ArrayType det(1, 1);
         det(0, 0) = detJ;

         return det;
      }
   
   ArrayType operator()(ArrayType & array)
      {
         double J11 = array(1, 0)  - array(0, 0);
         double J12 = array(1, 1)  - array(0, 1);
         double J13 = array(1, 2)  - array(0, 2);
         
         double J21 = array(2, 0)  - array(0, 0);
         double J22 = array(2, 1)  - array(0, 1);
         double J23 = array(2, 2)  - array(0, 2);
         
         double J31 = array(4, 0)  - array(0, 0);
         double J32 = array(4, 1)  - array(0, 1);
         double J33 = array(4, 2)  - array(0, 2);

//         std::cout << J11 << "  " << J12 << "  " << J13 << std::endl;
//         std::cout << J21 << "  " << J22 << "  " << J23 << std::endl;
//         std::cout << J31 << "  " << J32 << "  " << J33 << std::endl;

         
         double K11 = J22 * J33 - J23 * J32;
         double K21 = J23 * J31 - J21 * J33;
         double K31 = J21 * J32 - J22 * J31;
         double K12 = J13 * J32 - J12 * J33;
         double K22 = J11 * J33 - J13 * J31;
         double K32 = J12 * J31 - J11 * J32;
         double K13 = J12 * J23 - J13 * J22;
         double K23 = J13 * J21 - J11 * J23;
         double K33 = J11 * J22 - J12 * J21;
                        
         double detJ = J11 * J22 * J33 + J21 * J32 * J13 + J12 * J23 * J31 
            - J11 * J23 * J32 - J13 * J22 * J31 - J12 * J21 * J33;
         
         detJ = fabs(detJ);       

                  
         double ga = (K11 * K11 + K21 * K21 + K31 * K31) / detJ;
         double gb = (K12 * K12 + K22 * K22 + K32 * K32) / detJ;
         double gc = (K13 * K13 + K23 * K23 + K33 * K33) / detJ;
         
         double gd = (K11 * K12 + K21 * K22 + K31 * K32) / detJ;
         double ge = (K12 * K13 + K22 * K23 + K32 * K33) / detJ;
         double gf = (K13 * K11 + K23 * K21 + K33 * K31) / detJ;
         
         ArrayType result(3, 3);
         
         result(0, 0) = ga;
         result(1, 1) = gb;
         result(2, 2) = gc;
         
         result(1, 0) = gd;
         result(0, 1) = gd;
         result(1, 2) = ge;
         result(2, 1) = ge;
         result(0, 2) = gf;
         result(2, 0) = gf;
   
         return result;
      }
};



template <typename ArrayType>
struct g_matrix<ArrayType, simplex_topology_tag, 2>
{
   g_matrix() 
      { 
         //std::cout << "INIT 2D Simplex G_Matrix" << std::endl;            
      }

   ArrayType det(ArrayType & array)
      {
         //std::cout << array << std::endl;

         double J11 = array(1, 0) - array(0, 0);
         double J12 = array(1, 1) - array(0, 1);
         
         double J21 = array(2, 0) - array(0, 0);
         double J22 = array(2, 1) - array(0, 1);

         //std::cout << J11 << "   " << J12 << "   " << J21 << "   " << J22 << std::endl;
                                 
         double detJ = J11 * J22 - J21 * J12;

         ArrayType det(1, 1);
         det(0, 0) = detJ;

         return det;
      }
   
   ArrayType operator()(ArrayType & array)
      {
         //std::cout << array << std::endl;

         double J11 = array(1, 0) - array(0, 0);
         double J12 = array(1, 1) - array(0, 1);
         
         double J21 = array(2, 0) - array(0, 0);
         double J22 = array(2, 1) - array(0, 1);

         //std::cout << J11 << "   " << J12 << "   " << J21 << "   " << J22 << std::endl;
                                 
         double detJ = J11 * J22 - J21 * J12;
         
         detJ = fabs(detJ);       

         //std::cout << detJ << std::endl;
         
         double ga =   (J21 * J21 + J22 * J22) / detJ;
         double gb =   (J11 * J11 + J12 * J12) / detJ;
         double gc = - (J11 * J21 + J12 * J22) / detJ;
                  
         ArrayType result(2, 2);
         
         result(0, 0) = ga;
         result(1, 1) = gb;
         result(0, 1) = gc;
         result(1, 0) = gc;

         //std::cout << result << std::endl;
   
         return result;
      }
};

template <typename ArrayType>
struct g_matrix<ArrayType, cuboid_topology_tag, 2>
{
   g_matrix() 
      { 
         //std::cout << "INIT 2D Cuboid G_Matrix" << std::endl;            
      }


   ArrayType det(ArrayType & array)
      {
         //std::cout << array << std::endl;

         double J11 = array(1, 0) - array(0, 0);
         double J12 = array(1, 1) - array(0, 1);
         
         double J21 = array(2, 0) - array(0, 0);
         double J22 = array(2, 1) - array(0, 1);

         //std::cout << J11 << "   " << J12 << "   " << J21 << "   " << J22 << std::endl;
                                 
         double detJ = J11 * J22 - J21 * J12;

         ArrayType det(1, 1);
         det(0, 0) = detJ;

         return det;
      }
      
   ArrayType operator()(ArrayType & array)
      {
         //std::cout << array << std::endl;

         double J11 = array(1, 0) - array(0, 0);
         double J12 = array(1, 1) - array(0, 1);
         
         double J21 = array(2, 0) - array(0, 0);
         double J22 = array(2, 1) - array(0, 1);

         // std::cout << J11 << "   " << J12 << "   " << J21 << "   " << J22 << std::endl;
                                 
         double detJ = J11 * J22 - J21 * J12;
         
         detJ = fabs(detJ);       

         // std::cout << detJ << std::endl;
         
         double ga =   (J21 * J21 + J22 * J22) / detJ;
         double gb =   (J11 * J11 + J12 * J12) / detJ;
         double gc = - (J11 * J21 + J12 * J22) / detJ;
                  
         ArrayType result(2, 2);
         
         result(0, 0) = ga;
         result(1, 1) = gb;
         result(0, 1) = gc;
         result(1, 0) = gc;

         // std::cout << result << std::endl;
   
         return result;
      }
};



template <typename ArrayType>
struct g_matrix<ArrayType, simplex_topology_tag, 1>
{
   g_matrix() 
      { 
         // std::cout << "INIT 1D Simplex G_Matrix" << std::endl;      
      }


   ArrayType det(ArrayType & array)
      {
         double J11 = array(1, 0) - array(0, 0);

         ArrayType det(1, 1);
         det(0, 0) = J11;

         return det;
      }
      
   ArrayType operator()(ArrayType & array)
      {
         // std::cout << array << std::endl;

         double J11 = array(1, 0) - array(0, 0);
         
         double ga =  1 / J11;
                  
         ArrayType result(1, 1);
         
         result(0, 0) = ga;

         // std::cout << result << std::endl;
   
         return result;
      }
};



template <typename ArrayType>
struct g_matrix<ArrayType, cuboid_topology_tag, 1>
{
   g_matrix() 
      { 
         // std::cout << "INIT 1D Cuboid G_Matrix" << std::endl;
      }

   ArrayType det(ArrayType & array)
      {
         double J11 = array(1, 0) - array(0, 0);

         ArrayType det(1, 1);
         det(0, 0) = J11;

         return det;
      }
   
   ArrayType operator()(ArrayType & array)
      {
         // std::cout << array << std::endl;

         double J11 = array(1, 0) - array(0, 0);
                           
         ArrayType result(1, 1);
         
         result(0, 0) = J11;

         // std::cout << result << std::endl;
   
         return result;
      }
};

}

#endif
