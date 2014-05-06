#include "viennamesh/viennamesh.hpp"
#include "common.hpp"

inline void test()
{
  viennamesh::LoggingStack ls("Testing tetgen with a cube of side length 10");

  // creating an algorithm for reading a mesh from a file
  viennamesh::algorithm_handle reader( new viennamesh::io::mesh_reader() );

  // Setting the filename for the reader and writer
  reader->set_input( "filename", "../examples/data/cube.poly" );

  // start the algorithm
  reader->run();


  // creating an algorithm using the Tetgen meshing library for meshing a hull
  viennamesh::algorithm_handle mesher( new viennamesh::tetgen::make_mesh() );

  viennagrid::segmented_mesh<viennagrid::tetrahedral_3d_mesh, viennagrid::tetrahedral_3d_segmentation> mesh;

  // linking the output from the reader to the mesher
  mesher->set_default_source(reader);
  mesher->set_output( "mesh", mesh );

  // start the algorithm
  mesher->run();

  double vol = viennagrid::volume( mesh.mesh );
  fuzzy_check( vol, 10.0*10.0*10.0 );
}

int main()
{
  test();

  return EXIT_SUCCESS;
}
