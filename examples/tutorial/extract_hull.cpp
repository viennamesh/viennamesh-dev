#include "viennamesh/algorithm/extract_hull.hpp"
#include "viennamesh/algorithm/file_reader.hpp"
#include "viennamesh/algorithm/file_writer.hpp"


int main()
{
  // creating an algorithm for reading a mesh from a file
  viennamesh::AlgorithmHandle reader = viennamesh::AlgorithmHandle( new viennamesh::FileReader() );

  // creating a hull extraction algorithm
  viennamesh::AlgorithmHandle extract_hull = viennamesh::AlgorithmHandle( new viennamesh::extract_hull::Algorithm() );

  // creating an algorithm for writing a mesh to a file
  viennamesh::AlgorithmHandle writer = viennamesh::AlgorithmHandle( new viennamesh::FileWriter() );


  // linking the output from the reader to the mesher
  extract_hull->link_input( "default", reader, "default" );

  // linking the output from the mesher to the writer
  writer->link_input( "default", extract_hull, "default" );


  // Setting the filename for the reader and writer
  reader->set_input( "filename", "../data/half-trigate.mesh" );
  writer->set_input( "filename", "half-trigate_hull.vtu" );

  // start the algorithms
  reader->run();
  extract_hull->run();
  writer->run();


}
