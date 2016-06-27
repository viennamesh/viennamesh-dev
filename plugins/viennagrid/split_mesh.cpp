/* ============================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                ViennaMesh - The Vienna Meshing Framework
                            -----------------

                    http://viennamesh.sourceforge.net/

   License:         MIT (X11), see file LICENSE in the base directory
=============================================================================== */

#include "split_mesh.hpp"

namespace viennamesh
{
  split_mesh::split_mesh() {}
  std::string split_mesh::name() { return "split_mesh"; }

  bool split_mesh::run(viennamesh::algorithm_handle &)
  {
    mesh_handle input_mesh = get_required_input<mesh_handle>("mesh");

    typedef viennagrid::mesh                                          MeshType;
    typedef viennagrid::result_of::region_range<MeshType>::type       RegionRangeType;
    typedef viennagrid::result_of::iterator<RegionRangeType>::type    RegionRangeIterator;

    RegionRangeType regions( input_mesh() );
    if (regions.size() <= 1)
    {
      mesh_handle output_mesh = make_data<mesh_handle>();
      viennagrid::copy( input_mesh(), output_mesh() );
      set_output( "mesh", output_mesh );
    }
    else
    {
      int region_index = 0;
      for (RegionRangeIterator rit = regions.begin(); rit != regions.end(); ++rit, ++region_index)
      {
        mesh_handle output_mesh = make_data<mesh_handle>();
        viennagrid::result_of::element_copy_map<>::type copy_map( output_mesh(), false );
        viennagrid::copy(copy_map, *rit, output_mesh(), viennagrid::true_functor());
        set_output( "mesh[" + lexical_cast<std::string>(region_index) + "]", output_mesh );
      }

      info(1) << "Split mesh into " << region_index << " meshes" << std::endl;
    }

    return true;
  }




//     RegionRangeType regions( input_mesh() );
//     if (regions.size() <= 1)
//     {
//       mesh_handle output_mesh = make_data<mesh_handle>();
//       viennagrid::copy( input_mesh(), output_mesh() );
//       set_output( "mesh", output_mesh );
//     }
//     else
//     {
//       mesh_handle output_meshes = make_data<mesh_handle>();
//       output_meshes.resize( regions.size() );
//
//       int region_index = 0;
//       for (RegionRangeIterator rit = regions.begin(); rit != regions.end(); ++rit, ++region_index)
//       {
//         viennagrid::mesh mesh = output_meshes(region_index);
//
//         viennagrid::result_of::element_copy_map<>::type copy_map( mesh, false );
//         viennagrid::copy(copy_map, *rit, mesh, viennagrid::true_functor());
//       }
//
//       set_output( "mesh", output_meshes );
//
//       info(1) << "Split mesh into " << region_index << " meshes" << std::endl;
//     }



}
