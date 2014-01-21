#include "viennamesh/algorithm/triangle.hpp"
#include "viennamesh/algorithm/io.hpp"
#include "viennamesh/utils/memory_capture.hpp"





struct times_t
{
  times_t() : convert_geometry_to_triangle_time(0), meshing_time(0), convert_mesh_to_viennagrid_time(0) {}

  double convert_geometry_to_triangle_time;
  double meshing_time;
  double convert_mesh_to_viennagrid_time;
};


struct memory_t
{
  memory_t() : viennagrid_geometry_size(0), triangle_geometry_size(0), viennagrid_mesh_size(0), triangle_mesh_size(0), num_vertices(0), num_cells(0) {}

  int64_t viennagrid_geometry_size;
  int64_t triangle_geometry_size;

  int64_t viennagrid_mesh_size;
  int64_t triangle_mesh_size;

  int num_vertices;
  int num_cells;
};




template<bool benchmarking_memory>
void generic_benchmark( double size, double min_angle, times_t & times, memory_t & memory )
{
  viennautils::Timer timer;
  viennautils::memory_capture memory_capture(benchmarking_memory);


  double s = 10.0;

  // creating an algorithm using the Tetgen meshing library for meshing a hull
  viennamesh::algorithm_handle mesher( new viennamesh::triangle::algorithm() );

  // Typedefing the mesh type representing the 2D geometry; using just lines
  typedef viennagrid::line_2d_mesh GeometryMeshType;
  typedef viennagrid::result_of::segmentation<GeometryMeshType>::type GeometrySegmentationType;
  typedef viennagrid::result_of::segment_handle<GeometrySegmentationType>::type GeometrySegmentHandleType;
  typedef viennagrid::segmented_mesh<GeometryMeshType, GeometrySegmentationType> SegmentedGeometryMeshType;

  // Typedefing vertex handle, line handle and point type for geometry creation
  typedef viennagrid::result_of::point<GeometryMeshType>::type PointType;
  typedef viennagrid::result_of::vertex_handle<GeometryMeshType>::type GeometryVertexHandle;
  typedef viennagrid::result_of::line_handle<GeometryMeshType>::type GeometryLineHandle;

  typedef viennagrid::result_of::vertex<GeometryMeshType>::type VertexType;
  typedef viennagrid::result_of::cell<GeometryMeshType>::type CellType;

  // creating the geometry mesh and segmentation
  viennamesh::result_of::parameter_handle< SegmentedGeometryMeshType >::type geometry_handle = viennamesh::make_parameter<SegmentedGeometryMeshType>();
  GeometryMeshType & geometry = geometry_handle().mesh;
  GeometrySegmentationType & segmentation = geometry_handle().segmentation;

  // setting the created line geometry as input for the mesher
  mesher->set_input( "default", geometry_handle );

  // creating the hole points and set it as input for the mesher
  viennamesh::point_2d_container hole_points;
  hole_points.push_back( PointType(s/2, s/2) );
  mesher->set_input( "hole_points", hole_points );

  // setting the mesher paramters
  mesher->set_input( "cell_size", size );      // maximum cell size
  mesher->set_input( "min_angle", min_angle );     // minimum angle in radiant, 0.35 are about 20 degrees
  mesher->set_input( "delaunay", true  );     // we want a Delaunay triangulation
  mesher->set_input( "algorithm_type", "incremental_delaunay" );  // incremental Delaunay algorithm is used


  int64_t memory_before_geometry = memory_capture.allocated_memory();



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

  int64_t memory_after_geometry_creation = memory_capture.allocated_memory();
  if (benchmarking_memory)
    memory.viennagrid_geometry_size += (memory_after_geometry_creation - memory_before_geometry);

  timer.start();
  viennamesh::parameter_handle converted_geometry = geometry_handle->get_converted< viennagrid::segmented_mesh<viennamesh::triangle::input_mesh, viennamesh::triangle::input_segmentation> >();
  if (!benchmarking_memory)
    times.convert_geometry_to_triangle_time += timer.get();

  int64_t memory_after_convert_to_triangle = memory_capture.allocated_memory();
  if (benchmarking_memory)
    memory.triangle_geometry_size += (memory_after_convert_to_triangle-memory_after_geometry_creation);


  // start the algorithms
  timer.start();
  mesher->run();
  if (!benchmarking_memory)
    times.meshing_time += timer.get();

  int64_t memory_after_meshing = memory_capture.allocated_memory();
  if (benchmarking_memory)
    memory.triangle_mesh_size += (memory_after_meshing-memory_after_convert_to_triangle);


  typedef viennagrid::segmented_mesh<viennagrid::thin_triangular_2d_mesh, viennagrid::thin_cell_only_triangular_2d_segmentation> MeshType;

  timer.start();
  viennamesh::result_of::parameter_handle<MeshType>::type converted_mesh = mesher->get_output("default")->get_converted< MeshType >();
  if (!benchmarking_memory)
    times.convert_mesh_to_viennagrid_time += timer.get();

  int64_t memory_after_convert = memory_capture.allocated_memory();
  if (benchmarking_memory)
    memory.viennagrid_mesh_size += (memory_after_convert-memory_after_meshing);

  if (benchmarking_memory)
  {
    memory.num_vertices += viennagrid::vertices(converted_mesh().mesh).size();
    memory.num_cells += viennagrid::cells(converted_mesh().mesh).size();
  }
}


void benchmark( double size, double min_angle, int count )
{
  times_t times;
  memory_t memory;

  for (int i = 0; i < count; ++i)
    generic_benchmark<false>(size, min_angle, times, memory);

  generic_benchmark<true>(size, min_angle, times, memory);

  times.convert_geometry_to_triangle_time /= count;
  times.meshing_time /= count;
  times.convert_mesh_to_viennagrid_time /= count;

  std::cout << "Num Vertices: " << memory.num_vertices << std::endl;
  std::cout << "Num Cells:    " << memory.num_cells << std::endl;
  std::cout << std::endl;

  std::cout << "Convert Geometry to Triangle Time: " << times.convert_geometry_to_triangle_time << " (" << times.convert_geometry_to_triangle_time / times.meshing_time << ")"<< std::endl;
  std::cout << "Meshing Time:                      " << times.meshing_time << std::endl;
  std::cout << "Convert Mesh to ViennaGrid Time:   " << times.convert_mesh_to_viennagrid_time << " (" << times.convert_mesh_to_viennagrid_time / times.meshing_time << ")"<< std::endl;
  std::cout << std::endl;

  std::cout << "Memory Footprint ViennaGrid Geometry : " << memory.viennagrid_geometry_size << " (" << static_cast<double>(memory.viennagrid_geometry_size) / static_cast<double>(memory.triangle_geometry_size) << ")" << std::endl;
  std::cout << "Memory Footprint Triangle Geometry :   " << memory.triangle_geometry_size << std::endl;
  std::cout << "Memory Footprint ViennaGrid Mesh :     " << memory.viennagrid_mesh_size << " (" << static_cast<double>(memory.viennagrid_mesh_size) / static_cast<double>(memory.triangle_mesh_size) << ")" << std::endl;
  std::cout << "Memory Footprint Triangle Mesh :       " << memory.triangle_mesh_size << std::endl;
}






int main()
{
  viennamesh::logger().set_all_log_level(0);

  const int num_benchmark_iteration = 10;
  double s;

  {
    std::cout << "min_angle = 0.2" << std::endl;
    s = 1.0;
    for (int i = 0; i < 4; ++i, s /= 10.0)
    {
      std::cout << "s = " << s << std::endl;
      benchmark(s, 0.2, num_benchmark_iteration);
      std::cout << std::endl;
    }
  }

  {
    std::cout << "min_angle = 0.3" << std::endl;
    s = 1.0;
    for (int i = 0; i < 4; ++i, s /= 10.0)
    {
      std::cout << "s = " << s << std::endl;
      benchmark(s, 0.3, num_benchmark_iteration);
      std::cout << std::endl;
    }
  }


  {
    std::cout << "min_angle = 0.4" << std::endl;
    s = 1.0;
    for (int i = 0; i < 4; ++i, s /= 10.0)
    {
      std::cout << "s = " << s << std::endl;
      benchmark(s, 0.4, num_benchmark_iteration);
      std::cout << std::endl;
    }
  }

  {
    std::cout << "min_angle = 0.5" << std::endl;
    s = 1.0;
    for (int i = 0; i < 4; ++i, s /= 10.0)
    {
      std::cout << "s = " << s << std::endl;
      benchmark(s, 0.5, num_benchmark_iteration);
      std::cout << std::endl;
    }
  }

}
