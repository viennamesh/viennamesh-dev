#include "viennamesh/viennamesh.hpp"
#include "common.hpp"

inline void test()
{
  viennamesh::LoggingStack ls("Testing tetgen with a cube of side length 10");

  viennamesh::algorithm_handle reader( new viennamesh::io::mesh_reader() );
  reader->set_input( "filename", "../examples/data/cube.poly" );
  reader->run();

  viennamesh::algorithm_handle hull( new viennamesh::triangle::make_hull() );
  hull->set_default_source(reader);
  hull->run();

  viennamesh::algorithm_handle segmenter( new viennamesh::seed_point_segmenting() );
  segmenter->set_default_source(hull);
  segmenter->run();


  viennagrid::segmented_mesh<viennagrid::tetrahedral_3d_mesh, viennagrid::tetrahedral_3d_segmentation> mesh;

  viennamesh::algorithm_handle mesher( new viennamesh::netgen::make_mesh() );
  mesher->set_default_source(reader);
  mesher->set_output( "mesh", mesh );

  mesher->run();

  double vol = viennagrid::volume( mesh.mesh );
  fuzzy_check( vol, 10.0*10.0*10.0 );
}

int main()
{
  test();

  return EXIT_SUCCESS;
}
