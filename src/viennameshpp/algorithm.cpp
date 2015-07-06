#include "viennameshpp/algorithm.hpp"
#include "viennameshpp/context.hpp"

namespace viennamesh
{

  algorithm_handle::algorithm_handle() : algorithm(0) {}
  algorithm_handle::algorithm_handle(algorithm_handle const & handle_) : algorithm(handle_.algorithm)
  {
    retain();
  }
  algorithm_handle::algorithm_handle(viennamesh_algorithm_wrapper algorithm_) : algorithm(algorithm_)
  {
    retain();
  }

  algorithm_handle::~algorithm_handle()
  {
    release();
  }


  algorithm_handle & algorithm_handle::operator=(algorithm_handle const & handle_)
  {
    release();
    algorithm = handle_.algorithm;
    retain();
    return *this;
  }

  bool algorithm_handle::valid() const
  {
    return algorithm != NULL;
  }


  void algorithm_handle::set_default_source(algorithm_handle const & source_algorithm)
  {
    handle_error(viennamesh_algorithm_set_default_source( internal(), source_algorithm.internal() ), algorithm);
  }

  void algorithm_handle::unset_default_source()
  {
    handle_error(viennamesh_algorithm_unset_default_source( internal() ), algorithm);
  }




  void algorithm_handle::set_input(std::string const & name, abstract_data_handle const & data)
  {
    handle_error(viennamesh_algorithm_set_input(algorithm, name.c_str(), data.internal()), algorithm);
  }

  void algorithm_handle::set_input(std::string const & name, const char * string)
  {
    data_handle<viennamesh_string> tmp = context().make_data<viennamesh_string>();
    tmp.set(string);
    set_input(name, tmp);
  }

  void algorithm_handle::set_input(std::string const & name, std::string const & string)
  {
    set_input(name, string.c_str());
  }

  void algorithm_handle::link_input(std::string const & name, algorithm_handle const & source_algorithm, std::string const & source_name)
  {
    handle_error(viennamesh_algorithm_link_input( internal(), name.c_str(), source_algorithm.internal(), source_name.c_str() ), algorithm);
  }

  abstract_data_handle algorithm_handle::get_input(std::string const & name)
  {
    viennamesh_data_wrapper data_;
    handle_error(viennamesh_algorithm_get_input(algorithm, name.c_str(), &data_), algorithm);

    return abstract_data_handle(data_);
  }

  abstract_data_handle algorithm_handle::get_required_input(std::string const & name)
  {
    abstract_data_handle result = get_input(name);

    if (!result.valid())
    {
      VIENNAMESH_ERROR(VIENNAMESH_ERROR_REQUIRED_INPUT_PARAMETER_NOT_FOUND, "Required input \"" + name + "\" is not present.");
    }

    return result;
  }


  void algorithm_handle::set_output(std::string const & name, abstract_data_handle data)
  {
    handle_error(viennamesh_algorithm_set_output(algorithm, name.c_str(), data.internal()), algorithm);
  }

  abstract_data_handle algorithm_handle::get_output(std::string const & name)
  {
    viennamesh_data_wrapper data_;
    handle_error(viennamesh_algorithm_get_output(algorithm, name.c_str(), &data_), algorithm);

    return abstract_data_handle(data_);
  }



  void algorithm_handle::init()
  {
    handle_error(viennamesh_algorithm_init(algorithm), algorithm);
  }

  bool algorithm_handle::run()
  {
    handle_error(viennamesh_algorithm_run(algorithm), algorithm);
    return true;
  }

  context_handle algorithm_handle::context()
  {
    return context_handle( internal_context(*this) );
  }

  viennamesh_algorithm_wrapper algorithm_handle::internal() const
  {
    return const_cast<viennamesh_algorithm_wrapper>(algorithm);
  }

  std::string algorithm_handle::type() const
  {
    const char * type_;
    handle_error(viennamesh_algorithm_get_type(internal(), &type_), algorithm);
    return type_;
  }


  std::string algorithm_handle::base_path() const
  {
    const char * base_path_;
    handle_error(viennamesh_algorithm_get_base_path(algorithm, &base_path_), algorithm);
    return std::string(base_path_);
  }

  void algorithm_handle::set_base_path(std::string const & base_path_)
  {
    handle_error(viennamesh_algorithm_set_base_path(algorithm, base_path_.c_str()), algorithm);
  }

  void algorithm_handle::retain()
  {
    if (algorithm)
      handle_error( viennamesh_algorithm_retain(algorithm), algorithm );
  }

  void algorithm_handle::release()
  {
    if (algorithm)
    {
      viennamesh_algorithm_release(algorithm);
    }
  }

  void algorithm_handle::make(viennamesh_context context, std::string const & algorithm_name)
  {
    release();
    handle_error(viennamesh_algorithm_make(context, algorithm_name.c_str(), &algorithm), context);
    init();
  }

}
