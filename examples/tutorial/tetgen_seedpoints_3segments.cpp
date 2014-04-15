#include "viennamesh/viennamesh.hpp"



// This method helps us to create or fetch a line based on vertex handles
// if a line is already present, we just use that line, otherwise we create it
// We are using a std::map< std::pair<VertexID, VertexID>, LineHandle > for keeping track of all lines in the mesh
template<typename MeshT, typename VertexHandleT, typename VertexLineMapT>
typename viennagrid::result_of::line_handle<MeshT>::type get_make_line(MeshT & mesh, VertexLineMapT & vertex_line_map, VertexHandleT const & vtxh0, VertexHandleT const & vtxh1)
{
  // querying the ID of the vertices
  typedef typename viennagrid::result_of::vertex_id<MeshT>::type VertexIDType;
  VertexIDType id0 = viennagrid::dereference_handle(mesh, vtxh0).id();
  VertexIDType id1 = viennagrid::dereference_handle(mesh, vtxh1).id();

  // creating the key for the line, note that we order the IDs within the pair for uniqueness
  std::pair<VertexIDType, VertexIDType> key = std::make_pair( std::min(id0, id1), std::max(id0, id1) );

  // searching for the line in our map
  typename VertexLineMapT::iterator it = vertex_line_map.find(key);
  if (it != vertex_line_map.end())
  {
    // found -> return
    return it->second;
  }
  else
  {
    // not found -> create the line
    typename viennagrid::result_of::line_handle<MeshT>::type tmp = viennagrid::make_line( mesh, vtxh0, vtxh1 );
    vertex_line_map[ key ] = tmp;
    return tmp;
  }
}


// This method helps us creating a quad PLC based on 4 vertex handles
// we use get_make_line (see above) and a vertex IDs to line map for keeping track of all lines
template<typename MeshT, typename VertexLineMapT, typename VertexHandleT>
typename viennagrid::result_of::plc_handle<MeshT>::type make_quad_plc(MeshT & mesh, VertexLineMapT & vertex_line_map, VertexHandleT v0, VertexHandleT v1, VertexHandleT v2, VertexHandleT v3)
{
  typedef typename viennagrid::result_of::line_handle<MeshT>::type LineHandleType;
  viennagrid::static_array<LineHandleType, 4> lines;
  lines[0] = get_make_line(mesh, vertex_line_map, v0, v1);
  lines[1] = get_make_line(mesh, vertex_line_map, v1, v2);
  lines[2] = get_make_line(mesh, vertex_line_map, v2, v3);
  lines[3] = get_make_line(mesh, vertex_line_map, v3, v0);

  return viennagrid::make_plc(mesh, lines.begin(), lines.end());
}

int main()
{
  // typedefing some needed type for creating the geometry
  typedef viennagrid::brep_3d_mesh GeometryMeshType;
  typedef viennagrid::result_of::segmentation<GeometryMeshType>::type GeometrySegmentationType;
  typedef viennagrid::result_of::segment_handle<GeometrySegmentationType>::type GeometrySegmentHandleType;
  typedef viennagrid::segmented_mesh<GeometryMeshType, GeometrySegmentationType> SegmentedGeometryMeshType;

  typedef viennagrid::result_of::point<GeometryMeshType>::type PointType;
  typedef viennagrid::result_of::vertex_handle<GeometryMeshType>::type GeometryVertexHandleType;
  typedef viennagrid::result_of::vertex_id<GeometryMeshType>::type GeometryVertexIDType;
  typedef viennagrid::result_of::line_handle<GeometryMeshType>::type GeometryLineHandleType;
  typedef viennagrid::result_of::plc_handle<GeometryMeshType>::type GeometryPLCHandleType;

  // creating the geometry mesh
  viennamesh::result_of::parameter_handle< SegmentedGeometryMeshType >::type geometry_handle = viennamesh::make_parameter<SegmentedGeometryMeshType>();
  GeometryMeshType & geometry = geometry_handle().mesh;
  GeometrySegmentationType & segmentation = geometry_handle().segmentation;


  // creating a geometry with 3 cubes next to each other:
  // +-----+-----+-----+
  // |     |     |     |
  // |  1  |  2  |  3  |
  // |     |     |     |
  // +-----+-----+-----+
  double s = 10.0;
  viennagrid::static_array<GeometryVertexHandleType, 16> vtx;

  // creating all points
  vtx[0] = viennagrid::make_vertex( geometry, PointType(0, 0, 0) );
  vtx[1] = viennagrid::make_vertex( geometry, PointType(0, s, 0) );
  vtx[2] = viennagrid::make_vertex( geometry, PointType(s, 0, 0) );
  vtx[3] = viennagrid::make_vertex( geometry, PointType(s, s, 0) );

  vtx[4] = viennagrid::make_vertex( geometry, PointType(0, 0, s) );
  vtx[5] = viennagrid::make_vertex( geometry, PointType(0, s, s) );
  vtx[6] = viennagrid::make_vertex( geometry, PointType(s, 0, s) );
  vtx[7] = viennagrid::make_vertex( geometry, PointType(s, s, s) );

  vtx[8] = viennagrid::make_vertex( geometry, PointType(0, 0, 2*s) );
  vtx[9] = viennagrid::make_vertex( geometry, PointType(0, s, 2*s) );
  vtx[10] = viennagrid::make_vertex( geometry, PointType(s, 0, 2*s) );
  vtx[11] = viennagrid::make_vertex( geometry, PointType(s, s, 2*s) );

  vtx[12] = viennagrid::make_vertex( geometry, PointType(0, 0, 3*s) );
  vtx[13] = viennagrid::make_vertex( geometry, PointType(0, s, 3*s) );
  vtx[14] = viennagrid::make_vertex( geometry, PointType(s, 0, 3*s) );
  vtx[15] = viennagrid::make_vertex( geometry, PointType(s, s, 3*s) );



  // We are using this map to keep track of all lines we use, this method is used by get_make_line which is used by make_quad_plc
  std::map< std::pair<GeometryVertexIDType, GeometryVertexIDType>, GeometryLineHandleType > vertex_line_map;
  viennagrid::static_array<GeometryPLCHandleType, 16> plcs;

  // the top quad of the first cube
  plcs[0] = make_quad_plc( geometry, vertex_line_map, vtx[0], vtx[1], vtx[3], vtx[2] );

  // the side quads of the first cube
  plcs[1] = make_quad_plc( geometry, vertex_line_map, vtx[0], vtx[4], vtx[5], vtx[1] );
  plcs[2] = make_quad_plc( geometry, vertex_line_map, vtx[2], vtx[6], vtx[7], vtx[3] );
  plcs[3] = make_quad_plc( geometry, vertex_line_map, vtx[0], vtx[4], vtx[6], vtx[2] );
  plcs[4] = make_quad_plc( geometry, vertex_line_map, vtx[1], vtx[5], vtx[7], vtx[3] );

  // the bottom quad of the first cube which is also the top quad of the second cube
  plcs[5] = make_quad_plc( geometry, vertex_line_map, vtx[4], vtx[5], vtx[7], vtx[6] );

  // the side quads of the second cube
  plcs[6] = make_quad_plc( geometry, vertex_line_map, vtx[4], vtx[8], vtx[9], vtx[5] );
  plcs[7] = make_quad_plc( geometry, vertex_line_map, vtx[6], vtx[10], vtx[11], vtx[7] );
  plcs[8] = make_quad_plc( geometry, vertex_line_map, vtx[4], vtx[8], vtx[10], vtx[6] );
  plcs[9] = make_quad_plc( geometry, vertex_line_map, vtx[5], vtx[9], vtx[11], vtx[7] );

  // the bottom quad of the second cube which is also the top quad of the third cube
  plcs[10] = make_quad_plc( geometry, vertex_line_map, vtx[8], vtx[9], vtx[11], vtx[10] );

  // the side quads of the third cube
  plcs[11] = make_quad_plc( geometry, vertex_line_map, vtx[8], vtx[12], vtx[13], vtx[9] );
  plcs[12] = make_quad_plc( geometry, vertex_line_map, vtx[10], vtx[14], vtx[15], vtx[11] );
  plcs[13] = make_quad_plc( geometry, vertex_line_map, vtx[8], vtx[12], vtx[14], vtx[10] );
  plcs[14] = make_quad_plc( geometry, vertex_line_map, vtx[9], vtx[13], vtx[15], vtx[11] );

  // the bottom quad of the third cube
  plcs[15] = make_quad_plc( geometry, vertex_line_map, vtx[12], vtx[13], vtx[15], vtx[14] );


  // creating segment for each part of the mesh using the boundary geometry for that segment
  GeometrySegmentHandleType segment0 = segmentation.make_segment();
  viennagrid::add( segment0, plcs[0] );
  viennagrid::add( segment0, plcs[1] );
  viennagrid::add( segment0, plcs[2] );
  viennagrid::add( segment0, plcs[3] );
  viennagrid::add( segment0, plcs[4] );
  viennagrid::add( segment0, plcs[5] );

  GeometrySegmentHandleType segment1 = segmentation.make_segment();
  viennagrid::add( segment1, plcs[5] );
  viennagrid::add( segment1, plcs[6] );
  viennagrid::add( segment1, plcs[7] );
  viennagrid::add( segment1, plcs[8] );
  viennagrid::add( segment1, plcs[9] );
  viennagrid::add( segment1, plcs[10] );

  GeometrySegmentHandleType segment2 = segmentation.make_segment();
  viennagrid::add( segment2, plcs[10] );
  viennagrid::add( segment2, plcs[11] );
  viennagrid::add( segment2, plcs[12] );
  viennagrid::add( segment2, plcs[13] );
  viennagrid::add( segment2, plcs[14] );
  viennagrid::add( segment2, plcs[15] );


  // creating an algorithm using the Tetgen meshing library for meshing a hull
  viennamesh::algorithm_handle mesher( new viennamesh::tetgen::mesh_generator() );

  // set the input geometry
  mesher->set_input( "mesh", geometry_handle );

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
  writer->set_input( "filename", "three_cubes.vtu" );

  // start the algorithm
  writer->run();
}
