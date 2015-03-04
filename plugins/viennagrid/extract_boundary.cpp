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

#include "extract_boundary.hpp"

#include "viennagrid/algorithm/extract_hole_points.hpp"
#include "viennagrid/algorithm/extract_boundary.hpp"
#include "viennagrid/algorithm/extract_seed_points.hpp"



namespace viennamesh
{

  extract_boundary::extract_boundary() {}
  std::string extract_boundary::name() { return "extract_boundary"; }

  bool extract_boundary::run(viennamesh::algorithm_handle &)
  {
    mesh_handle input_mesh = get_required_input<mesh_handle>("mesh");
//     int geometric_dimension = viennagrid::geometric_dimension( input_mesh() );

    mesh_handle output_mesh = make_data<mesh_handle>();

    PointContainerType hole_points;
    SeedPointContainerType seed_points;

    viennagrid::extract_boundary( input_mesh(), output_mesh() );
    viennagrid::extract_seed_points( input_mesh(), seed_points );
    viennagrid::extract_hole_points( input_mesh(), hole_points );

    set_output( "mesh", output_mesh );

    if (!hole_points.empty())
    {
      info(1) << "Extracted " << hole_points.size() << " hole points" << std::endl;
      for (PointContainerType::const_iterator it = hole_points.begin(); it != hole_points.end(); ++it)
        info(1) << "   " << *it << std::endl;

      point_container_handle output_hole_points = make_data<point_container_handle>();
      convert(hole_points, output_hole_points());
      set_output( "hole_points", output_hole_points );
    }

    if (!seed_points.empty())
    {
      info(1) << "Extracted " << seed_points.size() << " seed points" << std::endl;
      for (SeedPointContainerType::const_iterator it = seed_points.begin(); it != seed_points.end(); ++it)
        info(1) << "   " << (*it).first << " -> " << (*it).second << std::endl;


      seed_point_container_handle output_seed_points = make_data<seed_point_container_handle>();
      convert(seed_points, output_seed_points());
      set_output( "seed_points", output_seed_points );
    }

    return true;
  }

}
