#include <iostream>
#include <map>

#include "viennamesh/forwards.hpp"

#include "viennamesh/algorithm/tetgen_tetrahedron_mesher.hpp"
#include "viennamesh/algorithm/cgal_delaunay_tetrahedron_mesher.hpp"
#include "viennamesh/algorithm/file_reader.hpp"
#include "viennamesh/algorithm/file_writer.hpp"


int main()
{
  // creating an algorithm for reading a mesh from a file
  viennamesh::AlgorithmHandle reader = viennamesh::AlgorithmHandle( new viennamesh::FileReader() );

  // creating an algorithm using the Tetgen meshing library for meshing a hull
  viennamesh::AlgorithmHandle mesher = viennamesh::AlgorithmHandle( new viennamesh::Algorithm<viennamesh::tetgen_tetrahedron_tag>() );

  // creating an algorithm for writing a mesh to a file
  viennamesh::AlgorithmHandle writer = viennamesh::AlgorithmHandle( new viennamesh::FileWriter() );


  // linking the output from the reader to the mesher
  mesher->link_input( "default", reader, "default" );
  mesher->link_input( "seed_points", reader, "seed_points" );
  mesher->link_input( "hole_points", reader, "hole_points" );
  // set the cell size parameter for the mesher
  mesher->set_input( "cell_size", 1.0 );

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
