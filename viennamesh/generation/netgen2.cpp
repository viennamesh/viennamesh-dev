/* =============================================================================
   Copyright (c) 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                 ViennaMesh - The Vienna Mesh Generation Framework
                             -----------------

   authors:    Josef Weinbub                         weinbub@iue.tuwien.ac.at


   license:    see file LICENSE in the base directory
============================================================================= */



// *** system includes
#include <vector>
#include <map>
#include <cmath>

// *** vienna includes
#include "viennautils/dumptype.hpp"
#include "viennagrid/domain.hpp"

#include "netgen2.hpp"

namespace nglib {
#include "nglib.h"
}



namespace viennamesh {

void mesh_kernel<viennamesh::tag::netgen2>::operator()()
{
}

} // end namespace viennamesh



