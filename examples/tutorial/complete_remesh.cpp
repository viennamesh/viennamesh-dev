#include "viennamesh/algorithm/extract_hull.hpp"
#include "viennamesh/algorithm/extract_plc.hpp"
#include "viennamesh/algorithm/tetgen/tetgen.hpp"
#include "viennamesh/algorithm/file_reader.hpp"
#include "viennamesh/algorithm/file_writer.hpp"

//////////////////////////////////////////////
//////////////////////////////////////////////
//////////////////////////////////////////////

//   NO WORKING CURRENTLY !!!

//////////////////////////////////////////////
//////////////////////////////////////////////
//////////////////////////////////////////////


int main()
{
  // creating an algorithm for reading a mesh from a file
  viennamesh::AlgorithmHandle reader = viennamesh::AlgorithmHandle( new viennamesh::FileReader() );

  viennamesh::AlgorithmHandle mesher0 = viennamesh::AlgorithmHandle( new viennamesh::tetgen::Algorithm() );

  // creating an algorithm using the Tetgen meshing library for meshing a hull
  viennamesh::AlgorithmHandle extract_hull = viennamesh::AlgorithmHandle( new viennamesh::extract_hull::Algorithm() );
  viennamesh::AlgorithmHandle extract_plc = viennamesh::AlgorithmHandle( new viennamesh::extract_plc::Algorithm() );
  viennamesh::AlgorithmHandle mesher1 = viennamesh::AlgorithmHandle( new viennamesh::tetgen::Algorithm() );

  // creating an algorithm for writing a mesh to a file
  viennamesh::AlgorithmHandle writer0 = viennamesh::AlgorithmHandle( new viennamesh::FileWriter() );
  viennamesh::AlgorithmHandle writer1 = viennamesh::AlgorithmHandle( new viennamesh::FileWriter() );
  viennamesh::AlgorithmHandle writer2 = viennamesh::AlgorithmHandle( new viennamesh::FileWriter() );


  // linking the output from the reader to the mesher
  mesher0->link_input( "default", reader, "default" );
  mesher0->link_input( "seed_points", reader, "seed_points" );
  mesher0->link_input( "hole_points", reader, "hole_points" );
  mesher0->set_input( "cell_size", 1.0 );

  extract_hull->link_input( "default", mesher0, "default" );
  extract_plc->link_input( "default", extract_hull, "default" );

  mesher1->link_input( "default", extract_plc, "default" );
  mesher1->link_input( "seed_points", extract_hull, "seed_points" );
  mesher1->set_input( "cell_size", 1.0 );

  // linking the output from the mesher to the writer
  writer0->link_input( "default", mesher0, "default" );
  writer1->link_input( "default", extract_hull, "default" );
  writer2->link_input( "default", mesher1, "default" );


  // Setting the filename for the reader and writer
  reader->set_input( "filename", "../data/big_and_small_cube.poly" );
  writer0->set_input( "filename", "cube_with_tunnel_pass0.vtu" );
  writer1->set_input( "filename", "cube_with_tunnel_pass0_hull.vtu" );
  writer2->set_input( "filename", "cube_with_tunnel_pass1.vtu" );

  // start the algorithms
  reader->run();
  mesher0->run();
  writer0->run();
  extract_hull->run();
  writer1->run();
  extract_plc->run();
  mesher1->run();
  writer2->run();

}

