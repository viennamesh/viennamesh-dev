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
  typedef viennagrid::brep_1d_mesh GeometryMeshType;
  typedef viennagrid::result_of::segmentation<GeometryMeshType>::type GeometrySegmentationType;
  typedef viennagrid::result_of::segment_handle<GeometrySegmentationType>::type GeometrySegmentHandleType;
  typedef viennagrid::segmented_mesh<GeometryMeshType, GeometrySegmentationType> SegmentedGeometryMeshType;

  // Typedefing vertex handle and point type for geometry creation
  typedef viennagrid::result_of::point<GeometryMeshType>::type PointType;
  typedef viennagrid::result_of::vertex_handle<GeometryMeshType>::type GeometryVertexHandle;

  // creating the geometry mesh
  viennamesh::result_of::parameter_handle< SegmentedGeometryMeshType >::type geometry_handle = viennamesh::make_parameter<SegmentedGeometryMeshType>();
  GeometryMeshType & geometry = geometry_handle().mesh;
  GeometrySegmentationType & segmentation = geometry_handle().segmentation;

  double s = 10.0;
  GeometryVertexHandle vtx[7];

  vtx[0] = viennagrid::make_vertex( geometry, PointType(s) );
  vtx[1] = viennagrid::make_vertex( geometry, PointType(-s) );
  vtx[2] = viennagrid::make_vertex( geometry, PointType(0) );
  vtx[3] = viennagrid::make_vertex( geometry, PointType(0) );
  vtx[4] = viennagrid::make_vertex( geometry, PointType(0) );
  vtx[5] = viennagrid::make_vertex( geometry, PointType(2*s) );
  vtx[6] = viennagrid::make_vertex( geometry, PointType(3*s) );

  GeometrySegmentHandleType segment0 = segmentation.make_segment();
  viennagrid::add( segment0, vtx[1] );
  viennagrid::add( segment0, vtx[2] );

  GeometrySegmentHandleType segment1 = segmentation.make_segment();
  viennagrid::add( segment1, vtx[0] );
  viennagrid::add( segment1, vtx[5] );


  // creating an algorithm using the Tetgen meshing library for meshing a hull
  viennamesh::algorithm_handle mesher( new viennamesh::make_line_mesh() );

  // setting the created line geometry as input for the mesher
  mesher->set_input( "mesh", geometry_handle );

  // creating the seed points
//   viennamesh::seed_point_1d_container seed_points;
//   seed_points.push_back( std::make_pair(PointType(-s/2), 0) );
//   seed_points.push_back( std::make_pair(PointType(s+s/2), 1) );
//   seed_points.push_back( std::make_pair(PointType(2*s+s/2), 2) );

  // creating the hole points
  viennamesh::point_1d_container hole_points;
  hole_points.push_back( PointType(s/2) );

  // setting the seed points and hole points as input for the mesher
//   mesher->set_input( "seed_points", seed_points );
  mesher->set_input( "hole_points", hole_points );

  // setting the mesher paramters
  mesher->set_input( "cell_size", 1.0 );      // maximum cell size

  mesher->set_input( "make_segmented_mesh", true );      // make a segmented mesh? This is automatically true if seed points were specified

  mesher->set_input( "use_different_segment_ids_for_unknown_segments", false );      // using different segment IDs for segments which are not covered by any seed point

  mesher->set_input( "absolute_min_geometry_point_distance", 1e-10 );   // minimal distance between 2 points of the input geometry. IF two points are closer than this parameter one is discarded. an absolute_min_geometry_point_distance parameter will overwrite a relative_min_geometry_point_distance

  mesher->set_input( "relative_min_geometry_point_distance", 1e-10 );   // relative minimal distance between 2 points of the input geometry. If two points are closer than (max(points) - min(points)) * relative_min_geometry_point_distance one is discarded. If absolute_min_geometry_point_distance is set this parameter is ignored.

  // start the algorithm
  mesher->run();


  // creating an algorithm for writing a mesh to a file
  viennamesh::algorithm_handle writer( new viennamesh::io::mesh_writer() );

  // linking the output from the mesher to the writer
  writer->set_default_source(mesher);
//   writer->set_input( "default", mesher->get_output("default") );

  // Setting the filename for the reader and writer
  writer->set_input( "filename", "mesh_line.vtu" );

  // start the algorithm
  writer->run();
}
