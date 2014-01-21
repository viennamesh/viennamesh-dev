#include "viennamesh/algorithm/tetgen.hpp"
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


  viennamesh::algorithm_handle reader( new viennamesh::io::mesh_reader() );
  viennamesh::algorithm_handle mesher( new viennamesh::tetgen::algorithm() );

  reader->set_input( "filename", "../data/big_and_small_cube.poly" );

  // setting the mesher paramters
  mesher->link_input( "default", reader, "default" );
  mesher->set_input( "cell_size", size );      // maximum cell size
  mesher->set_input( "min_angle", min_angle );     // minimum angle in radiant, 0.35 are about 20 degrees
  mesher->set_input( "delaunay", true  );     // we want a Delaunay triangulation
  mesher->set_input( "algorithm_type", "incremental_delaunay" );  // incremental Delaunay algorithm is used

  int64_t memory_before_geometry = memory_capture.allocated_memory();
  reader->run();

  int64_t memory_after_geometry_creation = memory_capture.allocated_memory();
  if (benchmarking_memory)
    memory.viennagrid_geometry_size += (memory_after_geometry_creation - memory_before_geometry);

  timer.start();
  viennamesh::parameter_handle converted_geometry = reader->get_output("default")->get_converted< viennagrid::segmented_mesh<viennamesh::tetgen::input_mesh, viennamesh::tetgen::input_segmentation> >();
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


  typedef viennagrid::segmented_mesh<viennagrid::thin_tetrahedral_3d_mesh, viennagrid::thin_cell_only_tetrahedral_3d_segmentation> MeshType;

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
