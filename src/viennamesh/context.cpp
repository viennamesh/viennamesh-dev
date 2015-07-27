#include <cstdlib>
#include <dirent.h>

#include "viennagrid/viennagrid.h"
#include "context.hpp"


viennamesh_context_t::viennamesh_context_t() : use_count_(1)
{
#ifdef VIENNAMESH_BACKEND_RETAIN_RELEASE_LOGGING
  std::cout << "New context at " << this << std::endl;
#endif
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
    VIENNAMESH_ERROR(VIENNAMESH_ERROR_INVALID_ARGUMENT, "viennamesh_context_t::registered_data_type_name invalid index: " + boost::lexical_cast<std::string>(index_));

  std::map<std::string, viennamesh::data_template_t>::const_iterator it = data_types.begin();
  std::advance(it, index_);
  return it->first;
}

viennamesh::data_template_t & viennamesh_context_t::get_data_type(std::string const & data_type_name_)
{
  std::map<std::string, viennamesh::data_template_t>::iterator it = data_types.find(data_type_name_);
  if (it == data_types.end())
    VIENNAMESH_ERROR( VIENNAMESH_ERROR_DATA_TYPE_NOT_REGISTERED, "Data type \"" + data_type_name_ + "\" is not registered" );

  return it->second;
}

void viennamesh_context_t::register_data_type(std::string const & data_type_name_,
                                            viennamesh_data_make_function make_function_,
                                            viennamesh_data_delete_function delete_function_)
{
  if (data_type_name_.empty())
    VIENNAMESH_ERROR(VIENNAMESH_ERROR_INVALID_ARGUMENT, "data_type_name_ is empty");

  std::map<std::string, viennamesh::data_template_t>::iterator it = data_types.find(data_type_name_);
  if (it == data_types.end())
  {
    // TODO logging
    it = data_types.insert( std::make_pair(data_type_name_, viennamesh::data_template_t()) ).first;
    it->second.name() = data_type_name_;
    it->second.set_context(this);
    it->second.set_make_delete_function(make_function_, delete_function_);
  }

  viennamesh::backend::info(10) << "Data type \"" << data_type_name_ << "\" sucessfully registered" << std::endl;
}

viennamesh_data_wrapper viennamesh_context_t::make_data(std::string const & data_type_name_)
{
  try
  {
    return new viennamesh_data_wrapper_t( &get_data_type(data_type_name_) );
  }
  catch (...)
  {
    throw;
  }

  return 0;
}

void viennamesh_context_t::register_conversion_function(std::string const & data_type_from,
                                  std::string const & data_type_to,
                                  viennamesh_data_convert_function convert_function)
{
  get_data_type(data_type_from).add_conversion_function(data_type_to, convert_function);

  viennamesh::backend::info(10) << "Conversion function from data type \"" << data_type_from << "\" to data type \"" << data_type_to << "\" sucessfully registered" << std::endl;
}

void viennamesh_context_t::convert(viennamesh_data_wrapper from, viennamesh_data_wrapper to)
{
  if (from->context() != to->context())
    VIENNAMESH_ERROR(VIENNAMESH_ERROR_DIFFERENT_CONTEXT, "");

  std::string from_data_type_name = from->type_name();

  get_data_type(from_data_type_name).convert( from, to );
}

viennamesh_data_wrapper viennamesh_context_t::convert_to(viennamesh_data_wrapper from,
                            std::string const & data_type_name_)
{
  viennamesh_data_wrapper result = make_data(data_type_name_);
  convert(from, result);
  return result;
}

viennamesh::algorithm_template viennamesh_context_t::get_algorithm_template(std::string const & algorithm_name_)
{
  std::map<std::string, viennamesh::algorithm_template_t>::iterator it = algorithm_templates.find(algorithm_name_);
  if (it == algorithm_templates.end())
    VIENNAMESH_ERROR(VIENNAMESH_ERROR_ALGORITHM_NOT_REGISTERED, "Algorithm \"" + algorithm_name_ + "\" not registered");

  return &it->second;
}


viennamesh_plugin viennamesh_context_t::load_plugin(std::string const & plugin_filename)
{
  viennamesh::backend::LoggingStack stack("Loading plugin \"" + plugin_filename + "\"", 10);

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

  if (dir != NULL)
  {
    /* print all the files and directories within directory */
    std::vector<std::string> plugins_in_directory;

    while ((ent = readdir (dir)) != NULL)
    {
      std::string filename = ent->d_name;
      if ( (filename.size() > 3) && (filename.find(".so") == filename.size()-3) )
        plugins_in_directory.push_back( filename );
    }

    closedir (dir);

    if (plugins_in_directory.empty())
    {
      viennamesh::backend::info(10) << "No plugins in directory \"" << directory_name << "\" -> skipping" << std::endl;
      return;
    }

    viennamesh::backend::LoggingStack stack("Loading all plugins in directory \"" + directory_name + "\"", 10);
    for (std::size_t i = 0; i != plugins_in_directory.size(); ++i)
      load_plugin(directory_name + plugins_in_directory[i]);
  }
  else
  {
    viennamesh::backend::warning(10) << "Opening directory \"" << directory_name << "\" for plugin loading failed" << std::endl;
  }
}




