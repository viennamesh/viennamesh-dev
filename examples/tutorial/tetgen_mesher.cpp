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

  <h2>First Step: Create the algorithms</h2>

**/

int main()
{
  // creating an algorithm for reading a mesh from a file
  viennamesh::algorithm_handle reader( new viennamesh::io::mesh_reader() );
  viennamesh::algorithm_handle mesher( new viennamesh::tetgen::make_mesh() );
  viennamesh::algorithm_handle writer( new viennamesh::io::mesh_writer() );

  /** Setting the reader algorithm as default input source algorithm for the mesher and the mesher algorithm as default input source algorithm for the writer. **/
  mesher->set_default_source(reader);
  writer->set_default_source(mesher);

  /** Setting input parameters for the meshing algorithm. The maximum size of each cell is set to 1, the maximum ratio of the tetrahedral radois and edge  is set to 1.5 and the minimum dihedral angle is set to 0.17. **/
  mesher->set_input( "cell_size", 1.0 );              // maximum cell size
  mesher->set_input( "max_radius_edge_ratio", 1.5 );  // maximum radius edge ratio
  mesher->set_input( "min_dihedral_angle", 0.17 );     // minimum dihedral angle in radiant, 0.17 are about 10 degrees

  // Setting the filename for the reader and writer
  reader->set_input( "filename", "../data/big_and_small_cube.poly" );
  writer->set_input( "filename", "big_and_small_cube.vtu" );

  // start the algorithms
  reader->run();
  mesher->run();
  writer->run();


  // Setting the filename for the reader and writer
  reader->set_input( "filename", "../data/cube_with_tunnel.poly" );
  writer->set_input( "filename", "cube_with_tunnel.vtu" );

  // start the algorithms
  reader->run();
  mesher->run();
  writer->run();


  // Setting the filename for the reader and writer
  reader->set_input( "filename", "../data/two_cubes.poly" );
  writer->set_input( "filename", "two_cubes.vtu" );

  reader->run();
  mesher->run();
  writer->run();

}
