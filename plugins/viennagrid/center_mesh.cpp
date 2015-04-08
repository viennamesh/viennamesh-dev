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

#include "center_mesh.hpp"

#include <set>
#include "viennagridpp/algorithm/centroid.hpp"

namespace viennamesh
{
  center_mesh::center_mesh() {}
  std::string center_mesh::name() { return "center_mesh"; }

  bool center_mesh::run(viennamesh::algorithm_handle &)
  {
    mesh_handle input_mesh = get_required_input<mesh_handle>("mesh");
    mesh_handle output_mesh = make_data<mesh_handle>();

    typedef viennagrid::mesh_t MeshType;
    typedef viennagrid::result_of::point<MeshType>::type PointType;

    typedef viennagrid::result_of::const_vertex_range<MeshType>::type ConstVertexRangeType;
    typedef viennagrid::result_of::iterator<ConstVertexRangeType>::type ConstVertexRangeIterator;


    viennagrid::copy( input_mesh(), output_mesh() );
    PointType mesh_centroid = viennagrid::centroid( output_mesh() );

    ConstVertexRangeType vertices( output_mesh() );
    for (ConstVertexRangeIterator vit = vertices.begin(); vit != vertices.end(); ++vit)
      viennagrid::set_point( *vit, viennagrid::get_point(*vit) - mesh_centroid );


    set_output( "mesh", output_mesh );
    return true;
  }

}
