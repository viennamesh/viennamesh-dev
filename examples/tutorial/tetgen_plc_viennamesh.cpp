#include <iostream>

#include "viennamesh/algorithm/tetgen_tetrahedron_mesher.hpp"

#include "viennagrid/mesh/element_creation.hpp"
#include "viennagrid/io/vtk_writer.hpp"
#include "viennagrid/io/tetgen_poly_reader.hpp"





int main()
{
  // Typedefing the mesh type representing the 2D geometry; using PLCs
  typedef viennagrid::plc_3d_mesh GeometryMeshType;
  typedef viennagrid::result_of::point<GeometryMeshType>::type PointType;

  // Creating the geometry mesh object and loading a file
  GeometryMeshType geometry;

  // The container of hole points, either filled manually or automatically filled by tetgen_poly_reader
  std::vector<PointType> hole_points;
  // The container of seed points, either filled manually or automatically filled by tetgen_poly_reader
  std::vector< std::pair<PointType, int> > seed_points;

  // using tetgen_poly_reader to fill hole points and seed points
  viennagrid::io::tetgen_poly_reader reader;
//   reader(geometry, "../data/cube.poly", hole_points, seed_points);
  reader(geometry, "../data/two_cubes.poly", hole_points, seed_points);
//   reader(geometry, "../data/big_and_small_cube.poly", hole_points, seed_points);
//   reader(geometry, "../data/example.poly", hole_points, seed_points);
//   reader(geometry, "../data/cube_with_tunnel.poly", hole_points, seed_points);
//   reader(geometry, "../data/cube_with_cube_hole.poly", hole_points, seed_points);

  // creating a parameter set object
  viennamesh::ConstParameterSet settings;

  // setting the parameters
  settings.set("seed_points", seed_points);   // the seed points
  settings.set("hole_points", hole_points);   // the seed points
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
