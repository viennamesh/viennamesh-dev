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
//   viennamesh::AlgorithmHandle mesher = viennamesh::AlgorithmHandle( new viennamesh::Algorithm<viennamesh::cgal_delaunay_tetrahedron_tag>() );

  // creating an algorithm for writing a mesh to a file
  viennamesh::AlgorithmHandle writer = viennamesh::AlgorithmHandle( new viennamesh::FileWriter() );

  // Setting the filename input for the reader
  reader->set_input( "filename", "../data/cube_triangular_hull.vtu" );
  // start the reader algorithm
  reader->run();

  // using the output of the reader algorithm as input for the Tetgen algorithm
  mesher->set_input( "default", reader->get_output( "default" ) );
  // setting the desired cell size to 1.0
  mesher->set_input( "cell_size", 1.0 );
  // start the Tetgen algorithm
  mesher->run();

  // using the output of the Tetgen algorithm as input for the writer algorithm
  writer->set_input( "default", mesher->get_output( "default" ) );
  // providing the filename to which the file should be written
  writer->set_input( "filename", "dynamic_meshed_cube.vtu" );
  // start the writer algorithm
  writer->run();
}
