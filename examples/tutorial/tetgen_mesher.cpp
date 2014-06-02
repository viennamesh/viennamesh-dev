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

/** \example tetgen_mesher.cpp

  In this example the Tetgen algorithm is used to create a 3d tetrahedral mesh using the boundary representation of different geometries.

  <h2>First step: Create the algorithms</h2>

  We need three algorithms in this example: the mesh reading algorithm viennamesh::io::mesh_reader for reading the boundary representation geometry, the meshing algorithm itself, and the mesh writer viennamesh::io::mesh_writer for writing the resulting tetrahedral mesh to a file. In this example, the Tetgen algorithm viennamesh::tetgen::make_mesh is used to generate a mesh based on a boundary representation geometry.
**/

int main()
{
  viennamesh::algorithm_handle reader( new viennamesh::io::mesh_reader() );
  viennamesh::algorithm_handle mesher( new viennamesh::tetgen::make_mesh() );
  viennamesh::algorithm_handle writer( new viennamesh::io::mesh_writer() );

  /** Setting the reader algorithm as default input source algorithm for the mesher and the mesher algorithm as default input source algorithm for the writer. **/
  mesher->set_default_source(reader);
  writer->set_default_source(mesher);

  /** <h2>Second step: Setting algorithm parameters for all meshing operations</h2>
   Setting input parameters for the meshing algorithm. The maximum size of each cell is set to 1, the maximum ratio of the tetrahedral radois and edge  is set to 1.5 and the minimum dihedral angle is set to 0.17. **/
  mesher->set_input( "cell_size", 1.0 );
  mesher->set_input( "max_radius_edge_ratio", 1.5 );
  mesher->set_input( "min_dihedral_angle", 0.17 );


  /** <h2>Third step: Setting algorithm parameters and executing the aqlgorithms</h2>
   Setting the input and output filename for the reader and writer algorithm. Using the boundary representation of a big and a small cube as input geometry. The resulting tetrahedral mesh will be written to the big_and_small_cube.pvd and big_and_small_cube_*.vtu files in VTK format. **/
  reader->set_input( "filename", "../data/big_and_small_cube.poly" );
  writer->set_input( "filename", "big_and_small_cube.vtu" );

  /** Execute the algorithms. **/
  reader->run();
  mesher->run();
  writer->run();


  /** Setting the input and output filename for the reader and writer algorithm. Using the boundary representation of a cube with a tunnel as input geometry. The resulting tetrahedral mesh will be written to the cube_with_tunnel.vtu file in VTK format. **/
  reader->set_input( "filename", "../data/cube_with_tunnel.poly" );
  writer->set_input( "filename", "cube_with_tunnel.vtu" );

  /** Execute the algorithms. **/
  reader->run();
  mesher->run();
  writer->run();


  /** Setting the input and output filename for the reader and writer algorithm. Using the boundary representation of two cubes as input geometry. The resulting tetrahedral mesh will be written to the two_cubes.pvd and two_cubes*.vtu files in VTK format. **/
  reader->set_input( "filename", "../data/two_cubes.poly" );
  writer->set_input( "filename", "two_cubes.vtu" );

  /** Execute the algorithms. **/
  reader->run();
  mesher->run();
  writer->run();
}
