/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha          philipp@gsse.at
     Copyright (c) 2005-2006 Michael Spevak

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#ifndef GSSE_FEM_TRAITS_2006
#define GSSE_FEM_TRAITS__2006

#include "gsse/math/power.hpp"
#include "gsse/topology.hpp"
#include "stiffness.hpp"
#include "mass.hpp"
//#include "mechanics.hpp"
#include "fem_transformation.hpp"

namespace gsse
{

   template <typename Archetype, unsigned int dimension>
   struct fem_basic {};

   template <typename FEMType>
   struct fem_traits { };
   

   template <unsigned int dimension>
   struct fem_traits<fem_basic<simplex_topology_tag, dimension> >
   {
      template <typename ArrayType>
      struct g_matrix
      {
         typedef gsse::g_matrix<ArrayType, simplex_topology_tag, dimension> type;
      };

      template <typename ArrayType>
      struct transform
      {
         typedef gsse::transform<ArrayType, simplex_topology_tag, dimension> type;
      };

      template <typename ArrayType>
      struct backtransform
      {
         typedef gsse::backtransform<ArrayType, simplex_topology_tag, dimension> type;
      };


      template <typename ArrayType>
      struct stiffness
      {
         typedef gsse::stiffness<ArrayType, simplex_topology_tag, dimension> type;
      };

      template <typename ArrayType>
      struct mass
      {
         typedef gsse::mass<ArrayType, simplex_topology_tag, dimension> type;
      };

      static const int points      = dimension + 1;
      static const int coordinates = dimension;
      static const int entries     = dimension + 1;
   };


   template <unsigned int dimension>
   struct fem_traits<fem_basic<cuboid_topology_tag, dimension> >
   {
      template <typename ArrayType>
      struct g_matrix
      {
         typedef gsse::g_matrix<ArrayType, cuboid_topology_tag, dimension> type;
      };

      template <typename ArrayType>
      struct mass
      {
         typedef gsse::stiffness<ArrayType, cuboid_topology_tag, dimension> type;
      };

      template <typename ArrayType>
      struct stiffness
      {
         typedef gsse::stiffness<ArrayType, cuboid_topology_tag, dimension> type;
      };

      template <typename ArrayType>
      struct transform
      {
         typedef gsse::transform<ArrayType, cuboid_topology_tag, dimension> type;
      };

      template <typename ArrayType>
      struct backtransform
      {
         typedef gsse::backtransform<ArrayType, cuboid_topology_tag, dimension> type;
      };

      static const int points      = power<2, dimension>::value;
      static const int coordinates = dimension;
      static const int entries     = power<2, dimension>::value;
   };

}


#endif
