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
#include "viennagrid/core/ntree.hpp"
#include "viennagrid/algorithm/geometry.hpp"

namespace viennamesh
{
  merge_close_points::merge_close_points() {}
  std::string merge_close_points::name() { return "merge_close_points"; }

  bool merge_close_points::run(viennamesh::algorithm_handle &)
  {
    mesh_handle input_mesh = get_required_input<mesh_handle>("mesh");
    mesh_handle output_mesh = make_data<mesh_handle>();

    double merge_distance = get_required_input<double>("merge_distance")();

    typedef viennagrid::mesh                                                MeshType;
    typedef viennagrid::result_of::element<MeshType>::type                  ElementType;
    typedef viennagrid::result_of::point<MeshType>::type                    PointType;

    typedef viennagrid::result_of::const_element_range<MeshType>::type      ConstElementRangeType;
    typedef viennagrid::result_of::iterator<ConstElementRangeType>::type    ConstElementIteratorType;

    ConstElementRangeType vertices( input_mesh(), 0 );
    info(1) << "Old vertex count = " << vertices.size() << std::endl;


//     OLD IMPLEMENTATION
//
//     std::vector<ElementType> new_vertices;
//     for (ConstElementIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit)
//     {
//       PointType p = viennagrid::get_point(*vit);
//
//       std::vector<ElementType>::iterator nvit = new_vertices.begin();
//       for (; nvit != new_vertices.end(); ++nvit)
//       {
//         if (viennagrid::norm_2(p - viennagrid::get_point(*nvit)) < merge_distance )
//         {
//           new_vertices.push_back(*nvit);
//           break;
//         }
//       }
//
//       if (nvit == new_vertices.end())
//         new_vertices.push_back( viennagrid::make_vertex(output_mesh(), p) );
//     }


    // build up ntree
    std::pair<PointType, PointType> bb = viennagrid::bounding_box( input_mesh() );
    PointType center = (bb.first + bb.second)/2;
    PointType size = bb.second - bb.first;

    typedef viennagrid::vertex_with_distance_wrapper<ElementType> WrapperType;
    typedef viennagrid::ntree_node<WrapperType> NodeType;
    boost::shared_ptr<NodeType> root( new NodeType( center-size/2*1.1 , center+size/2*1.1 ) );

    int element_per_node = 10;
    for (ConstElementIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit)
      root->add( WrapperType(*vit, merge_distance),
                 element_per_node,
                 vertices.size()/element_per_node);

    // create new vertices
    viennagrid::vector<ElementType> new_vertices( vertices.size() );
    for (ConstElementIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit)
    {
      PointType p = viennagrid::get_point(*vit);

      viennagrid::element_id new_vertex_id = (*vit).id();
      NodeType * node = root->get(p);
      for (std::size_t i = 0; i != node->elements().size(); ++i)
      {
        if (viennagrid::norm_2(p - viennagrid::get_point(node->elements()[i]())) < merge_distance )
        {
          if ( node->elements()[i]().id() < new_vertex_id )
          {
            new_vertex_id = node->elements()[i]().id();
            break;
          }
        }
      }

      if ( new_vertex_id == (*vit).id() )
        new_vertices[(*vit).id()] = viennagrid::make_vertex(output_mesh(), p);
      else
        new_vertices[(*vit).id()] = new_vertices[new_vertex_id];
    }

    // create cell for new mesh using merged vertices
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

    info(1) << "New vertex count = " << viennagrid::vertices(output_mesh()).size() << std::endl;
    set_output( "mesh", output_mesh );

    return true;
  }

}
