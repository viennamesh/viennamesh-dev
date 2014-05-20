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
  // Typedefing the mesh type representing the 2D geometry; using just lines
  typedef viennagrid::line_2d_mesh GeometryMeshType;
  typedef viennagrid::result_of::segmentation<GeometryMeshType>::type GeometrySegmentationType;
  typedef viennagrid::result_of::segment_handle<GeometrySegmentationType>::type GeometrySegmentHandleType;
  typedef viennagrid::segmented_mesh<GeometryMeshType, GeometrySegmentationType> SegmentedGeometryMeshType;

  // Typedefing vertex handle, line handle and point type for geometry creation
  typedef viennagrid::result_of::point<GeometryMeshType>::type PointType;
  typedef viennagrid::result_of::vertex_handle<GeometryMeshType>::type GeometryVertexHandle;
  typedef viennagrid::result_of::line_handle<GeometryMeshType>::type GeometryLineHandle;

  // creating the geometry mesh and segmentation
  viennamesh::result_of::parameter_handle< SegmentedGeometryMeshType >::type geometry_handle = viennamesh::make_parameter<SegmentedGeometryMeshType>();
  GeometryMeshType & geometry = geometry_handle().mesh;
  GeometrySegmentationType & segmentation = geometry_handle().segmentation;

  double s = 10.0;
  GeometryVertexHandle vtx[10];
  GeometryLineHandle line[11];

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


  line[0] = viennagrid::make_line( geometry, vtx[0], vtx[1] );

  line[1] = viennagrid::make_line( geometry, vtx[0], vtx[2] );
  line[2] = viennagrid::make_line( geometry, vtx[1], vtx[3] );

  line[3] = viennagrid::make_line( geometry, vtx[2], vtx[3] );

  line[4] = viennagrid::make_line( geometry, vtx[2], vtx[4] );
  line[5] = viennagrid::make_line( geometry, vtx[3], vtx[5] );

  line[6] = viennagrid::make_line( geometry, vtx[4], vtx[5] );

  line[7] = viennagrid::make_line( geometry, vtx[6], vtx[7] );

  line[8] = viennagrid::make_line( geometry, vtx[6], vtx[8] );
  line[9] = viennagrid::make_line( geometry, vtx[7], vtx[9] );

  line[10] = viennagrid::make_line( geometry, vtx[8], vtx[9] );


  // creating a geometry segment for each segment in the mesh
  GeometrySegmentHandleType segment0 = segmentation.make_segment();
  viennagrid::add( segment0, line[0] );
  viennagrid::add( segment0, line[1] );
  viennagrid::add( segment0, line[2] );
  viennagrid::add( segment0, line[3] );

  viennagrid::add( segment0, line[7] );
  viennagrid::add( segment0, line[8] );
  viennagrid::add( segment0, line[9] );
  viennagrid::add( segment0, line[10] );

  GeometrySegmentHandleType segment1 = segmentation.make_segment();
  viennagrid::add( segment1, line[3] );
  viennagrid::add( segment1, line[4] );
  viennagrid::add( segment1, line[5] );
  viennagrid::add( segment1, line[6] );



  // creating an algorithm using the Tetgen meshing library for meshing a hull
  viennamesh::algorithm_handle mesher( new viennamesh::triangle::make_mesh() );

  // setting the created line geometry as input for the mesher
  mesher->set_input( "mesh", geometry_handle );

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
