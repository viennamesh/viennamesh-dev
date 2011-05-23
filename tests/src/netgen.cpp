/* =============================================================================
   Copyright (c) 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                 ViennaMesh - The Vienna Mesh Generation Framework
                             -----------------

   authors:    Josef Weinbub                         weinbub@iue.tuwien.ac.at


   license:    see file LICENSE in the base directory
============================================================================= */

#include <iostream>


#include "viennautils/io/bnd.hpp"
#include "viennautils/io/hin.hpp"
#ifdef VIENNAMESH_HAVE_GTSIO
#include "viennautils/io/gts.hpp"
#endif
#include "viennautils/dumptype.hpp"
#include "viennautils/file.hpp"

#include "viennagrid/io/vtk_writer.hpp"
#include "viennagrid/domain.hpp"

#include "viennamesh/wrapper/hin.hpp"
#include "viennamesh/generation/cervpt.hpp"
#include "viennamesh/generation/netgen.hpp"
#include "viennamesh/adaptation/orienter.hpp"

int main(int argc, char * argv[])
{
   std::string inputfile("../../input/simple_boxes.hin");
   std::string outputfile("output_netgen.vtk");
   
   viennautils::io::hin_reader my_hin_reader;
   my_hin_reader(inputfile);

   typedef viennamesh::wrapper<viennamesh::tag::hin, viennautils::io::hin_reader>      hin_wrapper_type;
   hin_wrapper_type                    wrapped_data(my_hin_reader);      

   typedef viennamesh::result_of::mesh_generator<viennamesh::tag::cervpt>::type        cervpt_hull_mesh_generator_type;
   cervpt_hull_mesh_generator_type     hull_mesher;       
   
   typedef viennamesh::result_of::mesh_adaptor<viennamesh::tag::orienter>::type        orienter_adaptor_type;
   orienter_adaptor_type               orienter;   
   
   typedef viennamesh::result_of::mesh_generator<viennamesh::tag::netgen>::type        netgen_volume_mesh_generator_type;
   netgen_volume_mesh_generator_type       volume_mesher;         
   
   typedef netgen_volume_mesh_generator_type::result_type       volume_result_type;
   //volume_result_type volume_mesh = volume_mesher(orienter(hull_mesher(wrapped_data)));
   
   return 0;
}



