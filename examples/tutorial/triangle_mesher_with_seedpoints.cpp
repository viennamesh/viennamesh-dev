/* ============================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                ViennaMesh - The Vienna Meshing Framework
                            -----------------

                    http://viennamesh.sourceforge.net/

   License:         MIT (X11), see file LICENSE in the base directory
=============================================================================== */

#include "viennamesh/viennamesh.hpp"


int main()
{
  // Typedefing the mesh type representing the 2D geometry; using just lines, segments are represented using seed points
  typedef viennagrid::line_2d_mesh GeometryMeshType;
  // Typedefing vertex handle and point type for geometry creation
  typedef viennagrid::result_of::point<GeometryMeshType>::type PointType;
  typedef viennagrid::result_of::vertex_handle<GeometryMeshType>::type GeometryVertexHandle;

  // creating the geometry mesh
  viennamesh::result_of::parameter_handle< GeometryMeshType >::type geometry_handle = viennamesh::make_parameter<GeometryMeshType>();
  GeometryMeshType & geometry = geometry_handle();

  double s = 10.0;
  GeometryVertexHandle vtx[10];

  vtx[0] = viennagrid::make_vertex( geometry, PointType(0, 0) );
  vtx[1] = viennagrid::make_vertex( geometry, PointType(0, s) );
  vtx[2] = viennagrid::make_vertex( geometry, PointType(s, 0) );
  vtx[3] = viennagrid::make_vertex( geometry, PointType(s, s) );
  vtx[4] = viennagrid::make_vertex( geometry, PointType(2*s, 0) );
  vtx[5] = viennagrid::make_vertex( geometry, PointType(2*s, s) );

  vtx[6] = viennagrid::make_vertex( geometry, PointType(s/3, s/3) );
  vtx[7] = viennagrid::make_vertex( geometry, PointType(s/3, 2*s/3) );
  vtx[8] = viennagrid::make_vertex( geometry, PointType(2*s/3, s/3) );
  vtx[9] = viennagrid::make_vertex( geometry, PointType(2*s/3, 2*s/3) );


  viennagrid::make_line( geometry, vtx[0], vtx[1] );

  viennagrid::make_line( geometry, vtx[0], vtx[2] );
  viennagrid::make_line( geometry, vtx[1], vtx[3] );

  viennagrid::make_line( geometry, vtx[2], vtx[3] );

  viennagrid::make_line( geometry, vtx[2], vtx[4] );
  viennagrid::make_line( geometry, vtx[3], vtx[5] );

  viennagrid::make_line( geometry, vtx[4], vtx[5] );

  viennagrid::make_line( geometry, vtx[6], vtx[7] );

  viennagrid::make_line( geometry, vtx[6], vtx[8] );
  viennagrid::make_line( geometry, vtx[7], vtx[9] );

  viennagrid::make_line( geometry, vtx[8], vtx[9] );




  // creating an algorithm using the Tetgen meshing library for meshing a hull
  viennamesh::algorithm_handle mesher( new viennamesh::triangle::make_mesh() );

  // setting the created line geometry as input for the mesher
  mesher->set_input( "mesh", geometry );

  // creating the seed points and set it as input for the mesher
  viennamesh::seed_point_2d_container seed_points;
  seed_points.push_back( std::make_pair(PointType(s/4, s/2), 0) );
  seed_points.push_back( std::make_pair(PointType(s+s/2, s/2), 1) );
  mesher->set_input( "seed_points", seed_points );

  // creating the hole points and set it as input for the mesher
  viennamesh::point_2d_container hole_points;
  hole_points.push_back( PointType(s/2, s/2) );
  mesher->set_input( "hole_points", hole_points );

  // setting the mesher paramters
  mesher->set_input( "cell_size", 1.0 );      // maximum cell size
  mesher->set_input( "min_angle", 0.35 );     // minimum angle in radiant, 0.35 are about 20 degrees
  mesher->set_input( "delaunay", true  );     // we want a Delaunay triangulation
  mesher->set_input( "algorithm_type", "incremental_delaunay" );  // incremental Delaunay algorithm is used

  // start the algorithm
  mesher->run();



  // creating an algorithm for writing a mesh to a file
  viennamesh::algorithm_handle writer( new viennamesh::io::mesh_writer() );

  // linking the output from the mesher to the writer
  writer->set_default_source(mesher);

  // Setting the filename for the reader and writer
  writer->set_input( "filename", "meshed_quads.vtu" );

  // start the algorithm
  writer->run();
}
