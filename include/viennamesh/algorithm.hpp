#ifndef _VIENNAMESH_ALGORITHM_HPP_
#define _VIENNAMESH_ALGORITHM_HPP_

#include <cstdlib>
#include <cstring>

#include "viennamesh/forwards.hpp"
#include "viennamesh/context.hpp"
#include "viennamesh/data.hpp"
#include "viennamesh/logger.hpp"
#include "viennamesh/exceptions.hpp"
#include "viennamesh/utils/string_tools.hpp"

using stringtools::lexical_cast;

namespace viennamesh
{

  class algorithm_handle
  {
    friend class context_handle;

  public:

    algorithm_handle() : algorithm(0) {}
    algorithm_handle(algorithm_handle const & handle_) : algorithm(handle_.algorithm)
    {
      retain();
    }
    algorithm_handle(viennamesh_algorithm_wrapper algorithm_) : algorithm(algorithm_)
    {
      retain();
    }

    ~algorithm_handle()
    {
      release();
    }

    algorithm_handle & operator=(algorithm_handle const & handle_)
    {
      release();
      algorithm = handle_.algorithm;
      retain();
      return *this;
    }

    bool valid() const
    {
      return algorithm != NULL;
    }


    void set_default_source(algorithm_handle const & source_algorithm)
    {
      viennamesh_algorithm_set_default_source( internal(), source_algorithm.internal() );
    }

    void unset_default_source()
    {
      viennamesh_algorithm_unset_default_source( internal() );
    }




    void set_input(std::string const & name, abstract_data_handle const & data)
    {
      viennamesh_algorithm_set_input(algorithm, name.c_str(), data.internal());
    }

    template<typename T>
    void set_input(std::string const & name, data_handle<T> const & data)
    {
      viennamesh_algorithm_set_input(algorithm, name.c_str(), data.internal());
    }

    template<typename T>
    void set_input(std::string const & name, T data)
    {
      data_handle<T> tmp = context().make_data<T>();
      tmp.set(data);
      set_input(name, tmp);
    }

    void set_input(std::string const & name, const char * string)
    {
      data_handle<viennamesh_string> tmp = context().make_data<viennamesh_string>();
      tmp.set(string);
      set_input(name, tmp);
    }

    void set_input(std::string const & name, std::string const & string)
    {
      set_input(name, string.c_str());
    }

    void link_input(std::string const & name, algorithm_handle const & source_algorithm, std::string const & source_name)
    {
      viennamesh_algorithm_link_input( internal(), name.c_str(), source_algorithm.internal(), source_name.c_str() );
    }

    abstract_data_handle get_input(std::string const & name)
    {
      viennamesh_data_wrapper data_;
      viennamesh_algorithm_get_input(algorithm, name.c_str(), &data_);

      return abstract_data_handle(data_);
    }

    abstract_data_handle get_required_input(std::string const & name)
    {
      abstract_data_handle result = get_input(name);

      if (!result.valid())
      {
        throw exception::input_parameter_not_found("Required input \"" + name + "\" is not present.");
      }

      return result;
    }



    template<typename DataT>
    data_handle< typename result_of::unpack_data<DataT>::type > get_input(std::string const & name)
    {
      typedef typename result_of::unpack_data<DataT>::type UnpackedDataType;

      viennamesh_data_wrapper data_;
      viennamesh_algorithm_get_input_with_type(algorithm, name.c_str(),
                                               result_of::data_information<UnpackedDataType>::type_name().c_str(),
                                               &data_);

      return data_handle<UnpackedDataType>(data_, false);
    }

    template<typename DataT>
    data_handle< typename result_of::unpack_data<DataT>::type > get_required_input(std::string const & name)
    {
      data_handle< typename result_of::unpack_data<DataT>::type > result = get_input<DataT>(name);

      if (!result.valid())
      {
        throw exception::input_parameter_not_of_requested_type_and_not_convertable("Required input \"" + name + "\" is not present or not of convertable type.");
      }

      return result;
    }



    void set_output(std::string const & name, abstract_data_handle data)
    {
      viennamesh_algorithm_set_output(algorithm, name.c_str(), data.internal());
    }

    abstract_data_handle get_output(std::string const & name)
    {
      viennamesh_data_wrapper data_;
      viennamesh_algorithm_get_output(algorithm, name.c_str(), &data_);

      return abstract_data_handle(data_);
    }

    template<typename DataT>
    data_handle<DataT> get_output(std::string const & name)
    {
      viennamesh_data_wrapper data_;
      viennamesh_algorithm_get_output_with_type(algorithm, name.c_str(),
                                               result_of::data_information<DataT>::type_name().c_str(),
                                               &data_);

      return data_handle<DataT>(data_, true);
    }




    void init()
    {
      viennamesh_algorithm_init(algorithm);
    }

    bool run()
    {
      viennamesh_algorithm_run(algorithm);
      return true;
    }

    context_handle context();

    viennamesh_algorithm_wrapper internal() const { return const_cast<viennamesh_algorithm_wrapper>(algorithm); }
    std::string name() const
    {
      const char * name_;
      viennamesh_algorithm_get_name(internal(), &name_);
      return name_;
    }


    std::string base_path() const
    {
      const char * base_path_;
      viennamesh_algorithm_get_base_path(algorithm, &base_path_);
      return std::string(base_path_);
    }

    void set_base_path(std::string const & base_path_)
    {
      viennamesh_algorithm_set_base_path(algorithm, base_path_.c_str());
    }



  private:

    void retain()
    {
      if (algorithm)
        viennamesh_algorithm_retain(algorithm);
    }

    void release()
    {
      if (algorithm)
        viennamesh_algorithm_release(algorithm);
    }

    void make(viennamesh_context context, std::string const & algorithm_name)
    {
      release();
      viennamesh_error err = viennamesh_algorithm_make(context, algorithm_name.c_str(), &algorithm);
      if (err != VIENNAMESH_SUCCESS)
        throw exception::base_exception("Creating algorithm \"" + algorithm_name + "\" failed");

      init();
    }

    viennamesh_algorithm_wrapper algorithm;
  };





  template<typename AlgorithmT>
  viennamesh_error generic_make_algorithm(viennamesh_algorithm * algorithm)
  {
    AlgorithmT * tmp = new AlgorithmT();
    *algorithm = tmp;

    return VIENNAMESH_SUCCESS;
  }

  template<typename AlgorithmT>
  viennamesh_error generic_delete_algorithm(viennamesh_algorithm algorithm)
  {
    delete (AlgorithmT*)algorithm;
    return VIENNAMESH_SUCCESS;
  }


  template<typename AlgorithmT>
  viennamesh_error generic_algorithm_init(viennamesh_algorithm_wrapper algorithm)
  {
    viennamesh_algorithm internal_algorithm;
    viennamesh_algorithm_get_internal_algorithm(algorithm, &internal_algorithm);

    if (!((AlgorithmT*)internal_algorithm)->init( viennamesh::algorithm_handle(algorithm) ))
      return VIENNAMESH_ERROR_ALGORITHM_RUN_FAILED;

    return VIENNAMESH_SUCCESS;
  }


  template<typename AlgorithmT>
  viennamesh_error generic_algorithm_run(viennamesh_algorithm_wrapper algorithm)
  {
    viennamesh_algorithm internal_algorithm;
    viennamesh_algorithm_get_internal_algorithm(algorithm, &internal_algorithm);

    {
      const char * name_;
      viennamesh_algorithm_get_name(algorithm, &name_);
      viennamesh::LoggingStack stack( std::string("Running algorithm \"") + name_ + "\"");

      viennamesh::algorithm_handle algorithm_handle(algorithm);

      try
      {
        ((AlgorithmT*)internal_algorithm)->run(algorithm_handle);
      }
      catch (exception::base_exception const & ex)
      {
        error(1) << "Algorithm failed: " << ex.what() << std::endl;
        return VIENNAMESH_ERROR_ALGORITHM_RUN_FAILED;
      }
    }

    return VIENNAMESH_SUCCESS;
  }

}

#endif
