#include "viennamesh/viennamesh.hpp"


int main()
{
  // creating an algorithm for reading a mesh from a file
  viennamesh::algorithm_handle reader( new viennamesh::io::mesh_reader() );

  viennamesh::algorithm_handle mesher( new viennamesh::tetgen::mesh_generator() );

  // creating a hull extraction algorithm
  viennamesh::algorithm_handle extract_hull( new viennamesh::extract_boundary() );

  // creating an algorithm for writing a mesh to a file
  viennamesh::algorithm_handle writer( new viennamesh::io::mesh_writer() );

  reader->set_input( "filename", "../data/cubeception.poly" );

  mesher->set_default_source(reader);

  // linking the output from the reader to the mesher
  extract_hull->set_default_source(mesher);

  // linking the output from the mesher to the writer
  writer->set_default_source(extract_hull);
  // Setting the filename for the reader and writer
  writer->set_input( "filename", "half-trigate_hull.vtu" );

  // start the algorithms
  reader->run();
  mesher->run();
  extract_hull->run();
  writer->run();


}
