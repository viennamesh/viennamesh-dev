#include <iostream>
#include <map>

#include "viennamesh/forwards.hpp"

#include "viennamesh/algorithm/tetgen_tetrahedron_mesher.hpp"
#include "viennamesh/algorithm/file_reader.hpp"
#include "viennamesh/algorithm/file_writer.hpp"


int main()
{
//   viennamesh::logger().register_callback( new viennamesh::FileStreamCallback<viennamesh::FileStreamFormater>("viennamesh.log") );

  viennamesh::AlgorithmHandle reader = viennamesh::AlgorithmHandle( new viennamesh::FileReader() );
  viennamesh::AlgorithmHandle mesher = viennamesh::AlgorithmHandle( new viennamesh::Algorithm<viennamesh::tetgen_tetrahedron_tag>() );
  viennamesh::AlgorithmHandle writer = viennamesh::AlgorithmHandle( new viennamesh::FileWriter() );

  reader->set_input( "filename", "../data/cube_triangular_hull.vtu" );
  reader->run();

  mesher->set_input( "default", reader->get_output( "default" ) );
  mesher->set_input( "cell_size", 1.0 );
  mesher->run();

  writer->set_input( "default", mesher->get_output( "default" ) );
  writer->set_input( "filename", "dynamic_meshed_cube.vtu" );
  writer->run();

}
