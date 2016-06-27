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
#include "viennagrid/algorithm/centroid.hpp"

namespace viennamesh
{
  center_mesh::center_mesh() {}
  std::string center_mesh::name() { return "center_mesh"; }

  bool center_mesh::run(viennamesh::algorithm_handle &)
  {

    mesh_handle input_mesh = get_input<mesh_handle>("mesh");
    if (input_mesh.valid())
    {
      mesh_handle output_mesh = make_data<mesh_handle>();

      typedef viennagrid::mesh                                                MeshType;
      typedef viennagrid::result_of::point<MeshType>::type                    PointType;

      typedef viennagrid::result_of::const_vertex_range<MeshType>::type       ConstVertexRangeType;
      typedef viennagrid::result_of::iterator<ConstVertexRangeType>::type     ConstVertexRangeIterator;


      viennagrid::copy( input_mesh(), output_mesh() );
      PointType mesh_centroid = viennagrid::centroid( output_mesh() );

      info(1) << "Mesh centroid " << mesh_centroid << std::endl;

      ConstVertexRangeType vertices( output_mesh() );
      for (ConstVertexRangeIterator vit = vertices.begin(); vit != vertices.end(); ++vit)
        viennagrid::set_point( *vit, viennagrid::get_point(*vit) - mesh_centroid );


      set_output( "mesh", output_mesh );
    }


    data_handle<viennagrid_plc> input_geometry = get_input<viennagrid_plc>("geometry");
    if (input_geometry.valid())
    {
      data_handle<viennagrid_plc> output_geometry = make_data<viennagrid_plc>();

      viennagrid_dimension geometric_dimension;
      viennagrid_plc_geometric_dimension_get( input_geometry(), &geometric_dimension );

      viennagrid_numeric * src_coords;
      viennagrid_plc_vertex_coords_pointer( input_geometry(), &src_coords );

      viennagrid_int vertex_count;
      viennagrid_plc_element_count( input_geometry(), 0, &vertex_count );

      std::vector<viennagrid_numeric> center( geometric_dimension, 0 );

      for (viennagrid_int i = 0; i != vertex_count; ++i)
      {
        for (viennagrid_dimension d = 0; d != geometric_dimension; ++d)
          center[d] += src_coords[ geometric_dimension*i + d ];
      }

      for (viennagrid_dimension d = 0; d != geometric_dimension; ++d)
        center[d] /= vertex_count;


      viennagrid_plc_copy( input_geometry(), output_geometry() );

      viennagrid_numeric * dst_coords;
      viennagrid_plc_vertex_coords_pointer( output_geometry(), &dst_coords );

      for (viennagrid_int i = 0; i != vertex_count; ++i)
      {
        for (viennagrid_dimension d = 0; d != geometric_dimension; ++d)
          dst_coords[ geometric_dimension*i + d ] -= center[d];
      }


      set_output( "geometry", output_geometry );
    }

    return true;
  }

}
