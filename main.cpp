#include <string>
#include <deque>
#include <vector>

#include <cassert>
#include <iostream>

#include "viennamesh/core.hpp"





int main()
{
  viennamesh::context_handle context;

//   viennamesh::data_handle<int> my_int = context.make_data<int>();
//   viennamesh::data_handle<double> my_double = context.make_data<double>();

//   context.load_plugins_in_directory("plugins");
  context.load_plugins_in_directory(VIENNAMESH_DEFAULT_PLUGIN_DIRECTORY);



//   double x;
//   my_double.use(x);

//   my_int() = 42;
//   viennamesh::convert( my_int, my_double );

//   std::cout << "generic_get<double>(my_double) = " << my_double() << std::endl;
//   std::cout << "x = " << x << std::endl;


//   viennamesh::algorithm_handle algorithm = context.make_algorithm("square");
//   viennamesh::algorithm_handle algorithm1 = context.make_algorithm("square");


//   algorithm.set_input( "value", my_int );
//   algorithm.run();
//
//   algorithm1.link_input( "value", algorithm, "value" );
//   algorithm1.run();

//   std::cout << algorithm1.get_output<double>("value")() << std::endl;





  viennamesh::algorithm_handle mesh_reader = context.make_algorithm("mesh_reader");
  mesh_reader.set_input( "filename", "../examples/data/half-trigate.mesh" );
//   mesh_reader.set_input( "filename", "../examples/data/cube3072.mesh" );
  mesh_reader.run();


  viennamesh::algorithm_handle mesh_writer = context.make_algorithm("mesh_writer");
//   mesh_writer.link_input( "mesh", mesh_reader, "mesh" );
  mesh_writer.set_default_source(mesh_reader);
  mesh_writer.set_input( "filename", "bla.vtu" );
  mesh_writer.run();


  return -1;
}
