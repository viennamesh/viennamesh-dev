

#include <iostream>

#include "viennautils/dumptype.hpp"

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


