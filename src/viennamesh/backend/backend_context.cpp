#include <cstdlib>
#include <dirent.h>

#include "viennagrid/backend/api.h"
#include "viennamesh/backend/backend_context.hpp"
#include "viennamesh/basic_data.hpp"

namespace viennamesh
{
  namespace backend
  {
    template<typename T>
    int generic_make(viennamesh_data * data)
    {
      T * tmp = new T;
      *data = tmp;

      return VIENNAMESH_SUCCESS;
    }

    template<typename T>
    int generic_delete(viennamesh_data data)
    {
      delete (T*)data;
      return VIENNAMESH_SUCCESS;
    }



    template<typename DataT, typename MakeFunctionT>
    int make_viennamesh_data(viennamesh_data * data, MakeFunctionT make_function_)
    {
      DataT * internal_data = new DataT;
      make_function_(internal_data);
      *data = internal_data;

      return VIENNAMESH_SUCCESS;
    }

    template<typename DataT, typename MakeFunctionT>
    int delete_viennamesh_data(viennamesh_data data, MakeFunctionT free_function_)
    {
      DataT * internal_data = (DataT*)data;
      free_function_(*internal_data);
      delete internal_data;

      return VIENNAMESH_SUCCESS;
    }


    int make_string(viennamesh_data * data)
    {
      return make_viennamesh_data<viennamesh_string>(data, viennamesh_string_make);
    }

    int delete_string(viennamesh_data data)
    {
      return delete_viennamesh_data<viennamesh_string>(data, viennamesh_string_free);
    }


    int make_point_container(viennamesh_data * data)
    {
      return make_viennamesh_data<viennamesh_point_container>(data, viennamesh_point_container_make);
    }

    int delete_point_container(viennamesh_data data)
    {
      return delete_viennamesh_data<viennamesh_point_container>(data, viennamesh_point_container_free);
    }


    int make_seed_point_container(viennamesh_data * data)
    {
      return make_viennamesh_data<viennamesh_seed_point_container>(data, viennamesh_seed_point_container_make);
    }

    int delete_seed_point_container(viennamesh_data data)
    {
      return delete_viennamesh_data<viennamesh_seed_point_container>(data, viennamesh_seed_point_container_free);
    }


    int make_mesh(viennamesh_data * data)
    {
      viennagrid_mesh_hierarchy mesh_hierarchy;
      viennagrid_mesh * mesh = new viennagrid_mesh;

      viennagrid_mesh_hierarchy_create(&mesh_hierarchy);
      viennagrid_mesh_hierarchy_get_root(mesh_hierarchy, mesh);

      *data = mesh;

      return VIENNAMESH_SUCCESS;
    }

    int delete_mesh(viennamesh_data data)
    {
      viennagrid_mesh * mesh = (viennagrid_mesh *)data;

      viennagrid_mesh_hierarchy mesh_hierarchy;
      viennagrid_mesh_get_mesh_hierarchy(*mesh, &mesh_hierarchy);
      viennagrid_mesh_hierarchy_release(mesh_hierarchy);

      delete mesh;

      return VIENNAMESH_SUCCESS;
    }



    template<typename FromT, typename ToT>
    int generic_convert(viennamesh_data from_, viennamesh_data to_)
    {
      *static_cast<ToT*>(to_) = *static_cast<FromT*>(from_);
      return VIENNAMESH_SUCCESS;
    }
  }
}


viennamesh_context_t::viennamesh_context_t() : use_count_(1)
{
#ifdef VIENNAMESH_BACKEND_RETAIN_RELEASE_LOGGING
  std::cout << "New context at " << this << std::endl;
#endif

  register_data_type(viennamesh::result_of::data_information<bool>::type_name(),
                     viennamesh::result_of::data_information<bool>::local_binary_format(),
                     viennamesh::backend::generic_make<bool>,
                     viennamesh::backend::generic_delete<bool>);

  register_data_type(viennamesh::result_of::data_information<int>::type_name(),
                     viennamesh::result_of::data_information<int>::local_binary_format(),
                     viennamesh::backend::generic_make<int>,
                     viennamesh::backend::generic_delete<int>);

  register_data_type(viennamesh::result_of::data_information<double>::type_name(),
                     viennamesh::result_of::data_information<double>::local_binary_format(),
                     viennamesh::backend::generic_make<double>,
                     viennamesh::backend::generic_delete<double>);

  register_data_type(viennamesh::result_of::data_information<viennamesh_string>::type_name(),
                     viennamesh::result_of::data_information<viennamesh_string>::local_binary_format(),
                     viennamesh::backend::make_string,
                     viennamesh::backend::delete_string);

  register_data_type(viennamesh::result_of::data_information<viennamesh_point_container>::type_name(),
                     viennamesh::result_of::data_information<viennamesh_point_container>::local_binary_format(),
                     viennamesh::backend::make_point_container,
                     viennamesh::backend::delete_point_container);

  register_data_type(viennamesh::result_of::data_information<viennamesh_seed_point_container>::type_name(),
                     viennamesh::result_of::data_information<viennamesh_seed_point_container>::local_binary_format(),
                     viennamesh::backend::make_seed_point_container,
                     viennamesh::backend::delete_seed_point_container);

  register_data_type(viennamesh::result_of::data_information<viennagrid_mesh>::type_name(),
                     viennamesh::result_of::data_information<viennagrid_mesh>::local_binary_format(),
                     viennamesh::backend::make_mesh,
                     viennamesh::backend::delete_mesh);

  register_conversion_function("int","","double","",viennamesh::backend::generic_convert<int, double>);
  register_conversion_function("double","","int","",viennamesh::backend::generic_convert<double, int>);
}

viennamesh_context_t::~viennamesh_context_t()
{
  for (std::set<viennamesh_plugin>::iterator it = loaded_plugins.begin(); it != loaded_plugins.end(); ++it)
    dlclose(*it);
}



int viennamesh_context_t::registered_data_type_count() const { return data_types.size(); }

std::string const & viennamesh_context_t::registered_data_type_name(int index_) const
{
  if (index_ < 0 || index_ >= registered_data_type_count())
    handle_error(VIENNAMESH_ERROR_INVALID_ARGUMENT);

  std::map<std::string, viennamesh::data_template_t>::const_iterator it = data_types.begin();
  std::advance(it, index_);
  return it->first;
}

viennamesh::data_template_t & viennamesh_context_t::get_data_type(std::string const & data_type_name_)
{
  std::map<std::string, viennamesh::data_template_t>::iterator it = data_types.find(data_type_name_);
  if (it == data_types.end())
    throw viennamesh::error_t(VIENNAMESH_ERROR_DATA_TYPE_NOT_REGISTERED);

  return it->second;
}

void viennamesh_context_t::register_data_type(std::string const & data_type_name_,
                                            std::string const & data_type_binary_format_,
                                            viennamesh_data_make_function make_function_,
                                            viennamesh_data_delete_function delete_function_)
{
  if (data_type_name_.empty())
    handle_error(VIENNAMESH_ERROR_INVALID_ARGUMENT);

  std::map<std::string, viennamesh::data_template_t>::iterator it = data_types.find(data_type_name_);
  if (it == data_types.end())
  {
    // TODO logging
    it = data_types.insert( std::make_pair(data_type_name_, viennamesh::data_template_t()) ).first;
    it->second.name() = data_type_name_;
    it->second.set_context(this);
  }

  it->second.register_data_type(data_type_binary_format_, make_function_, delete_function_);

  viennamesh::backend::info(1) << "Data type \"" << data_type_name_ << "\" with binary format \"" << data_type_binary_format_ << "\" sucessfully registered" << std::endl;
}

viennamesh_data_wrapper viennamesh_context_t::make_data(std::string const & data_type_name_,
                                                     std::string const & data_type_binary_format_)
{
  viennamesh_data_wrapper result = new viennamesh_data_wrapper_t(&get_data_type(data_type_name_).get_binary_format_template(data_type_binary_format_));
  try
  {
    result->make_data();
  }
  catch (...)
  {
    delete result;
    throw;
  }

  return result;
}

void viennamesh_context_t::register_conversion_function(std::string const & data_type_from,
                                  std::string const & binary_format_from,
                                  std::string const & data_type_to,
                                  std::string const & binary_format_to,
                                  viennamesh_data_convert_function convert_function)
{
  get_data_type(data_type_from).get_binary_format_template(binary_format_from).add_conversion_function(data_type_to, binary_format_to, convert_function);

  viennamesh::backend::info(1) << "Conversion function from data type \"" << data_type_from << "\" with binary format \"" << binary_format_from << "\" to data type \"" << data_type_to << "\" with binary format \"" << binary_format_to << "\" sucessfully registered" << std::endl;
}

void viennamesh_context_t::convert(viennamesh_data_wrapper from, viennamesh_data_wrapper to)
{
  std::string from_data_type_name = from->type_name();
  std::string from_data_binary_format = from->binary_format();

  get_data_type(from_data_type_name).get_binary_format_template(from_data_binary_format).convert( from, to );
}

viennamesh_data_wrapper viennamesh_context_t::convert_to(viennamesh_data_wrapper from,
                            std::string const & data_type_name_,
                            std::string const & data_type_binary_format_)
{
  viennamesh_data_wrapper result = make_data(data_type_name_, data_type_binary_format_);
  convert(from, result);
  return result;
}

viennamesh::algorithm_template viennamesh_context_t::get_algorithm_template(std::string const & algorithm_name_)
{
  std::map<std::string, viennamesh::algorithm_template_t>::iterator it = algorithm_templates.find(algorithm_name_);
  if (it == algorithm_templates.end())
    throw viennamesh::error_t(VIENNAMESH_ERROR_ALGORITHM_NOT_REGISTERED);

  return &it->second;
}


viennamesh_plugin viennamesh_context_t::load_plugin(std::string const & plugin_filename)
{
  viennamesh::backend::LoggingStack stack("Loading plugin \"" + plugin_filename + "\"");

  void * dl = dlopen(plugin_filename.c_str(), RTLD_NOW);
  if (!dl)
  {
    viennamesh::backend::error(1) << "Could not load plugin \"" << plugin_filename << "\"" << std::endl;
    viennamesh::backend::error(1) << dlerror() << std::endl;
    return 0;
  }

  typedef int (*viennamesh_plugin_init_function)(viennamesh_context ctx_);
  typedef int (*viennamesh_version_function)();

  viennamesh_plugin_init_function init_function = (viennamesh_plugin_init_function)dlsym(dl, "viennamesh_plugin_init");
  if (!init_function)
  {
    dlclose(dl);
    viennamesh::backend::error(1) << "Plugin \"" << plugin_filename << "\" does not have a viennamesh_plugin_init function" << std::endl;
    return 0;
  }

  viennamesh_version_function version_function = (viennamesh_version_function)dlsym(dl, "viennamesh_version");
  if (!version_function)
  {
    dlclose(dl);
    viennamesh::backend::error(1) << "Plugin \"" << plugin_filename << "\" does not have a viennamesh_version function" << std::endl;
    return 0;
  }

  if (version_function() > VIENNAMESH_VERSION)
  {
    dlclose(dl);
    viennamesh::backend::error(1) << "Plugin \"" << plugin_filename << "\" has version missmatch. Plugin ViennaMesh version: " << version_function() << ", ViennaMesh version: " << VIENNAMESH_VERSION << std::endl;
    return 0;
  }

  init_function( this );
  loaded_plugins.insert(dl);

//   viennamesh::backend::info(1) << "Plugin \"" << plugin_filename << "\" successfully loaded" << std::endl;

  return dl;
}


void viennamesh_context_t::load_plugins_in_directory(std::string directory_name)
{
  DIR *dir;
  struct dirent *ent;

  if (directory_name[directory_name.size()-1] != '/')
    directory_name += '/';

  dir = opendir(directory_name.c_str());

  if (dir != NULL) {
    /* print all the files and directories within directory */
    while ((ent = readdir (dir)) != NULL)
    {
      std::string filename = ent->d_name;
      if ( (filename.size() > 3) && (filename.find(".so") == filename.size()-3) )
        load_plugin(directory_name + filename);
    }
    closedir (dir);
  }
  else
  {
    viennamesh::backend::warning(1) << "Loading plugins in directory \"" << directory_name << "\" failed" << std::endl;
  }
}




