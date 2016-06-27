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

#include "plc_reader.hpp"
#include "viennagrid/viennagrid.h"

namespace viennamesh
{
  plc_reader::plc_reader() {}
  std::string plc_reader::name() { return "plc_reader"; }

  bool plc_reader::run(viennamesh::algorithm_handle &)
  {
    string_handle filename = get_required_input<string_handle>("filename");


    data_handle<viennagrid_plc> output_geometry = make_data<viennagrid_plc>();
    viennagrid_error error = viennagrid_plc_read_tetgen_poly( output_geometry(), filename().c_str() );




    if (error == VIENNAGRID_SUCCESS)
    {
      info(1) << "Tetgen PLC successfully read" << std::endl;

      set_output("geometry", output_geometry);



      typedef viennagrid::point PointType;
      viennagrid_dimension geometric_dimension;
      viennagrid_plc_geometric_dimension_get( output_geometry(), &geometric_dimension );

      point_container hole_points;
      viennagrid_numeric * hole_point_coords;
      viennagrid_int hole_point_count;
      viennagrid_plc_volumetric_hole_points_get( output_geometry(), &hole_point_count, &hole_point_coords);

      if (hole_point_count != 0)
      {
        for (viennagrid_int i = 0; i != hole_point_count; ++i)
        {
          PointType tmp(geometric_dimension);
          std::copy( hole_point_coords + i*geometric_dimension, hole_point_coords + (i+1)*geometric_dimension, tmp.begin() );
          hole_points.push_back(tmp);
        }

        point_handle output_hole_points = make_data<point>();
        output_hole_points.set( hole_points );
        set_output("hole_points", output_hole_points);
      }


      seed_point_container seed_points;
      viennagrid_numeric * seed_point_coords;
      viennagrid_int * seed_point_region_ids;
      viennagrid_int seed_point_count;
      viennagrid_plc_seed_points_get( output_geometry(), &seed_point_count, &seed_point_coords, &seed_point_region_ids );

      if (seed_point_count != 0)
      {
        for (viennagrid_int i = 0; i != seed_point_count; ++i)
        {
          PointType tmp(geometric_dimension);
          std::copy( seed_point_coords + i*geometric_dimension, seed_point_coords + (i+1)*geometric_dimension, tmp.begin() );
          seed_points.push_back( std::make_pair(tmp, seed_point_region_ids[i]) );
        }

        seed_point_handle output_seed_points = make_data<seed_point>();
        output_seed_points.set( seed_points );
        set_output("seed_points", output_seed_points);
      }
    }
    else
    {
      VIENNAMESH_ERROR(VIENNAMESH_ERROR_ALGORITHM_RUN_FAILED, "Error reading Tetgen PLC: " + boost::lexical_cast<std::string>(error) );
    }

    return true;
  }

}
