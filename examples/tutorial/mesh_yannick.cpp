#include "viennamesh/algorithm/viennagrid.hpp"
#include "viennamesh/algorithm/io.hpp"
#include "viennamesh/algorithm/triangle.hpp"


int main()
{
  // creating an algorithm for reading a mesh from a file
  viennamesh::algorithm_handle reader( new viennamesh::io::mesh_reader() );

  // creating a hull extraction algorithm
  viennamesh::algorithm_handle mesher( new viennamesh::triangle::algorithm() );

  // creating an algorithm for writing a mesh to a file
  viennamesh::algorithm_handle writer( new viennamesh::io::mesh_writer() );


  // linking the output from the reader to the mesher
  mesher->link_input( "default", reader, "default" );
  writer->link_input( "default", mesher, "default" );


  // Setting the filename for the reader and writer
  reader->set_input( "filename", "/export/florian/work/projects/2013_11 ViennaSHE Yannick/geometry.deva" );

  mesher->set_input( "min_angle", 0.35 );     // minimum angle in radiant, 0.35 are about 20 degrees
  mesher->set_input( "delaunay", true  );     // we want a Delaunay triangulation
  mesher->set_input( "algorithm_type", "incremental_delaunay" );  // incremental Delaunay algorithm is used

  writer->set_input( "filename", "nld_mosfet.vtu" );

  // start the algorithms
  reader->run();
  mesher->run();
  writer->run();


}
