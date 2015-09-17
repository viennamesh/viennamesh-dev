#include "viennameshpp/context.hpp"
#include "viennameshpp/core.hpp"

#include "boost/algorithm/string.hpp"

namespace viennamesh
{

  context_handle::context_handle() : ctx(0)
  {
    make();

    {
      viennamesh::LoggingStack logging_stack("Registering built-in data types", 10);

      register_data_type<bool>();
      register_data_type<int>();
      register_data_type<double>();
      register_data_type<viennamesh_string>();
      register_data_type<viennamesh_point>();
      register_data_type<viennamesh_seed_point>();
      register_data_type<viennagrid_quantity_field>();
      register_data_type<viennagrid_mesh>();
      register_data_type<viennagrid_plc>();

      register_conversion<int,double>();
      register_conversion<double,int>();
    }

    load_plugins_in_directories(VIENNAMESH_DEFAULT_PLUGIN_DIRECTORY, ";");
  }

  context_handle::context_handle(viennamesh_context ctx_) : ctx(ctx_) { retain(); }
  context_handle::context_handle(context_handle const & handle_) : ctx(handle_.ctx) { retain(); }

  context_handle::~context_handle()
  {
    release();
  }

  context_handle & context_handle::operator=(context_handle const & handle_)
  {
    release();
    ctx = handle_.ctx;
    retain();
    return *this;
  }

  void context_handle::register_data_type(std::string const & data_type,
                                          viennamesh_data_make_function make_function,
                                          viennamesh_data_delete_function delete_function)
  {
    handle_error(
      viennamesh_data_type_register(ctx, data_type.c_str(), make_function, delete_function),
      ctx);
  }

  void context_handle::register_conversion(std::string const & data_type_from,
                                           std::string const & data_type_to,
                                           viennamesh_data_convert_function convert_function)
  {
    handle_error(
      viennamesh_data_conversion_register(ctx, data_type_from.c_str(), data_type_to.c_str(), convert_function),
      ctx);
  }

  void context_handle::register_algorithm(std::string const & algorithm_name,
                                          viennamesh_algorithm_make_function make_function,
                                          viennamesh_algorithm_delete_function delete_function,
                                          viennamesh_algorithm_init_function init_function,
                                          viennamesh_algorithm_run_function run_function)
  {
    handle_error(
      viennamesh_algorithm_register(ctx, algorithm_name.c_str(),
                                  make_function, delete_function,
                                  init_function, run_function),
      ctx);
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
    typedef std::vector<std::string> StringContainerType;
    StringContainerType directories;
    boost::algorithm::split( directories, directory_name, boost::is_any_of(delimiter) );

    for (StringContainerType::const_iterator dit = directories.begin(); dit != directories.end(); ++dit)
    {
      if ( !(*dit).empty() )
        viennamesh_context_load_plugins_in_directory( internal(), (*dit).c_str());
    }
  }


  viennamesh_context context_handle::internal() const
  {
    return const_cast<viennamesh_context>(ctx);
  }

  void context_handle::retain()
  {
    if (ctx)
      handle_error(viennamesh_context_retain(ctx), ctx);
  }

  void context_handle::release()
  {
    if (ctx)
      viennamesh_context_release(ctx);
  }


  void context_handle::make()
  {
    if (ctx)
      handle_error(viennamesh_context_retain(ctx), ctx);
    handle_error(viennamesh_context_make(&ctx), ctx);
  }

}
