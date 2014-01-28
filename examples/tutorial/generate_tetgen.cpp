#include "viennamesh/algorithm/tetgen.hpp"
#include "viennamesh/algorithm/io.hpp"


int main( int argc, char** argv )
{
  if (argc != 4)
  {
    std::cout << "Usage: generate_tetgen [input] [output] [tetgen_options]" << std::endl;
    return 0;
  }

  // creating an algorithm for reading a mesh from a file
  viennamesh::algorithm_handle reader( new viennamesh::io::mesh_reader() );

  // creating an algorithm using the Tetgen meshing library for meshing a hull
  viennamesh::algorithm_handle mesher( new viennamesh::tetgen::algorithm() );

  // creating an algorithm for writing a mesh to a file
  viennamesh::algorithm_handle writer( new viennamesh::io::mesh_writer() );


  // linking the output from the reader to the mesher
  mesher->link_input( "default", reader, "default" );
  mesher->link_input( "seed_points", reader, "seed_points" );
  mesher->link_input( "hole_points", reader, "hole_points" );

  mesher->set_input("use_logger", false);
  mesher->set_input("option_string", argv[3]);

  // linking the output from the mesher to the writer
  writer->link_input( "default", mesher, "default" );


  // Setting the filename for the reader and writer
  reader->set_input( "filename", argv[1] );
  writer->set_input( "filename", argv[2] );

  // start the algorithms
  reader->run();
  mesher->run();
  writer->run();


}
