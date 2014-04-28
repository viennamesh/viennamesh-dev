#include "viennamesh/viennamesh.hpp"
#include "viennagrid/io/vtk_writer.hpp"
#include "viennamesh/statistic/statistic.hpp"

int main()
{
  // creating an algorithm for reading a mesh from a file
  viennamesh::algorithm_handle reader( new viennamesh::io::mesh_reader() );

  // Setting the filename for the reader and writer
  reader->set_input( "filename", "../data/big_and_small_cube.poly" );

  // start the algorithm
  reader->run();


  // creating an algorithm using the Tetgen meshing library for meshing a hull
  viennamesh::algorithm_handle mesher( new viennamesh::tetgen::mesh_generator() );

  viennagrid::segmented_mesh<viennagrid::tetrahedral_3d_mesh, viennagrid::tetrahedral_3d_segmentation> mesh;

  // linking the output from the reader to the mesher
  mesher->set_default_source(reader);

  // setting the mesher paramters
  mesher->set_input( "cell_size", 1.0 );              // maximum cell size
  mesher->set_input( "max_radius_edge_ratio", 1.5 );  // maximum radius edge ratio
  mesher->set_input( "min_dihedral_angle", 0.17 );     // minimum dihedral angle in radiant, 0.17 are about 10 degrees


  mesher->set_output( "mesh", mesh );

  // start the algorithm
  mesher->run();

  typedef viennagrid::result_of::cell<viennagrid::tetrahedral_3d_mesh>::type CellType;

  viennamesh::statistic<double> s(10);
  s(mesh.mesh, viennamesh::aspect_ratio<CellType>);

  std::cout << s << std::endl;


  viennagrid::io::vtk_writer<viennagrid::tetrahedral_3d_mesh> writer;
  writer(mesh.mesh, mesh.segmentation, "test");
}
