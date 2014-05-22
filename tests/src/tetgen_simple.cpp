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

#include "viennamesh/viennamesh.hpp"
#include "common.hpp"

inline void test(std::string const & filename, double expected_volume)
{
  viennamesh::LoggingStack ls("Testing tetgen");
  viennamesh::info(1) << "Using file: \"" << filename << "\"" << std::endl;

  viennamesh::algorithm_handle reader( new viennamesh::io::mesh_reader() );
  reader->set_input( "filename", filename );
  checked_run(reader);

  viennagrid::segmented_mesh<viennagrid::tetrahedral_3d_mesh, viennagrid::tetrahedral_3d_segmentation> mesh;

  viennamesh::algorithm_handle mesher( new viennamesh::tetgen::make_mesh() );
  mesher->set_default_source(reader);
  mesher->set_output( "mesh", mesh );
  checked_run(mesher);

  double volume = viennagrid::volume( mesh.mesh );

  viennamesh::info(1) << "Expected volume: " << expected_volume << std::endl;
  viennamesh::info(1) << "Actual volume  : " << volume << std::endl;

  fuzzy_check( volume, expected_volume );
}

int main()
{
  test("../examples/data/cube.poly", 10*10*10);
  test("../examples/data/big_and_small_cube.poly", 20*20*20 + 10*10*20);
  test("../examples/data/cube_using_lines.poly", 10*10*10);
  test("../examples/data/cube_with_cube_hole.poly", 20*20*20 - 10*10*10);
  test("../examples/data/cube_with_tunnel.poly", 20*20*20 - 4*4*20);
  test("../examples/data/cubeception.poly", 60*60*60 - 40*40*40 + 20*20*20);
  test("../examples/data/two_cubes.poly", 2 * 10*10*10);

  return EXIT_SUCCESS;
}
