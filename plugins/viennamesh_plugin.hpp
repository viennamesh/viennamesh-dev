#ifndef _VIENNAMESH_PLUGIN_PLUGIN_HPP_
#define _VIENNAMESH_PLUGIN_PLUGIN_HPP_

#include "viennamesh/core.hpp"
#include "viennagrid/core.hpp"

namespace viennamesh
{
  namespace result_of
  {
    template<typename ValueT>
    struct unpack_data
    {
      typedef ValueT type;
    };

    template<typename ValueT>
    struct unpack_data< viennamesh::data_handle<ValueT> >
    {
      typedef ValueT type;
    };
  }


  class plugin_algorithm
  {
  public:

    typedef data_handle<viennagrid_mesh> mesh_handle;
    typedef data_handle<viennamesh_string> string_handle;
    typedef data_handle<viennamesh_point_container> point_container_handle;
    typedef data_handle<viennamesh_seed_point_container> seed_point_container_handle;


    bool init(viennamesh::algorithm_handle algorithm_in)
    {
      algorithm_wrapper = algorithm_in.internal();
      return true;
    }

    template<typename SomethingT>
    typename result_of::data_handle<SomethingT>::type make_data()
    { return algorithm().context().make_data<typename result_of::unpack_data<SomethingT>::type>(); }


    abstract_data_handle get_input(std::string const & name)
    { return algorithm().get_input(name); }

    template<typename SomethingT>
    typename result_of::data_handle<SomethingT>::type get_input(std::string const & name)
    { return algorithm().get_input<typename result_of::unpack_data<SomethingT>::type>(name); }

    template<typename SomethingT>
    typename result_of::data_handle<SomethingT>::type get_required_input(std::string const & name)
    {
      typename result_of::data_handle<SomethingT>::type result = algorithm().get_input<typename result_of::unpack_data<SomethingT>::type>(name);

      if (!result)
      {
        // TODO throw
      }

      return result;
    }


    void set_output(std::string const & name, abstract_data_handle data)
    { algorithm().set_output(name, data); }

  private:

    algorithm_handle algorithm() { return algorithm_handle(algorithm_wrapper); }
    viennamesh_algorithm_wrapper algorithm_wrapper;
  };


  template<typename AlgorithmT>
  int generic_make_algorithm(viennamesh_algorithm * algorithm)
  {
    AlgorithmT * tmp = new AlgorithmT();
    *algorithm = tmp;

    return VIENNAMESH_SUCCESS;
  }

  template<typename AlgorithmT>
  int generic_delete_algorithm(viennamesh_algorithm algorithm)
  {
    delete (AlgorithmT*)algorithm;
    return VIENNAMESH_SUCCESS;
  }


  template<typename AlgorithmT>
  int generic_algorithm_init(viennamesh_algorithm_wrapper algorithm)
  {
    viennamesh_algorithm internal_algorithm;
    viennamesh_algorithm_get_internal_algorithm(algorithm, &internal_algorithm);

    if (!((AlgorithmT*)internal_algorithm)->init( viennamesh::algorithm_handle(algorithm) ))
      return VIENNAMESH_ERROR_ALGORITHM_RUN_FAILED;

    return VIENNAMESH_SUCCESS;
  }


  template<typename AlgorithmT>
  int generic_algorithm_run(viennamesh_algorithm_wrapper algorithm)
  {
    viennamesh_algorithm internal_algorithm;
    viennamesh_algorithm_get_internal_algorithm(algorithm, &internal_algorithm);

    {
      const char * name_;
      viennamesh_algorithm_get_name(algorithm, &name_);
      viennamesh::LoggingStack stack( std::string("Running algorithm \"") + name_ + "\"");

      viennamesh::algorithm_handle algorithm_handle(algorithm);

      if (!((AlgorithmT*)internal_algorithm)->run(algorithm_handle))
        return VIENNAMESH_ERROR_ALGORITHM_RUN_FAILED;
    }

    return VIENNAMESH_SUCCESS;
  }








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




  template<typename FromT, typename ToT>
  int generic_convert(viennamesh_data from_, viennamesh_data to_)
  {
    *static_cast<ToT*>(to_) = *static_cast<FromT*>(from_);
    return VIENNAMESH_SUCCESS;
  }



  template<typename T>
  void register_data_type(context_handle & ctx)
  {
    ctx.register_data_type<T>( generic_make<T>, generic_delete<T> );
  }

  template<typename FromT, typename ToT>
  void register_convert(context_handle & ctx)
  {
    ctx.register_conversion<FromT,ToT>( generic_convert<FromT,ToT> );
  }



  template<typename AlgorithmT>
  void register_algorithm( context_handle & ctx )
  {
    ctx.register_algorithm( AlgorithmT::name(),
                            generic_make_algorithm<AlgorithmT>,
                            generic_delete_algorithm<AlgorithmT>,
                            generic_algorithm_init<AlgorithmT>,
                            generic_algorithm_run<AlgorithmT> );
  }


  std::string local_binary_format();
  void plugin_init( context_handle & context );
}


DYNAMIC_EXPORT int viennamesh_plugin_init(viennamesh_context ctx_);
DYNAMIC_EXPORT int viennamesh_version();


#endif
