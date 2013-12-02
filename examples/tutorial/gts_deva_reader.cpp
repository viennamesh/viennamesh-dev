#include "viennamesh/algorithm/io.hpp"


int main()
{
  viennamesh::algorithm_handle reader( new viennamesh::io::mesh_reader() );
  viennamesh::algorithm_handle writer( new viennamesh::io::mesh_writer() );

  reader->set_input( "filename", "/export/florian/work/projects/2013_11 ViennaSHE Yannick/geometry.deva" );

  writer->link_input( "default", reader, "default" );

  writer->set_input( "filename", "deva_test.vmesh" );

  reader->run();
  writer->run();
}
