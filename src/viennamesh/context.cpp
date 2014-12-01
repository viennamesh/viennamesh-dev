#include "viennamesh/context.hpp"
#include "viennamesh/algorithm.hpp"

namespace viennamesh
{

  algorithm_handle context_handle::make_algorithm(std::string const & algorithm_name)
  {
    algorithm_handle tmp;
    tmp.make(ctx, algorithm_name);
    return tmp;
  }

  void context_handle::load_plugin(std::string const & plugin_filename)
  {
    viennamesh_plugin plugin;
    viennamesh_context_load_plugin( internal(), plugin_filename.c_str(), &plugin);
  }

}
