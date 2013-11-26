#include "viennamesh/algorithm/netgen.hpp"
#include "viennamesh/algorithm/io.hpp"


int main()
{

  // creating an algorithm using the Tetgen meshing library for meshing a hull
  viennamesh::algorithm_handle mesher( new viennamesh::netgen::csg_mesher() );

  // creating an algorithm for writing a mesh to a file
  viennamesh::algorithm_handle writer( new viennamesh::io::mesh_writer() );

  // Define the CSG string
  string csg_string =
  "algebraic3d \n"
  "solid cube = plane (0, 0, 0; 0, 0, -1) \n"
  "       and plane (0, 0, 0; 0, -1, 0) \n"
  "       and plane (0, 0, 0; -1, 0, 0) \n"
  "       and plane (1, 1, 1; 0, 0, 1) \n"
  "       and plane (1, 1, 1; 0, 1, 0) \n"
  "       and plane (1, 1, 1; 1, 0, 0); \n"
  "tlo cube;\n";


  // setting the mesher paramters
  mesher->set_input( "default", csg_string );              // the CSG string

  // linking the output from the mesher to the writer
  writer->link_input( "default", mesher, "default" );


  // Setting the filename for the reader and writer
  writer->set_input( "filename", "cube_netgen_csg.vtu" );

  // start the algorithms
  mesher->run();
  writer->run();
}
