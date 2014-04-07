#include "viennamesh/viennamesh.hpp"

int main()
{
  viennamesh::algorithm_handle reader( new viennamesh::io::mesh_reader() );
  reader->set_input( "filename", "/export/florian/work/projects/2013_11 ViennaSHE Yannick/geometry.deva" );
  reader->run();


  viennamesh::algorithm_handle writer( new viennamesh::io::mesh_writer() );
  writer->set_input( "default", reader->get_output("default") );
  writer->set_input( "filename", "deva_test.vmesh" );
  writer->run();
}
