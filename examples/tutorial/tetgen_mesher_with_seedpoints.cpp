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
  typedef viennagrid::plc_3d_mesh GeometryMeshType;

  typedef viennagrid::result_of::point<GeometryMeshType>::type PointType;
  typedef viennagrid::result_of::vertex_handle<GeometryMeshType>::type GeometryVertexHandle;
  typedef viennagrid::result_of::line_handle<GeometryMeshType>::type GeometryLineHandle;

  // creating the geometry mesh
  viennamesh::result_of::parameter_handle< GeometryMeshType >::type geometry_handle = viennamesh::make_parameter<GeometryMeshType>();
  GeometryMeshType & geometry = geometry_handle();


  double s = 10.0;
  GeometryVertexHandle vtx[8];
  GeometryLineHandle lines[12];

  vtx[0] = viennagrid::make_vertex( geometry, PointType(0, 0, 0) );
  vtx[1] = viennagrid::make_vertex( geometry, PointType(0, s, 0) );
  vtx[2] = viennagrid::make_vertex( geometry, PointType(s, 0, 0) );
  vtx[3] = viennagrid::make_vertex( geometry, PointType(s, s, 0) );

  vtx[4] = viennagrid::make_vertex( geometry, PointType(0, 0, s) );
  vtx[5] = viennagrid::make_vertex( geometry, PointType(0, s, s) );
  vtx[6] = viennagrid::make_vertex( geometry, PointType(s, 0, s) );
  vtx[7] = viennagrid::make_vertex( geometry, PointType(s, s, s) );


  // bottom 4 lines
  lines[0] = viennagrid::make_line( geometry, vtx[0], vtx[1] );
  lines[1] = viennagrid::make_line( geometry, vtx[1], vtx[3] );
  lines[2] = viennagrid::make_line( geometry, vtx[3], vtx[2] );
  lines[3] = viennagrid::make_line( geometry, vtx[2], vtx[0] );

  // top 4 lines
  lines[4] = viennagrid::make_line( geometry, vtx[4], vtx[5] );
  lines[5] = viennagrid::make_line( geometry, vtx[5], vtx[7] );
  lines[6] = viennagrid::make_line( geometry, vtx[7], vtx[6] );
  lines[7] = viennagrid::make_line( geometry, vtx[6], vtx[4] );

  // columns
  lines[8] = viennagrid::make_line( geometry, vtx[0], vtx[4] );
  lines[9] = viennagrid::make_line( geometry, vtx[1], vtx[5] );
  lines[10] = viennagrid::make_line( geometry, vtx[2], vtx[6] );
  lines[11] = viennagrid::make_line( geometry, vtx[3], vtx[7] );



  viennagrid::make_plc( geometry, lines+0, lines+4 );
  viennagrid::make_plc( geometry, lines+4, lines+8 );

  {
    GeometryLineHandle cur_lines[4];
    cur_lines[0] = lines[0];
    cur_lines[1] = lines[9];
    cur_lines[2] = lines[4];
    cur_lines[3] = lines[8];
    viennagrid::make_plc( geometry, cur_lines+0, cur_lines+4 );
  }

  {
    GeometryLineHandle cur_lines[4];
    cur_lines[0] = lines[2];
    cur_lines[1] = lines[11];
    cur_lines[2] = lines[6];
    cur_lines[3] = lines[10];
    viennagrid::make_plc( geometry, cur_lines+0, cur_lines+4 );
  }

  {
    GeometryLineHandle cur_lines[4];
    cur_lines[0] = lines[3];
    cur_lines[1] = lines[10];
    cur_lines[2] = lines[7];
    cur_lines[3] = lines[8];
    viennagrid::make_plc( geometry, cur_lines+0, cur_lines+4 );
  }

  {
    GeometryLineHandle cur_lines[4];
    cur_lines[0] = lines[1];
    cur_lines[1] = lines[11];
    cur_lines[2] = lines[5];
    cur_lines[3] = lines[9];
    viennagrid::make_plc( geometry, cur_lines+0, cur_lines+4 );
  }


  // creating an algorithm using the Tetgen meshing library for meshing a hull
  viennamesh::algorithm_handle mesher( new viennamesh::tetgen::make_mesh() );

  // set the input geometry and seed points
  mesher->set_input( "mesh", geometry_handle );
  viennamesh::seed_point_3d_container seed_points;
  seed_points.push_back( std::make_pair( PointType(s/2, s/2, s/2), 0 ) );

  mesher->set_input( "seed_points", seed_points );


  // setting the mesher paramters
  mesher->set_input( "cell_size", 1.0 );              // maximum cell size
  mesher->set_input( "max_radius_edge_ratio", 1.5 );  // maximum radius edge ratio
  mesher->set_input( "min_dihedral_angle", 0.17 );     // minimum dihedral angle in radiant, 0.17 are about 10 degrees

  // start the algorithm
  mesher->run();


  // creating an algorithm for writing a mesh to a file
  viennamesh::algorithm_handle writer( new viennamesh::io::mesh_writer() );

  // linking the output from the mesher to the writer
  writer->set_default_source(mesher);

  // Setting the filename for the reader and writer
  writer->set_input( "filename", "one_cube.vtu" );

  // start the algorithm
  writer->run();


}
