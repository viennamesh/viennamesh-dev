#ifndef _VIENNAMESH_BACKEND_ALGORITHM_HPP_
#define _VIENNAMESH_BACKEND_ALGORITHM_HPP_

#include <iostream>
#include "data.hpp"

class input_parameter
{
public:

  ~input_parameter()
  {
    unset();
  }

  void unset();
  void set(viennamesh_data_wrapper input_);
  void link(viennamesh_algorithm_wrapper source_algorithm_, std::string source_name_);

  viennamesh_data_wrapper unpack() const;

private:
  viennamesh_data_wrapper input;

  viennamesh_algorithm_wrapper source_algorithm;
  std::string source_name;
};


struct viennamesh_algorithm_wrapper_t
{
public:

  viennamesh_algorithm_wrapper_t() : default_source(0), use_count_(1) {}
  viennamesh_algorithm_wrapper_t(viennamesh::algorithm_template algorithm_template_in) : default_source(0), algorithm_template_(algorithm_template_in), use_count_(1)
  {
#ifdef VIENNAMESH_BACKEND_RETAIN_RELEASE_LOGGING
    std::cout << "New algorithm at " << this << std::endl;
#endif
  }

  ~viennamesh_algorithm_wrapper_t()
  {
    for (OutputMapType::iterator it = outputs.begin(); it != outputs.end(); ++it)
      it->second->release();

    unset_default_source();
  }

  void init();
  void run();

  void clear_inputs();
  void unset_input(std::string const & name);
  void set_input(std::string const & name, viennamesh_data_wrapper input);
  void link_input(std::string const & name, viennamesh_algorithm_wrapper source_algorithm, std::string const & source_name);
  viennamesh_data_wrapper get_input(std::string const & name);
  viennamesh_data_wrapper get_input(std::string const & name,
                                    std::string const & type_name);

  void clear_outputs();
  void set_output(std::string const & name, viennamesh_data_wrapper output);
  viennamesh_data_wrapper get_output(std::string const & name);
  viennamesh_data_wrapper get_output(std::string const & name,
                                     std::string const & type_name);

  viennamesh_algorithm internal_algorithm() { return internal_algorithm_; }
  void set_internal_algorithm(viennamesh_algorithm internal_algorithm_in) { internal_algorithm_ = internal_algorithm_in; }
  viennamesh::algorithm_template algorithm_template() { return algorithm_template_; }
  std::string const & type();
  viennamesh_context context();

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


  void unset_default_source()
  {
    if (default_source)
      default_source->release();
    default_source = 0;
  }

  void set_default_source(viennamesh_algorithm_wrapper default_source_)
  {
    if (context() != default_source_->context())
      VIENNAMESH_ERROR(VIENNAMESH_ERROR_DIFFERENT_CONTEXT, "");

    unset_default_source();
    default_source = default_source_;
    default_source->retain();
  }


  std::string const & base_path() const { return base_path_; }
  void set_base_path(std::string const & base_path_in) { base_path_ = base_path_in; }


private:
  viennamesh_algorithm_wrapper default_source;

  std::string base_path_;

  viennamesh::algorithm_template algorithm_template_;
  viennamesh_algorithm internal_algorithm_;

  typedef std::map<std::string, input_parameter> InputMapType;
  typedef std::map<std::string, viennamesh_data_wrapper> OutputMapType;

  InputMapType inputs;
  OutputMapType outputs;

  void delete_this();
  mutable int use_count_;
};


namespace viennamesh
{
  class algorithm_template_t
  {
  public:

    void init(std::string const & algorithm_type_in,
              viennamesh_algorithm_make_function make_function_in,
              viennamesh_algorithm_delete_function delete_function_in,
              viennamesh_algorithm_init_function init_function_in,
              viennamesh_algorithm_run_function run_function_in)
    {
      algorithm_type_ = algorithm_type_in;

      make_function_ = make_function_in;
      delete_function_ = delete_function_in;

      init_function_ = init_function_in;
      run_function_ = run_function_in;
    }

    viennamesh_algorithm make_algorithm() const
    {
      viennamesh_algorithm algorithm;
      viennamesh_error result = make_function_(&algorithm);
      if (result != VIENNAMESH_SUCCESS)
        VIENNAMESH_ERROR(result, "Algorithm creation failed");
      return algorithm;
    }

    void delete_algorithm(viennamesh_algorithm algorithm) const
    {
      viennamesh_error result = delete_function_( algorithm );
      if (result != VIENNAMESH_SUCCESS)
        VIENNAMESH_ERROR(result, "Algorithm deletion failed");
    }

    void init(viennamesh_algorithm_wrapper algorithm) const
    {
      init_function_(algorithm);
    }

    void run(viennamesh_algorithm_wrapper algorithm) const
    {
      run_function_(algorithm);
    }

    viennamesh_context context() { return context_; }
    std::string const & type() const { return algorithm_type_; }
    void set_context(viennamesh_context context_in) { context_ = context_in; }

  private:
    viennamesh_context context_;

    std::string algorithm_type_;

    viennamesh_algorithm_make_function make_function_;
    viennamesh_algorithm_delete_function delete_function_;

    viennamesh_algorithm_init_function init_function_;
    viennamesh_algorithm_run_function run_function_;
  };
}

#endif
