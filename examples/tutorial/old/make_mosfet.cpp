#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/mesh/element_creation.hpp"
#include "viennagrid/io/vtk_writer.hpp"

int main(int argc, char* argv[])
{
  // Typedefing the mesh type representing the 2D geometry; using just lines
  typedef viennagrid::line_2d_mesh GeometryMeshType;
  typedef viennagrid::result_of::segmentation<GeometryMeshType>::type GeometrySegmentationType;
  typedef viennagrid::result_of::segment_handle<GeometrySegmentationType>::type GeometrySegmentHandleType;

  // Typedefing vertex handle, line handle and point type for geometry creation
  typedef viennagrid::result_of::point<GeometryMeshType>::type PointType;
  typedef viennagrid::result_of::vertex_handle<GeometryMeshType>::type GeometryVertexHandle;
  typedef viennagrid::result_of::line_handle<GeometryMeshType>::type GeometryLineHandle;

  // creating the geometry mesh and segmentation
  GeometryMeshType geometry;
  GeometrySegmentationType segmentation(geometry);


  GeometrySegmentHandleType gate_contact = segmentation.make_segment();     // 0
  GeometrySegmentHandleType source_contact = segmentation.make_segment();   // 1
  GeometrySegmentHandleType gate_oxide = segmentation.make_segment();       // 2
  GeometrySegmentHandleType drain_contact = segmentation.make_segment();    // 3
  GeometrySegmentHandleType source = segmentation.make_segment();           // 4
  GeometrySegmentHandleType drain = segmentation.make_segment();            // 5
  GeometrySegmentHandleType body = segmentation.make_segment();             // 6
  GeometrySegmentHandleType body_contact = segmentation.make_segment();     // 7


  double gate_length = atof(argv[1]);
  double oxide_thickness = atof(argv[2]);

  double hgl = gate_length/2.0;
  GeometryVertexHandle vtx[24];

  vtx[ 0] = viennagrid::make_vertex( geometry, PointType(-hgl-4.0, oxide_thickness+5.0) );        // -15 10
  vtx[ 1] = viennagrid::make_vertex( geometry, PointType( hgl+4.0, oxide_thickness+5.0) );        //  15 10
  vtx[ 2] = viennagrid::make_vertex( geometry, PointType(-hgl-29.0, 5.0) );    // -40  5
  vtx[ 3] = viennagrid::make_vertex( geometry, PointType(-hgl-19.0, 5.0) );    // -30  5
  vtx[ 4] = viennagrid::make_vertex( geometry, PointType(-hgl-4.0, oxide_thickness) );         // -15  5
  vtx[ 5] = viennagrid::make_vertex( geometry, PointType( hgl+4.0, oxide_thickness) );         //  15  5
  vtx[ 6] = viennagrid::make_vertex( geometry, PointType( hgl+19.0, 5.0) );    //  30  5
  vtx[ 7] = viennagrid::make_vertex( geometry, PointType( hgl+29.0, 5.0) );    //  40  5
  vtx[ 8] = viennagrid::make_vertex( geometry, PointType(-hgl-29.0, 0.0) );    // -40  0
  vtx[ 9] = viennagrid::make_vertex( geometry, PointType(-hgl-19.0, 0.0) );    // -30  0
  vtx[10] = viennagrid::make_vertex( geometry, PointType(-hgl-4.0, 0.0) );    // -15  0
  vtx[11] = viennagrid::make_vertex( geometry, PointType(-hgl, 0.0) );    // -11  0
  vtx[12] = viennagrid::make_vertex( geometry, PointType( hgl, 0.0) );    // 11  0
  vtx[13] = viennagrid::make_vertex( geometry, PointType( hgl+4.0, 0.0) );    // 15  0
  vtx[14] = viennagrid::make_vertex( geometry, PointType( hgl+19.0, 0.0) );    // 30  0
  vtx[15] = viennagrid::make_vertex( geometry, PointType( hgl+29.0, 0.0) );    // 40  0
  vtx[16] = viennagrid::make_vertex( geometry, PointType(-hgl, -40.0) );    // -11  -40
  vtx[17] = viennagrid::make_vertex( geometry, PointType( hgl, -40.0) );    // 11  -40
  vtx[18] = viennagrid::make_vertex( geometry, PointType(-hgl-29.0, -60.0) );    // -40  -60
  vtx[19] = viennagrid::make_vertex( geometry, PointType( hgl+29.0, -60.0) );    //  40  -60
  vtx[20] = viennagrid::make_vertex( geometry, PointType(-hgl-29.0, -90.0) );    // -40  -90
  vtx[21] = viennagrid::make_vertex( geometry, PointType( hgl+29.0, -90.0) );    //  40  -90
  vtx[22] = viennagrid::make_vertex( geometry, PointType(-hgl-29.0, -100.0) );    // -40  -100
  vtx[23] = viennagrid::make_vertex( geometry, PointType( hgl+29.0, -100.0) );    //  40  -100

  GeometryLineHandle line;

  line = viennagrid::make_line(geometry, vtx[0], vtx[4]);
  viennagrid::add(gate_contact, line);

  line = viennagrid::make_line(geometry, vtx[4], vtx[5]);
  viennagrid::add(gate_contact, line);
  viennagrid::add(gate_oxide, line);

  line = viennagrid::make_line(geometry, vtx[5], vtx[1]);
  viennagrid::add(gate_contact, line);

  line = viennagrid::make_line(geometry, vtx[1], vtx[0]);
  viennagrid::add(gate_contact, line);



  line = viennagrid::make_line(geometry, vtx[2], vtx[8]);
  viennagrid::add(source_contact, line);

  line = viennagrid::make_line(geometry, vtx[8], vtx[9]);
  viennagrid::add(source_contact, line);
  viennagrid::add(source, line);

  line = viennagrid::make_line(geometry, vtx[9], vtx[3]);
  viennagrid::add(source_contact, line);

  line = viennagrid::make_line(geometry, vtx[3], vtx[2]);
  viennagrid::add(source_contact, line);



  line = viennagrid::make_line(geometry, vtx[4], vtx[10]);
  viennagrid::add(gate_oxide, line);

  line = viennagrid::make_line(geometry, vtx[10], vtx[11]);
  viennagrid::add(gate_oxide, line);
  viennagrid::add(source, line);

  line = viennagrid::make_line(geometry, vtx[11], vtx[12]);
  viennagrid::add(gate_oxide, line);
  viennagrid::add(body, line);

  line = viennagrid::make_line(geometry, vtx[12], vtx[13]);
  viennagrid::add(gate_oxide, line);
  viennagrid::add(drain, line);

  line = viennagrid::make_line(geometry, vtx[13], vtx[5]);
  viennagrid::add(gate_oxide, line);



  line = viennagrid::make_line(geometry, vtx[6], vtx[14]);
  viennagrid::add(drain_contact, line);

  line = viennagrid::make_line(geometry, vtx[14], vtx[15]);
  viennagrid::add(drain_contact, line);
  viennagrid::add(drain, line);

  line = viennagrid::make_line(geometry, vtx[15], vtx[7]);
  viennagrid::add(drain_contact, line);

  line = viennagrid::make_line(geometry, vtx[7], vtx[6]);
  viennagrid::add(drain_contact, line);



  line = viennagrid::make_line(geometry, vtx[8], vtx[18]);
  viennagrid::add(source, line);

  line = viennagrid::make_line(geometry, vtx[18], vtx[16]);
  viennagrid::add(source, line);
  viennagrid::add(body, line);

  line = viennagrid::make_line(geometry, vtx[16], vtx[11]);
  viennagrid::add(source, line);
  viennagrid::add(body, line);

  line = viennagrid::make_line(geometry, vtx[10], vtx[9]);
  viennagrid::add(source, line);



  line = viennagrid::make_line(geometry, vtx[12], vtx[17]);
  viennagrid::add(drain, line);
  viennagrid::add(body, line);

  line = viennagrid::make_line(geometry, vtx[17], vtx[19]);
  viennagrid::add(drain, line);
  viennagrid::add(body, line);

  line = viennagrid::make_line(geometry, vtx[19], vtx[15]);
  viennagrid::add(drain, line);

  line = viennagrid::make_line(geometry, vtx[14], vtx[13]);
  viennagrid::add(drain, line);



  line = viennagrid::make_line(geometry, vtx[18], vtx[20]);
  viennagrid::add(body, line);

  line = viennagrid::make_line(geometry, vtx[20], vtx[21]);
  viennagrid::add(body, line);
  viennagrid::add(body_contact, line);

  line = viennagrid::make_line(geometry, vtx[21], vtx[19]);
  viennagrid::add(body, line);



  line = viennagrid::make_line(geometry, vtx[20], vtx[22]);
  viennagrid::add(body_contact, line);

  line = viennagrid::make_line(geometry, vtx[22], vtx[23]);
  viennagrid::add(body_contact, line);

  line = viennagrid::make_line(geometry, vtx[23], vtx[21]);
  viennagrid::add(body_contact, line);


  {
    viennagrid::io::vtk_writer<GeometryMeshType, GeometrySegmentationType> writer;
    writer(geometry, segmentation, argv[3]);
  }



//   // creating an algorithm using the Tetgen meshing library for meshing a hull
//   viennamesh::algorithm_handle mesher( new viennamesh::triangle::algorithm() );
//
//   // setting the created line geometry as input for the mesher
//   mesher->set_input( "default", geometry_handle );
//
//   // setting the mesher paramters
//   mesher->set_input( "cell_size", 1.0 );      // maximum cell size
//   mesher->set_input( "min_angle", 0.35 );     // minimum angle in radiant, 0.35 are about 20 degrees
//   mesher->set_input( "delaunay", true  );     // we want a Delaunay triangulation
//   mesher->set_input( "algorithm_type", "incremental_delaunay" );  // incremental Delaunay algorithm is used
//
//   // start the algorithm
//   mesher->run();
//
//
//   // creating an algorithm for writing a mesh to a file
//   viennamesh::algorithm_handle writer( new viennamesh::io::mesh_writer() );
//
//   // linking the output from the mesher to the writer
//   writer->set_input( "default", mesher->get_output("default") );
//   // Setting the filename for the reader and writer
//   writer->set_input( "filename", "mosfet2d.vtu" );
//
//   // start the algorithm
//   writer->run();
}
