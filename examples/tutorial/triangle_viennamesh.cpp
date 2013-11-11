#include <iostream>

#include "viennamesh/algorithm/triangle_mesher.hpp"

#include "viennagrid/mesh/element_creation.hpp"
#include "viennagrid/io/vtk_writer.hpp"



int main()
{
  // Typedefing the mesh type representing the 2D geometry; using just lines, segments are represented using seed points
  typedef viennagrid::line_2d_mesh GeometryMeshType;

  // Typedefing vertex handle and point type for geometry creation
  typedef viennagrid::result_of::point<GeometryMeshType>::type PointType;
  typedef viennagrid::result_of::vertex_handle<GeometryMeshType>::type GeometryVertexHandle;

  // Creating the geometry mesh object
  GeometryMeshType geomerty;

  // creating the geometry
  double s = 10.0;
  GeometryVertexHandle vtx[6];

  vtx[0] = viennagrid::make_vertex( geomerty, PointType(0, 0) );
  vtx[1] = viennagrid::make_vertex( geomerty, PointType(0, s) );
  vtx[2] = viennagrid::make_vertex( geomerty, PointType(s, 0) );
  vtx[3] = viennagrid::make_vertex( geomerty, PointType(s, s) );
  vtx[4] = viennagrid::make_vertex( geomerty, PointType(2*s, 0) );
  vtx[5] = viennagrid::make_vertex( geomerty, PointType(2*s, s) );


  viennagrid::make_line( geomerty, vtx[0], vtx[1] );

  viennagrid::make_line( geomerty, vtx[0], vtx[2] );
  viennagrid::make_line( geomerty, vtx[1], vtx[3] );

  viennagrid::make_line( geomerty, vtx[2], vtx[3] );

  viennagrid::make_line( geomerty, vtx[2], vtx[4] );
  viennagrid::make_line( geomerty, vtx[3], vtx[5] );

  viennagrid::make_line( geomerty, vtx[4], vtx[5] );


  // creating a parameter set object
  viennamesh::ConstParameterSet settings;

  // creating the seed points
  // seed point constructor: seed_point_2d(x_coordinate_of_seed_point, y_coordinate_of_seed_point, segment_id)
  viennamesh::seed_point_2d_container seed_points;
  seed_points.push_back( viennamesh::seed_point_2d(s/2, s/2, 0) );
  seed_points.push_back( viennamesh::seed_point_2d(s+s/2, s/2, 1) );

  // setting the parameters
  settings.set("seed_points", seed_points);   // the seed points
  settings.set("cell_size", 1.0);             // maximum cell size is set to 1
  settings.set("min_angle", 30.0);            // minimum angle is set to 30

  // creating a triangular mesh and segmentation
  viennagrid::triangular_2d_mesh tri_mesh;
  viennagrid::triangular_2d_segmentation tri_segmentation(tri_mesh);

  // starting the algorithm
  viennamesh::run_algo<viennamesh::triangle_tag>(
    geomerty, viennamesh::NoSegmentation(),
    tri_mesh, tri_segmentation,
    settings);

  // writing the output to a VTK file
  viennagrid::io::vtk_writer<viennagrid::triangular_2d_mesh> vtk_writer;
  vtk_writer(tri_mesh, tri_segmentation, "meshed_triangles");
}
