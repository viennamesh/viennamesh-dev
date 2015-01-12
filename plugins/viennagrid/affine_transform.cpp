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

#include "affine_transform.hpp"
#include "viennagrid/algorithm/geometric_transform.hpp"

namespace viennamesh
{
  affine_transform::affine_transform() {}
  std::string affine_transform::name() { return "affine_transform"; }

  bool affine_transform::run(viennamesh::algorithm_handle &)
  {
    mesh_handle input_mesh = get_required_input<mesh_handle>("mesh");
    point_container_handle input_matrix = get_required_input<point_container_handle>("matrix");
    point_container_handle input_translate = get_required_input<point_container_handle>("translate");

    mesh_handle output_mesh = make_data<mesh_handle>();

    int geometric_dimension = viennagrid::geometric_dimension( input_mesh() );

    int matrix_vector_dimension = 0;
    int matrix_vector_count = 0;
    double * matrix_vector_data = 0;
    viennamesh_point_container_get(input_matrix(), &matrix_vector_data, &matrix_vector_dimension, &matrix_vector_count);

    int translate_dimension = 0;
    int translate_count = 0;
    double * translate_data = 0;
    viennamesh_point_container_get(input_translate(), &translate_data, &translate_dimension, &translate_count);


    if ( geometric_dimension != matrix_vector_dimension )
      return false;

    if ( geometric_dimension >= matrix_vector_count || geometric_dimension <= 0 )
      return false;

    if ( geometric_dimension != translate_dimension )
      return false;


    point_t translate;
    convert( input_translate(), translate );

    if (output_mesh != input_mesh)
      viennagrid::copy( input_mesh(), output_mesh() );

    viennagrid::affine_transform( output_mesh(), matrix_vector_data, translate );

    set_output( "mesh", output_mesh );

    return true;
  }

}
