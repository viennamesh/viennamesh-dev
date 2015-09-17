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

#include "make_boundary_box.hpp"
#include "viennagrid/algorithm/geometry.hpp"

namespace viennamesh
{
  make_boundary_box::make_boundary_box() {}
  std::string make_boundary_box::name() { return "make_boundary_box"; }

  bool make_boundary_box::run(viennamesh::algorithm_handle &)
  {
    data_handle<double> border_x = get_input<double>("border_x");
    data_handle<double> border_y = get_input<double>("border_y");
    data_handle<double> border_z = get_input<double>("border_z");


    mesh_handle input_mesh = get_required_input<mesh_handle>("mesh");

    std::pair<point, point> bounding_box = viennagrid::bounding_box( input_mesh() );
    point ll = bounding_box.first;
    point ur = bounding_box.second;

    if (border_x.valid())
    {
      ll[0] -= border_x();
      ur[0] += border_x();
    }

    if (border_y.valid())
    {
      ll[1] -= border_y();
      ur[1] += border_y();
    }

    if (border_z.valid())
    {
      ll[2] -= border_z();
      ur[2] += border_z();
    }


    info(1) << "LL = " << ll << std::endl;
    info(1) << "UR = " << ur << std::endl;


    typedef viennagrid::mesh                                  MeshType;
    typedef viennagrid::result_of::point<MeshType>::type      PointType;
    typedef viennagrid::result_of::element<MeshType>::type    VertexType;

    mesh_handle output_mesh = make_data<mesh_handle>();


    PointType points[8];
    points[0] = viennagrid::make_point( ll[0], ll[1], ll[2] );
    points[1] = viennagrid::make_point( ll[0], ur[1], ll[2] );
    points[2] = viennagrid::make_point( ur[0], ll[1], ll[2] );
    points[3] = viennagrid::make_point( ur[0], ur[1], ll[2] );

    points[4] = viennagrid::make_point( ll[0], ll[1], ur[2] );
    points[5] = viennagrid::make_point( ll[0], ur[1], ur[2] );
    points[6] = viennagrid::make_point( ur[0], ll[1], ur[2] );
    points[7] = viennagrid::make_point( ur[0], ur[1], ur[2] );


    VertexType vertices[8];

    for (int i = 0; i != 8; ++i)
      vertices[i] = viennagrid::make_vertex(output_mesh(), points[i]);

    viennagrid::make_triangle(output_mesh(), vertices[0], vertices[2], vertices[3]);
    viennagrid::make_triangle(output_mesh(), vertices[0], vertices[3], vertices[1]);

    viennagrid::make_triangle(output_mesh(), vertices[4], vertices[6], vertices[7]);
    viennagrid::make_triangle(output_mesh(), vertices[4], vertices[7], vertices[5]);

    viennagrid::make_triangle(output_mesh(), vertices[0], vertices[2], vertices[6]);
    viennagrid::make_triangle(output_mesh(), vertices[0], vertices[6], vertices[4]);

    viennagrid::make_triangle(output_mesh(), vertices[1], vertices[5], vertices[7]);
    viennagrid::make_triangle(output_mesh(), vertices[1], vertices[7], vertices[3]);

    viennagrid::make_triangle(output_mesh(), vertices[0], vertices[4], vertices[5]);
    viennagrid::make_triangle(output_mesh(), vertices[0], vertices[5], vertices[1]);

    viennagrid::make_triangle(output_mesh(), vertices[2], vertices[3], vertices[7]);
    viennagrid::make_triangle(output_mesh(), vertices[2], vertices[7], vertices[6]);


    set_output( "mesh", output_mesh );

    return true;
  }

}
