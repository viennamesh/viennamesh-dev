#include <iostream>

#include "viennamesh/mesh/tetgen_tetrahedron.hpp"
#include "viennamesh/algorithm/tetgen_tetrahedron_mesher.hpp"
#include "viennamesh/algorithm/cgal_delaunay_tetrahedron_mesher.hpp"
#include "viennagrid/io/vtk_writer.hpp"


int main()
{
  viennamesh::MeshHandle input = viennamesh::readFile("../data/cube_triangular_hull.vtu");

//   viennamesh::AlgorithmHandle algo( new viennamesh::Algorithm<viennamesh::tetgen_tetrahedron_tag>() );
  viennamesh::AlgorithmHandle algo( new viennamesh::Algorithm<viennamesh::cgal_delaunay_tetrahedron_tag>() );

  algo->setParameter( "cell_size", 1.0 );
  algo->setInput(input);
  algo->run();

  algo->getOutput()->write("dynamic_meshed_cube.vtu");
}
