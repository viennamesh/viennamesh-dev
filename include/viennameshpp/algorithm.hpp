#ifndef _VIENNAMESH_ALGORITHM_HPP_
#define _VIENNAMESH_ALGORITHM_HPP_

#include <cstdlib>
#include <cstring>

#include "viennameshpp/forwards.hpp"
#include "viennameshpp/context.hpp"
#include "viennameshpp/data.hpp"
#include "viennameshpp/logger.hpp"
#include "viennameshpp/exceptions.hpp"

#include "viennamesh/cpp_error.hpp"


// #include "viennamesh/backend/backend_forwards.hpp"

namespace viennamesh
{
  class algorithm_handle
  {
    friend class context_handle;

  public:

    algorithm_handle();
    algorithm_handle(algorithm_handle const & handle_);
    algorithm_handle(viennamesh_algorithm_wrapper algorithm_);

    ~algorithm_handle();

    algorithm_handle & operator=(algorithm_handle const & handle_);


    bool valid() const;

    void set_default_source(algorithm_handle const & source_algorithm);
    void unset_default_source();


    void set_input(std::string const & name, abstract_data_handle const & data);
    template<typename T>
    void set_input(std::string const & name, data_handle<T> const & data)
    { handle_error(viennamesh_algorithm_set_input(algorithm, name.c_str(), data.internal()), algorithm); }
    template<typename T>
    void set_input(std::string const & name, T data)
    {
      typename result_of::data_handle<T>::type tmp = context().make_data<T>();
      tmp.set(data);
      set_input(name, tmp);
    }


    template<typename T>
    void push_back_input(std::string const & name, T data)
    {
      typedef typename result_of::c_type<T>::type CType;

      typename result_of::data_handle<T>::type tmp = get_input<T>(name);

      if (!tmp.valid())
      {
        set_input(name, data);
        return;
      }

      if (tmp.type_name() != result_of::data_information<CType>::type_name())
      {
        VIENNAMESH_ERROR(VIENNAMESH_ERROR_INPUT_PARAMETER_PUSH_BACK_FAILED, std::string("Input ") + name + " is of type " + tmp.type_name() + ", data to push is of type " + result_of::data_information<CType>::type_name());
      }

      tmp.push_back(data);
    }


    void set_input(std::string const & name, const char * string);
    void set_input(std::string const & name, std::string const & string);
    void unset_input(std::string const & name)
    {
      handle_error(
        viennamesh_algorithm_unset_input(algorithm, name.c_str()),
        algorithm);
    }

    void link_input(std::string const & name, algorithm_handle const & source_algorithm, std::string const & source_name);

    abstract_data_handle get_input(std::string const & name);
    abstract_data_handle get_required_input(std::string const & name);



    template<typename DataT>
    data_handle< typename result_of::unpack_data<DataT>::type > get_input(std::string const & name)
    {
      typedef typename result_of::unpack_data<DataT>::type UnpackedDataType;

      viennamesh_data_wrapper data_;
      handle_error(
        viennamesh_algorithm_get_input_with_type(algorithm, name.c_str(),
                                               result_of::data_information<UnpackedDataType>::type_name().c_str(),
                                               &data_),
        algorithm);

      return data_handle<UnpackedDataType>(data_, false);
    }

    template<typename DataT>
    data_handle< typename result_of::unpack_data<DataT>::type > get_required_input(std::string const & name)
    {
      data_handle< typename result_of::unpack_data<DataT>::type > result = get_input<DataT>(name);

      if (!result.valid())
      {
        VIENNAMESH_ERROR(VIENNAMESH_ERROR_REQUIRED_INPUT_PARAMETER_NOT_FOUND_OR_NOT_CONVERTABLE, "Required input \"" + name + "\" is not present or not of convertable type.");
      }

      return result;
    }


    void clear_inputs()
    {
      handle_error(
        viennamesh_algorithm_clear_inputs(algorithm),
        algorithm);
    }

    void clear_outputs()
    {
      handle_error(
        viennamesh_algorithm_clear_outputs(algorithm),
        algorithm);
    }


    void set_output(std::string const & name, abstract_data_handle data);
    abstract_data_handle get_output(std::string const & name);

    template<typename DataT>
    data_handle< typename result_of::unpack_data<DataT>::type > get_output(std::string const & name)
    {
      typedef typename result_of::unpack_data<DataT>::type UnpackedDataType;

      viennamesh_data_wrapper data_;
      handle_error(
        viennamesh_algorithm_get_output_with_type(algorithm, name.c_str(),
                                               result_of::data_information<UnpackedDataType>::type_name().c_str(),
                                               &data_),
        algorithm);

      return data_handle<UnpackedDataType>(data_, true);
    }


    void init();
    bool run();

    context_handle context();

    viennamesh_algorithm_wrapper internal() const;
    std::string type() const;


    std::string base_path() const;
    void set_base_path(std::string const & base_path_);

  private:

    void retain();
    void release();
    void make(viennamesh_context context, std::string const & algorithm_name);

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
    viennamesh_error err = viennamesh_algorithm_get_internal_algorithm(algorithm, &internal_algorithm);
    if (err != VIENNAMESH_SUCCESS)
      return err;

    {
      const char * type_;
      err = viennamesh_algorithm_get_type(algorithm, &type_);
      if (err != VIENNAMESH_SUCCESS)
        return err;

      viennamesh::algorithm_handle algorithm_handle(algorithm);

      try
      {
        ((AlgorithmT*)internal_algorithm)->run(algorithm_handle);
      }
      catch (...)
      {
        viennamesh_context context;
        viennamesh_algorithm_get_context(algorithm, &context);
        return handle_error(context);
      }
    }

    return VIENNAMESH_SUCCESS;
  }

}

#endif
