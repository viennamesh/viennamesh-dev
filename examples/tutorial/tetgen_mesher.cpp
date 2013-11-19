#include "viennamesh/algorithm/tetgen.hpp"
#include "viennamesh/algorithm/file_reader.hpp"
#include "viennamesh/algorithm/file_writer.hpp"


int main()
{
  // creating an algorithm for reading a mesh from a file
  viennamesh::algorithm_handle reader( new viennamesh::file_reader() );

  // creating an algorithm using the Tetgen meshing library for meshing a hull
  viennamesh::algorithm_handle mesher( new viennamesh::tetgen::algorithm() );

  // creating an algorithm for writing a mesh to a file
  viennamesh::algorithm_handle writer( new viennamesh::file_writer() );


  // linking the output from the reader to the mesher
  mesher->link_input( "default", reader, "default" );
  mesher->link_input( "seed_points", reader, "seed_points" );
  mesher->link_input( "hole_points", reader, "hole_points" );


  // setting the mesher paramters
  mesher->set_input( "cell_size", 1.0 );              // maximum cell size
  mesher->set_input( "max_radius_edge_ratio", 1.5 );  // maximum radius edge ratio
  mesher->set_input( "min_dihedral_angle", 0.17 );     // minimum dihedral angle in radiant, 0.17 are about 10 degrees

  // linking the output from the mesher to the writer
  writer->link_input( "default", mesher, "default" );


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


}
