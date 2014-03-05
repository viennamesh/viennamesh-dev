#include "viennamesh/algorithm/tetgen.hpp"
#include "viennamesh/algorithm/io.hpp"


int main()
{
  // creating an algorithm for reading a mesh from a file
  viennamesh::algorithm_handle reader( new viennamesh::io::mesh_reader() );

  // Setting the filename for the reader
  reader->set_input( "filename", "../data/big_and_small_cube.poly" );


  // creating an algorithm using the Tetgen meshing library for meshing a hull
  viennamesh::algorithm_handle mesher( new viennamesh::tetgen::algorithm() );

  // setting the mesher paramters
  mesher->set_input( "cell_size", 1.0 );              // maximum cell size
  mesher->set_input( "max_radius_edge_ratio", 1.5 );  // maximum radius edge ratio
  mesher->set_input( "min_dihedral_angle", 0.17 );     // minimum dihedral angle in radiant, 0.17 are about 10 degrees


  // creating an algorithm for writing a mesh to a file
  viennamesh::algorithm_handle writer( new viennamesh::io::mesh_writer() );


  // Setting the filename for the reader and writer
  reader->set_input( "filename", "../data/big_and_small_cube.poly" );
  writer->set_input( "filename", "big_and_small_cube.vtu" );

  // start the algorithms
  reader->run();

  // linking the output from the reader to the mesher
  mesher->set_input( "default", reader->get_output("default") );
  mesher->set_input( "seed_points", reader->get_output("seed_points") );
  mesher->set_input( "hole_points", reader->get_output("hole_points") );

  mesher->run();

  writer->set_input( "default", mesher->get_output("default") );

  writer->run();


  // Setting the filename for the reader and writer
  reader->set_input( "filename", "../data/cube_with_tunnel.poly" );
  writer->set_input( "filename", "cube_with_tunnel.vtu" );

  // start the algorithms
  reader->run();

  // linking the output from the reader to the mesher
  mesher->set_input( "default", reader->get_output("default") );
  mesher->set_input( "seed_points", reader->get_output("seed_points") );
  mesher->set_input( "hole_points", reader->get_output("hole_points") );

  mesher->run();

  writer->set_input( "default", mesher->get_output("default") );

  writer->run();


  // Setting the filename for the reader and writer
  reader->set_input( "filename", "../data/two_cubes.poly" );
  writer->set_input( "filename", "two_cubes.vtu" );

  // start the algorithms
  reader->run();

  // linking the output from the reader to the mesher
  mesher->set_input( "default", reader->get_output("default") );
  mesher->set_input( "seed_points", reader->get_output("seed_points") );
  mesher->set_input( "hole_points", reader->get_output("hole_points") );

  mesher->run();

  writer->set_input( "default", mesher->get_output("default") );

  writer->run();

}
