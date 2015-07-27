#ifndef _VIENNAMESH_BACKEND_CONTEXT_HPP_
#define _VIENNAMESH_BACKEND_CONTEXT_HPP_

#include <set>
#include <dlfcn.h>

#include "forwards.hpp"
#include "data.hpp"
#include "algorithm.hpp"
#include "logger.hpp"

struct viennamesh_context_t
{
public:

  viennamesh_context_t();
  ~viennamesh_context_t();

  int registered_data_type_count() const;
  std::string const & registered_data_type_name(int index_) const;

  viennamesh::data_template_t & get_data_type(std::string const & data_type_name_);
  viennamesh::data_template_t const & get_data_type(std::string const & data_type_name_) const;

  void register_data_type(std::string const & data_type_name_,
                          viennamesh_data_make_function make_function_,
                          viennamesh_data_delete_function delete_function_);

  viennamesh_data_wrapper make_data(std::string const & data_type_name_);
  void delete_data(viennamesh_data_wrapper data) const;



  void register_conversion_function(std::string const & data_type_from,
                                    std::string const & data_type_to,
                                    viennamesh_data_convert_function convert_function);


  void convert(viennamesh_data_wrapper from, viennamesh_data_wrapper to);
  viennamesh_data_wrapper convert_to(viennamesh_data_wrapper from,
                                    std::string const & data_type_name_);




  viennamesh::algorithm_template get_algorithm_template(std::string const & algorithm_name_);

  void register_algorithm(std::string const & algorithm_id,
                          viennamesh_algorithm_make_function make_function,
                          viennamesh_algorithm_delete_function delete_function,
                          viennamesh_algorithm_init_function init_function,
                          viennamesh_algorithm_run_function run_function)
  {
    std::map<std::string, viennamesh::algorithm_template_t>::iterator it = algorithm_templates.find(algorithm_id);
    if (it != algorithm_templates.end())
      VIENNAMESH_ERROR(VIENNAMESH_ERROR_ALGORITHM_ALREADY_REGISTERED, "Algorithm \"" + algorithm_id + "\" already registered");

    viennamesh::algorithm_template_t & algorithm_template = algorithm_templates[algorithm_id];
    algorithm_template.set_context(this);
    algorithm_template.init(algorithm_id,
                            make_function, delete_function,
                            init_function, run_function);

    viennamesh::backend::info(10) << "Algorithm \"" << algorithm_id << "\" sucessfully registered" << std::endl;
  }

  viennamesh_algorithm_wrapper make_algorithm(std::string const & algorithm_id)
  {
    viennamesh::algorithm_template algorithm_template = get_algorithm_template(algorithm_id);
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

  viennamesh_error error_code() const { return error_code_; }
  std::string const & error_function() const { return error_function_; };
  std::string const & error_file() const  { return error_file_; };
  int error_line() const { return error_line_; }
  std::string const & error_message() const { return error_message_; }

  void set_error(viennamesh_error error_code_in,
                 std::string const & function_in, std::string const & file_in, int line_in,
                 std::string const & error_message_in)
  {
    error_code_ = error_code_in;
    error_function_ = function_in;
    error_file_ = file_in;
    error_line_ = line_in;
    error_message_ = error_message_in;
    viennamesh::backend::error(1) << error_message() << std::endl;
  }

  void set_error(viennamesh::exception const & ex)
  {
    set_error( ex.error_code(), ex.function(), ex.file(), ex.line(), ex.what() );
  }

  void clear_error()
  {
    error_code_ = VIENNAMESH_SUCCESS;
    error_function_.clear();
    error_file_.clear();
    error_line_ = -1;
    error_message_.clear();
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
  viennamesh_error error_code_;
  std::string error_function_;
  std::string error_file_;
  int error_line_;
  std::string error_message_;

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
