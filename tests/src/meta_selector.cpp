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
#include <iostream>

// *** viennamesh includes
#include "viennamesh/common.hpp"
#include "viennamesh/meta_selection.hpp"

int main(int argc, char *argv[])
{
   //
   // we are looking here for an advancing front mesher which supports 
   // _simplex_ based mesh elements --> such a mesh generator _is_ available
   //
   typedef boost::fusion::result_of::make_map<
      viennamesh::tag::cell_type, viennamesh::tag::algorithm,      
      viennamesh::tag::simplex,   viennamesh::tag::advancing_front
   >::type                                                                             working_properties;

   typedef viennamesh::result_of::compute_mesh_generator<working_properties>::type     working_result;
   viennautils::dumptype<working_result>();
   


   return 0;
}


