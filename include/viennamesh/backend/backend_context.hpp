#ifndef _VIENNAMESH_BACKEND_CONTEXT_HPP_
#define _VIENNAMESH_BACKEND_CONTEXT_HPP_

#include <set>
#include <dlfcn.h>

#include "viennamesh/backend/backend_forwards.hpp"
#include "viennamesh/backend/backend_data.hpp"
#include "viennamesh/backend/backend_algorithm.hpp"
#include "viennamesh/backend/backend_logger.hpp"

class viennamesh_context_t
{
public:

  viennamesh_context_t();
  ~viennamesh_context_t();

  int registered_data_type_count() const;
  std::string const & registered_data_type_name(int index_) const;

  viennamesh::data_template_t & get_data_type(std::string const & data_type_name_);
//   viennamesh::data_template_t const & get_data_type(std::string const & data_type_name_) const;

  void register_data_type(std::string const & data_type_name_,
                          std::string const & data_type_binary_format_,
                          viennamesh_data_make_function make_function_,
                          viennamesh_data_delete_function delete_function_);

  viennamesh_data_wrapper make_data(std::string const & data_type_name_,
                                   std::string const & data_type_binary_format_);
  void delete_data(viennamesh_data_wrapper data) const;



  void register_conversion_function(std::string const & data_type_from,
                                    std::string const & binary_format_from,
                                    std::string const & data_type_to,
                                    std::string const & binary_format_to,
                                    viennamesh_data_convert_function convert_function);


  void convert(viennamesh_data_wrapper from, viennamesh_data_wrapper to);
  viennamesh_data_wrapper convert_to(viennamesh_data_wrapper from,
                                    std::string const & data_type_name_,
                                    std::string const & data_type_binary_format_);




  viennamesh::algorithm_template get_algorithm_template(std::string const & algorithm_name_);

  void register_algorithm(std::string const & algorithm_name,
                          viennamesh_algorithm_make_function make_function,
                          viennamesh_algorithm_delete_function delete_function,
                          viennamesh_algorithm_init_function init_function,
                          viennamesh_algorithm_run_function run_function)
  {
    std::map<std::string, viennamesh::algorithm_template_t>::iterator it = algorithm_templates.find(algorithm_name);
    if (it != algorithm_templates.end())
      handle_error(VIENNAMESH_ERROR_ALGORITHM_ALREADY_REGISTERED);

    viennamesh::algorithm_template_t & algorithm_template = algorithm_templates[algorithm_name];
    algorithm_template.set_context(this);
    algorithm_template.init(algorithm_name,
                            make_function, delete_function,
                            init_function, run_function);

    viennamesh::backend::info(1) << "Algorithm \"" << algorithm_name << "\" sucessfully registered" << std::endl;
  }

  viennamesh_algorithm_wrapper make_algorithm(std::string const & algorithm_name)
  {
    std::map<std::string, viennamesh::algorithm_template_t>::const_iterator atit = algorithm_templates.begin();

    viennamesh::algorithm_template algorithm_template = get_algorithm_template(algorithm_name);
    viennamesh_algorithm internal_algorithm = algorithm_template->make_algorithm();

    viennamesh_algorithm_wrapper result = new viennamesh_algorithm_wrapper_t(algorithm_template);
    result->set_internal_algorithm(internal_algorithm);

    return result;
  }

  void delete_algorithm(viennamesh_algorithm_wrapper algorithm)
  {
    algorithm->algorithm_template()->delete_algorithm(algorithm);
    delete algorithm;
  }

  void run_algorithm(viennamesh_algorithm_wrapper algorithm)
  {
    algorithm->run();
  }


  viennamesh_plugin load_plugin(std::string const & plugin_filename);
  void load_plugins_in_directory(std::string directory_name);


  void retain() { ++use_count_; }
  bool release()
  {
    if (--use_count_ <= 0)
    {
      delete_this();
      return false;
    }

    return true;
  }

private:

  void handle_error(int error_code) const
  {
    throw viennamesh::error_t(error_code);
  }

  std::map<std::string, viennamesh::data_template_t> data_types;
  std::map<std::string, viennamesh::algorithm_template_t> algorithm_templates;

  void delete_this()
  {
#ifdef VIENNAMESH_BACKEND_RETAIN_RELEASE_LOGGING
    std::cout << "Delete context at " << this << std::endl;
#endif
    delete this;
  }

  std::set<viennamesh_plugin> loaded_plugins;

  int use_count_;
};

#endif
