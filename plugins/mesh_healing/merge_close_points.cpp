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

#include "merge_close_points.hpp"

namespace viennamesh
{
  merge_close_points::merge_close_points() {}
  std::string merge_close_points::name() { return "merge_close_points"; }

  bool merge_close_points::run(viennamesh::algorithm_handle &)
  {
    mesh_handle input_mesh = get_required_input<mesh_handle>("mesh");
    mesh_handle output_mesh = make_data<mesh_handle>();

    double merge_distance = get_required_input<double>("merge_distance")();

    typedef viennagrid::mesh_t MeshType;
    typedef viennagrid::result_of::element<MeshType>::type ElementType;
    typedef viennagrid::result_of::point<MeshType>::type PointType;

    typedef viennagrid::result_of::const_element_range<MeshType>::type ConstElementRangeType;
    typedef viennagrid::result_of::iterator<ConstElementRangeType>::type ConstElementIteratorType;

    std::vector<ElementType> new_vertices;
    ConstElementRangeType vertices( input_mesh(), 0 );
    for (ConstElementIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit)
    {
      PointType p = viennagrid::get_point(*vit);

      std::vector<ElementType>::iterator nvit = new_vertices.begin();
      for (; nvit != new_vertices.end(); ++nvit)
      {
        if (viennagrid::norm_2(p - viennagrid::get_point(*nvit)) < merge_distance )
        {
          new_vertices.push_back(*nvit);
          break;
        }
      }

      if (nvit == new_vertices.end())
        new_vertices.push_back( viennagrid::make_vertex(output_mesh(), p) );
    }


    ConstElementRangeType cells( input_mesh(), viennagrid::cell_dimension(input_mesh()) );
    for (ConstElementIteratorType cit = cells.begin(); cit != cells.end(); ++cit)
    {
      std::vector<ElementType> local_vertices;

      typedef viennagrid::result_of::const_element_range<ElementType>::type ConstCoundaryRangeType;
      typedef viennagrid::result_of::iterator<ConstCoundaryRangeType>::type ConstCoundaryIteratorType;

      ConstCoundaryRangeType boundary_vertices(*cit, 0);
      for (ConstCoundaryIteratorType bvit = boundary_vertices.begin(); bvit != boundary_vertices.end(); ++bvit)
        local_vertices.push_back( new_vertices[(*bvit).id()] );

      viennagrid::make_element( output_mesh(), (*cit).tag(), local_vertices.begin(), local_vertices.end() );
    }

    set_output( "mesh", output_mesh );

    return true;
  }

}
