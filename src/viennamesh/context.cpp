#include "viennamesh/context.hpp"
#include "viennamesh/core.hpp"

namespace viennamesh
{

  context_handle::context_handle() : ctx(0)
  {
    make();

    {
      viennamesh::LoggingStack logging_stack("Registering built-in data types");

      register_data_type<bool>();
      register_data_type<int>();
      register_data_type<double>();
      register_data_type<viennamesh_string>();
      register_data_type<viennamesh_point>();
      register_data_type<viennamesh_seed_point>();
      register_data_type<viennagrid_quantity_field>();
      register_data_type<viennagrid_mesh>();

      register_conversion<int,double>();
      register_conversion<double,int>();
    }

    load_plugins_in_directories(VIENNAMESH_DEFAULT_PLUGIN_DIRECTORY, ";");
  }

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

  void context_handle::load_plugins_in_directories(std::string const & directory_name, std::string const & delimiter)
  {
    std::list<std::string> directories = stringtools::split_string(directory_name, delimiter);

    for (std::list<std::string>::const_iterator dit = directories.begin(); dit != directories.end(); ++dit)
      viennamesh_context_load_plugins_in_directory( internal(), (*dit).c_str());
  }

}
