#include <iostream>

#include "viennamesh/algorithm/tetgen_tetrahedron_mesher.hpp"

#include "viennagrid/mesh/element_creation.hpp"
#include "viennagrid/io/vtk_writer.hpp"
#include "viennagrid/io/tetgen_poly_reader.hpp"





int main()
{
  // Typedefing the mesh type representing the 2D geometry; using PLCs
  typedef viennagrid::plc_3d_mesh GeometryMeshType;

  // Creating the geometry mesh object and loading a file
  GeometryMeshType geometry;

  viennagrid::io::tetgen_poly_reader reader;
  reader(geometry, "../data/big_and_small_cube.poly");

  // creating a parameter set object
  viennamesh::ConstParameterSet settings;

  // creating the seed points
  // seed point constructor: seed_point_3d(x_coordinate_of_seed_point, y_coordinate_of_seed_point, z_coordinate_of_seed_point, segment_id)
  viennamesh::seed_point_3d_container seed_points;
  seed_points.push_back( viennamesh::seed_point_3d(0, 0, 0, 0) );
  seed_points.push_back( viennamesh::seed_point_3d(0, 0, 20, 1) );

  // setting the parameters
  settings.set("seed_points", seed_points);   // the seed points
  settings.set("cell_size", 1.0);

  // creating a triangular mesh and segmentation
  viennagrid::tetrahedral_3d_mesh tet_mesh;
  viennagrid::tetrahedral_3d_segmentation tet_segmentation(tet_mesh);

  // starting the algorithm
  viennamesh::run_algo<viennamesh::tetgen_tetrahedron_tag>(
    geometry, viennamesh::NoSegmentation(),
    tet_mesh, tet_segmentation,
    settings);

  // writing the output to a VTK file
  viennagrid::io::vtk_writer<viennagrid::tetrahedral_3d_mesh> vtk_writer;
  vtk_writer(tet_mesh, tet_segmentation, "cube_meshed_tetgen");

}
