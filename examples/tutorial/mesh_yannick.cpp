#include "viennamesh/algorithm/extract_hull.hpp"
#include "viennamesh/algorithm/file_reader.hpp"
#include "viennamesh/algorithm/file_writer.hpp"


int main()
{
  // creating an algorithm for reading a mesh from a file
  viennamesh::algorithm_handle reader( new viennamesh::file_reader() );

  // creating a hull extraction algorithm
  viennamesh::algorithm_handle extract_hull( new viennamesh::extract_hull::algorithm() );

  // creating an algorithm for writing a mesh to a file
  viennamesh::algorithm_handle writer( new viennamesh::file_writer() );


  // linking the output from the reader to the mesher
  extract_hull->link_input( "default", reader, "default" );

  // linking the output from the mesher to the writer
  writer->link_input( "default", extract_hull, "default" );


  // Setting the filename for the reader and writer
  reader->set_input( "filename", "/export/florian/work/projects/2013_11 ViennaSHE Yannick/mesh/ridiculously_fine_ortho_for_florian_out.devbz.vtu_main.pvd" );
  writer->set_input( "filename", "half-trigate_hull.vtu" );

  // start the algorithms
  reader->run();
  extract_hull->run();
  writer->run();


}
