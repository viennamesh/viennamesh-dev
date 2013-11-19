#include "viennamesh/algorithm/mesher1d.hpp"
#include "viennamesh/algorithm/file_writer.hpp"


int main()
{
  // creating an algorithm using the Tetgen meshing library for meshing a hull
  viennamesh::AlgorithmHandle mesher = viennamesh::AlgorithmHandle( new viennamesh::mesher1d::Algorithm() );

  // creating an algorithm for writing a mesh to a file
  viennamesh::AlgorithmHandle writer = viennamesh::AlgorithmHandle( new viennamesh::FileWriter() );

  // Typedefing the mesh type representing the 2D geometry; using just lines, segments are represented using seed points

  typedef viennagrid::vertex_1d_mesh GeometryMeshType;
  // Typedefing vertex handle and point type for geometry creation
  typedef viennagrid::result_of::point<GeometryMeshType>::type PointType;
  typedef viennagrid::result_of::vertex_handle<GeometryMeshType>::type GeometryVertexHandle;

  // creating the geometry mesh
  viennamesh::result_of::parameter_handle< GeometryMeshType >::type geometry = viennamesh::make_parameter<GeometryMeshType>();

  double s = 10.0;
  GeometryVertexHandle vtx[10];

  vtx[0] = viennagrid::make_vertex( geometry->get(), PointType(s) );
  vtx[0] = viennagrid::make_vertex( geometry->get(), PointType(-s) );
  vtx[0] = viennagrid::make_vertex( geometry->get(), PointType(0) );
  vtx[0] = viennagrid::make_vertex( geometry->get(), PointType(2*s) );




  // setting the created line geometry as input for the mesher
  mesher->set_input( "default", geometry );

  // creating the seed points
  viennamesh::SeedPoint1DContainer seed_points;
  seed_points.push_back( std::make_pair(PointType(-s/2), 0) );
  seed_points.push_back( std::make_pair(PointType(s+s/2), 1) );

  // creating the hole points
  viennamesh::Point1DContainer hole_points;
  hole_points.push_back( PointType(s/2) );

  // setting the seed points and hole points as input for the mesher
  mesher->set_input( "seed_points", seed_points );
  mesher->set_input( "hole_points", hole_points );

  // setting the mesher paramters
  mesher->set_input( "cell_size", 1.0 );      // maximum cell size

  // linking the output from the mesher to the writer
  writer->link_input( "default", mesher, "default" );
  // Setting the filename for the reader and writer
  writer->set_input( "filename", "mesh_line.vtu" );


  // start the algorithms
  mesher->run();
  writer->run();
}
