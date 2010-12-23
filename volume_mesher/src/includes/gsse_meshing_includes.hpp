/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **
     mesh generation and adapation: 2D/3D

     Copyright (c) 2003-2008 Rene Heinzl                     rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha              philipp@gsse.at
     Copyright (c) 2007-2008 Franz Stimpfl                  franz@gsse.at

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#ifndef GSSE_DOMAIN_MESHERHH_ID
#define GSSE_DOMAIN_MESHERHH_ID


// other includes
//
#include<boost/lexical_cast.hpp>


// *** system includes
//
#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <unistd.h>

// *** GSSE includes
//
#include "gsse/domain.hpp"
#include "gsse/topology/boundary_iterator.hpp"

namespace gsse
{
   typedef gsse::generic_point<double,3>                                         vector_t;
   typedef gsse::generic_point<double,3>                                         point_t;
   typedef gsse::detail_topology::unstructured<2>                                unstructured_topology_32t; 
   typedef gsse::get_domain<unstructured_topology_32t, double, double,3>::type   domain_32t;

   typedef gsse::detail_topology::unstructured<3>                                unstructured_topology_3t; 
   typedef gsse::get_domain<unstructured_topology_3t, double, double,3>::type    domain_3t;

   
   struct gsse_triangle
   {
      long      material;
      long      pindexarray[3];
      long      offset;
      long      mat_index[2];
      
      gsse_triangle(long offset=0):offset(offset){}
      long operator[](long index) {return pindexarray[index];}
   };

}
// ===========================

// *** gsse surface algorithms
//
#include "gsse_surface.hpp"


#endif
