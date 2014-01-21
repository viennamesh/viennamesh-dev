
#include "viennamesh/algorithm/cgal.hpp"
#include "viennamesh/algorithm/io.hpp"


int main()
{
  // creating an algorithm using the Tetgen meshing library for meshing a hull
  viennamesh::algorithm_handle mesher( new viennamesh::cgal::algorithm() );

  // creating an algorithm for writing a mesh to a file
  viennamesh::algorithm_handle writer( new viennamesh::io::mesh_writer() );


  // Typedefing the mesh type representing the 3D geometry; using triangles
  typedef viennagrid::triangular_3d_mesh GeometryMeshType;

  // Typedefing vertex handle, line handle and point type for geometry creation
  typedef viennagrid::result_of::point<GeometryMeshType>::type PointType;
  typedef viennagrid::result_of::vertex_handle<GeometryMeshType>::type GeometryVertexHandle;

  // creating the geometry mesh and segmentation
  viennamesh::result_of::parameter_handle< GeometryMeshType >::type geometry_handle = viennamesh::make_parameter<GeometryMeshType>();
  GeometryMeshType & geometry = geometry_handle();

  double s = 10.0;
  GeometryVertexHandle vtx[8];

  vtx[0] = viennagrid::make_vertex( geometry, PointType(0, 0, 0) );
  vtx[1] = viennagrid::make_vertex( geometry, PointType(0, s, 0) );
  vtx[2] = viennagrid::make_vertex( geometry, PointType(s, 0, 0) );
  vtx[3] = viennagrid::make_vertex( geometry, PointType(s, s, 0) );

  vtx[4] = viennagrid::make_vertex( geometry, PointType(0, 0, s) );
  vtx[5] = viennagrid::make_vertex( geometry, PointType(0, s, s) );
  vtx[6] = viennagrid::make_vertex( geometry, PointType(s, 0, s) );
  vtx[7] = viennagrid::make_vertex( geometry, PointType(s, s, s) );


  viennagrid::make_triangle( geometry, vtx[0], vtx[1], vtx[2] );
  viennagrid::make_triangle( geometry, vtx[2], vtx[1], vtx[3] );

  viennagrid::make_triangle( geometry, vtx[4], vtx[6], vtx[5] );
  viennagrid::make_triangle( geometry, vtx[6], vtx[7], vtx[5] );

  viennagrid::make_triangle( geometry, vtx[0], vtx[2], vtx[4] );
  viennagrid::make_triangle( geometry, vtx[2], vtx[6], vtx[4] );

  viennagrid::make_triangle( geometry, vtx[1], vtx[5], vtx[3] );
  viennagrid::make_triangle( geometry, vtx[3], vtx[5], vtx[7] );

  viennagrid::make_triangle( geometry, vtx[0], vtx[4], vtx[1] );
  viennagrid::make_triangle( geometry, vtx[1], vtx[4], vtx[5] );

  viennagrid::make_triangle( geometry, vtx[2], vtx[3], vtx[6] );
  viennagrid::make_triangle( geometry, vtx[3], vtx[7], vtx[6] );



  // linking the output from the reader to the mesher
  mesher->set_input( "default", geometry_handle );

  // setting the mesher paramters
  mesher->set_input( "cell_size", 0.3 );              // maximum cell size
  mesher->set_input( "max_radius_edge_ratio", 3.0);  // maximum radius edge ratio
  mesher->set_input( "min_facet_angle", 0.52 );  // maximum radius edge ratio

  // linking the output from the mesher to the writer
  writer->link_input( "default", mesher, "default" );


  // Setting the filename for the reader and writer
  writer->set_input( "filename", "meshed_cube.vtu" );

  // start the algorithms
  mesher->run();
  writer->run();
}
