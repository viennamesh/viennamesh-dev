#include <string>
#include <deque>
#include <vector>

#include <cassert>
#include <iostream>

#include "viennameshpp/core.hpp"


int main()
{
  viennamesh::context_handle context;

//   context.load_plugins_in_directory(VIENNAMESH_DEFAULT_PLUGIN_DIRECTORY);

  viennamesh::data_handle<int> my_int = context.make_data<int>();
  viennamesh::data_handle<double> my_double = context.make_data<double>();

  my_int.resize(2);
  my_int.set(0, 10);
  my_int.set(1, 42);

  viennamesh::convert(my_int, my_double);

  std::cout << my_double(0) << std::endl;
  std::cout << my_double(1) << std::endl;

  return -1;
}
