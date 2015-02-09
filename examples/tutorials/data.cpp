#include <string>
#include <deque>
#include <vector>

#include <cassert>
#include <iostream>

#include "viennamesh/core.hpp"


int main()
{
  viennamesh::context_handle context;

//   context.load_plugins_in_directory(VIENNAMESH_DEFAULT_PLUGIN_DIRECTORY);

  viennamesh::data_handle<int> my_int = context.make_data<int>();
  viennamesh::data_handle<double> my_double = context.make_data<double>();

  double x;
  my_double.use(x);

  my_int() = 42;
  viennamesh::convert( my_int, my_double );

  std::cout << "generic_get<double>(my_double) = " << my_double() << std::endl;
  std::cout << "x = " << x << std::endl;

  return -1;
}
