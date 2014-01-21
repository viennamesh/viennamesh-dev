#include "viennamesh/algorithm/cgal.hpp"
#include "viennamesh/algorithm/io.hpp"
#include "viennamesh/utils/memory_capture.hpp"





struct times_t
{
  times_t() : convert_geometry_to_cgal_time(0), meshing_time(0), convert_mesh_to_viennagrid_time(0) {}

  double convert_geometry_to_cgal_time;
  double meshing_time;
  double convert_mesh_to_viennagrid_time;
};


struct memory_t
{
  memory_t() : viennagrid_geometry_size(0), cgal_geometry_size(0), viennagrid_mesh_size(0), cgal_mesh_size(0), num_vertices(0), num_cells(0) {}

  int64_t viennagrid_geometry_size;
  int64_t cgal_geometry_size;

  int64_t viennagrid_mesh_size;
  int64_t cgal_mesh_size;

  int num_vertices;
  int num_cells;
};




template<bool benchmarking_memory>
void generic_benchmark( double size, times_t & times, memory_t & memory )
{
  viennautils::Timer timer;
  viennautils::memory_capture memory_capture(benchmarking_memory);


  double s = 10.0;

  // creating an algorithm using the Tetgen meshing library for meshing a hull
  viennamesh::algorithm_handle mesher( new viennamesh::cgal::algorithm() );

  // Typedefing the mesh type representing the 3D geometry; using triangles
  typedef viennagrid::triangular_3d_mesh GeometryMeshType;

  // Typedefing vertex handle, line handle and point type for geometry creation
  typedef viennagrid::result_of::point<GeometryMeshType>::type PointType;
  typedef viennagrid::result_of::vertex_handle<GeometryMeshType>::type GeometryVertexHandle;

  // creating the geometry mesh and segmentation
  viennamesh::result_of::parameter_handle< GeometryMeshType >::type geometry_handle = viennamesh::make_parameter<GeometryMeshType>();
  GeometryMeshType & geometry = geometry_handle();

  // setting the created line geometry as input for the mesher
  mesher->set_input( "default", geometry_handle );

  // setting the mesher paramters
  mesher->set_input( "cell_size", size );      // maximum cell size
  mesher->set_input( "max_radius_edge_ratio", 3.0 );
  mesher->set_input( "min_facet_angle", 0.3 );

  int64_t memory_before_geometry = memory_capture.allocated_memory();

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



  int64_t memory_after_geometry_creation = memory_capture.allocated_memory();
  if (benchmarking_memory)
    memory.viennagrid_geometry_size += (memory_after_geometry_creation - memory_before_geometry);

  timer.start();
  viennamesh::parameter_handle converted_geometry = geometry_handle->get_converted< viennamesh::cgal::input_mesh >();
  if (!benchmarking_memory)
    times.convert_geometry_to_cgal_time += timer.get();

  int64_t memory_after_convert_to_triangle = memory_capture.allocated_memory();
  if (benchmarking_memory)
    memory.cgal_geometry_size += (memory_after_convert_to_triangle-memory_after_geometry_creation);


  // start the algorithms
  timer.start();
  mesher->run();
  if (!benchmarking_memory)
    times.meshing_time += timer.get();

  int64_t memory_after_meshing = memory_capture.allocated_memory();
  if (benchmarking_memory)
    memory.cgal_mesh_size += (memory_after_meshing-memory_after_convert_to_triangle);


//   typedef viennagrid::thin_tetrahedral_3d_mesh MeshType;
  typedef viennagrid::tetrahedral_3d_mesh MeshType;

  timer.start();
  viennamesh::result_of::parameter_handle<MeshType>::type converted_mesh = mesher->get_output("default")->get_converted< MeshType >();
  if (!benchmarking_memory)
    times.convert_mesh_to_viennagrid_time += timer.get();

  viennagrid::detail::create_coboundary_information<viennagrid::vertex_tag, viennagrid::line_tag>( converted_mesh() );
  viennagrid::detail::create_coboundary_information<viennagrid::vertex_tag, viennagrid::triangle_tag>( converted_mesh() );
  viennagrid::detail::create_coboundary_information<viennagrid::vertex_tag, viennagrid::tetrahedron_tag>( converted_mesh() );

  viennagrid::detail::create_coboundary_information<viennagrid::line_tag, viennagrid::triangle_tag>( converted_mesh() );
  viennagrid::detail::create_coboundary_information<viennagrid::line_tag, viennagrid::tetrahedron_tag>( converted_mesh() );

  viennagrid::detail::create_coboundary_information<viennagrid::triangle_tag, viennagrid::tetrahedron_tag>( converted_mesh() );



  int64_t memory_after_convert = memory_capture.allocated_memory();
  if (benchmarking_memory)
    memory.viennagrid_mesh_size += (memory_after_convert-memory_after_meshing);

  if (benchmarking_memory)
  {
    memory.num_vertices += viennagrid::vertices(converted_mesh()).size();
    memory.num_cells += viennagrid::cells(converted_mesh()).size();
  }
}


void benchmark( double size, int count )
{
  times_t times;
  memory_t memory;

  for (int i = 0; i < count; ++i)
    generic_benchmark<false>(size, times, memory);

  generic_benchmark<true>(size, times, memory);

  times.convert_geometry_to_cgal_time /= count;
  times.meshing_time /= count;
  times.convert_mesh_to_viennagrid_time /= count;

  std::cout << "Num Vertices: " << memory.num_vertices << std::endl;
  std::cout << "Num Cells:    " << memory.num_cells << std::endl;
  std::cout << std::endl;

  std::cout << "Convert Geometry to CGAL Time: " << times.convert_geometry_to_cgal_time << " (" << times.convert_geometry_to_cgal_time / times.meshing_time << ")"<< std::endl;
  std::cout << "Meshing Time:                      " << times.meshing_time << std::endl;
  std::cout << "Convert Mesh to ViennaGrid Time:   " << times.convert_mesh_to_viennagrid_time << " (" << times.convert_mesh_to_viennagrid_time / times.meshing_time << ")"<< std::endl;
  std::cout << std::endl;

  std::cout << "Memory Footprint ViennaGrid Geometry : " << memory.viennagrid_geometry_size << " (" << static_cast<double>(memory.viennagrid_geometry_size) / static_cast<double>(memory.cgal_geometry_size) << ")" << std::endl;
  std::cout << "Memory Footprint CGAL Geometry :   " << memory.cgal_geometry_size << std::endl;
  std::cout << "Memory Footprint ViennaGrid Mesh :     " << memory.viennagrid_mesh_size << " (" << static_cast<double>(memory.viennagrid_mesh_size) / static_cast<double>(memory.cgal_mesh_size) << ")" << std::endl;
  std::cout << "Memory Footprint CGAL Mesh :       " << memory.cgal_mesh_size << std::endl;
}






int main()
{
  viennamesh::logger().set_all_log_level(0);

  const int num_benchmark_iteration = 1;
  double s;

  {
    s = 1.0;
    for (int i = 0; i < 4; ++i, s /= 2.0)
    {
      std::cout << "s = " << s << std::endl;
      benchmark(s, num_benchmark_iteration);
      std::cout << std::endl;
    }
  }

}
