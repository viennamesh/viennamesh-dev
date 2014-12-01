#include <string>
#include <deque>
#include <vector>

#include <cassert>
#include <iostream>
// #include <dlfcn.h>

#include "viennamesh/core.hpp"




// class dynamic_library
// {
// public:
//
//   dynamic_library() : dl(0) {}
//
//   bool load(std::string const & filename)
//   {
//     dl = dlopen(filename.c_str(), RTLD_NOW);
//     if (!dl)
//       return false;
//
//     return true;
//   }
//
//   template<typename FunctionT>
//   bool get_function(std::string const & function_symbol_name, FunctionT & function)
//   {
//     void * f = dlsym(dl, function_symbol_name.c_str());
//     if (!f)
//       return false;
//
//     function = (FunctionT)f;
//     return true;
//   }
//
// private:
//
//   void * dl;
// };




// namespace viennamesh
// {
//   class square
//   {
//   public:
//
//     static std::string name() { return "square"; }
//
//     bool init(viennamesh::algorithm_handle algorithm)
//     {
//       std::cout << "INIT" << std::endl;
//       return true;
//     }
//
//     bool run(viennamesh::algorithm_handle algorithm)
//     {
//       info(1) << "YAY!" << std::endl;
//
//       data_handle<double> my_double = algorithm.get_input<double>("value");
//       info(1) << "From the algorithm: " << my_double() << std::endl;
//
//       data_handle<double> squared = algorithm.context().make_data<double>();
//       squared() = my_double()*my_double();
//
//       algorithm.set_output( "value", squared );
//
//       return true;
//     }
//
//   private:
//   };
// }






int main()
{
  viennamesh::context_handle context;

//   viennamesh::register_data_type<int>(context);
//   viennamesh::register_data_type<double>(context);
//   viennamesh::register_data_type<char*>(context);
//   viennamesh::register_convert<int, double>(context);


  viennamesh::data_handle<int> my_int = context.make_data<int>();
  viennamesh::data_handle<double> my_double = context.make_data<double>();


  context.load_plugin("/export/florian/work/dev/ViennaMesh/viennamesh-dev/build/plugins/test/test.so");
  context.load_plugin("/export/florian/work/dev/ViennaMesh/viennamesh-dev/build/plugins/io/io.so");


  double x;
  my_double.use(x);

  my_int() = 42;
  viennamesh::convert( my_int, my_double );

  std::cout << "generic_get<double>(my_double) = " << my_double() << std::endl;
  std::cout << "x = " << x << std::endl;


  viennamesh::algorithm_handle algorithm = context.make_algorithm("square");
  viennamesh::algorithm_handle algorithm1 = context.make_algorithm("square");


  algorithm.set_input( "value", my_int );
  algorithm.run();

  algorithm1.link_input( "value", algorithm, "value" );
  algorithm1.run();

  std::cout << algorithm1.get_output<double>("value")() << std::endl;





  viennamesh::algorithm_handle mesh_reader = context.make_algorithm("mesh_reader");
  mesh_reader.set_input( "filename", "../examples/data/half-trigate.mesh" );
  mesh_reader.run();


  viennamesh::algorithm_handle mesh_writer = context.make_algorithm("mesh_writer");
//   mesh_writer.link_input( "mesh", mesh_reader, "mesh" );
  mesh_writer.set_default_source(mesh_reader);
  mesh_writer.set_input( "filename", "bla.vtu" );
  mesh_writer.run();


  return -1;
}
