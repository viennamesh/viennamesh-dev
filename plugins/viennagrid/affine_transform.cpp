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
    point_handle input_matrix = get_required_input<point_handle>("matrix");
    point_handle input_translate = get_required_input<point_handle>("translate");

    mesh_handle output_mesh = make_data<mesh_handle>();

    int geometric_dimension = viennagrid::geometric_dimension( input_mesh() );

    viennagrid::point translate = input_translate();
    std::vector<viennagrid::point> matrix_rows;
    for (int i = 0; i != input_matrix.size(); ++i)
      matrix_rows.push_back( input_matrix(i) );

    int row_count = matrix_rows.size();
    int column_count = matrix_rows[0].size();


    if ( geometric_dimension != column_count )
    {
      error(1) << "Dimension mismatch: geometric_dimension = " << geometric_dimension << " matrix column_count = " << column_count << std::endl;
      return false;
    }

    if ( geometric_dimension > row_count || geometric_dimension <= 0 )
    {
      error(1) << "Argument error: matrix  = " << row_count  << " geometric_dimension = " << geometric_dimension << std::endl;
      return false;
    }

    if ( geometric_dimension != static_cast<int>(translate.size()) )
    {
      error(1) << "Dimension mismatch: geometric_dimension = " << geometric_dimension << " translate_dimension = " << translate.size() << std::endl;
      return false;
    }


    std::vector<double> matrix_values;
    for (std::size_t i = 0; i != matrix_rows.size(); ++i)
      for (std::size_t j = 0; j != matrix_rows[i].size(); ++j)
        matrix_values.push_back( matrix_rows[i][j] );

    if (output_mesh != input_mesh)
      viennagrid::copy( input_mesh(), output_mesh() );

    viennagrid::affine_transform( output_mesh(), &matrix_values[0], translate );

    set_output( "mesh", output_mesh );

    return true;
  }

}
