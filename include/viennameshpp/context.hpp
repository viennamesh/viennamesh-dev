#ifndef _VIENNAMESH_CONTEXT_HPP_
#define _VIENNAMESH_CONTEXT_HPP_

#include <vector>

#include "viennameshpp/forwards.hpp"
#include "viennameshpp/common.hpp"
// #include "viennamesh/backend/backend_common.hpp"

namespace viennamesh
{
  template<typename AlgorithmT>
  viennamesh_error generic_make_algorithm(viennamesh_algorithm * algorithm);

  template<typename AlgorithmT>
  viennamesh_error generic_delete_algorithm(viennamesh_algorithm algorithm);

  template<typename AlgorithmT>
  viennamesh_error generic_algorithm_init(viennamesh_algorithm_wrapper algorithm);

  template<typename AlgorithmT>
  viennamesh_error generic_algorithm_run(viennamesh_algorithm_wrapper algorithm);



  class context_handle
  {
  public:

    context_handle();
    context_handle(viennamesh_context ctx_);
    context_handle(context_handle const & handle_);

    ~context_handle();

    context_handle & operator=(context_handle const & handle_);

    void register_data_type(std::string const & data_type,
                            viennamesh_data_make_function make_function,
                            viennamesh_data_delete_function delete_function);

    template<typename DataT>
    void register_data_type()
    {
      register_data_type(result_of::data_information<DataT>::type_name(),
                         result_of::data_information<DataT>::make_function(),
                         result_of::data_information<DataT>::delete_function());
    }

    void register_conversion(std::string const & data_type_from,
                             std::string const & data_type_to,
                             viennamesh_data_convert_function convert_function);

    template<typename FromT, typename ToT>
    void register_conversion(viennamesh_data_convert_function convert_function)
    {
      register_conversion(result_of::data_information<FromT>::type_name(),
                          result_of::data_information<ToT>::type_name(),
                          convert_function);
    }

    template<typename FromT, typename ToT>
    void register_conversion()
    {
      register_conversion<FromT, ToT>(generic_convert<FromT, ToT> );
    }


    template<typename DataT>
    typename result_of::data_handle<DataT>::type make_data()
    {
      typename result_of::data_handle<DataT>::type tmp;
      tmp.make(ctx);
      return tmp;
    }

    template<typename DataT, typename DataToCopyT>
    typename result_of::data_handle<DataT>::type make_data(DataToCopyT const & data)
    {
      typedef typename result_of::data_handle<DataT>::type HandleType;
      typedef typename result_of::c_type<DataT>::type CType;

      HandleType handle = make_data<CType>();
      handle.set( data );
      return handle;
    }






    void register_algorithm(std::string const & algorithm_name,
                            viennamesh_algorithm_make_function make_function,
                            viennamesh_algorithm_delete_function delete_function,
                            viennamesh_algorithm_init_function init_function,
                            viennamesh_algorithm_run_function run_function);

    template<typename AlgorithmT>
    void register_algorithm()
    {
      register_algorithm( AlgorithmT::name(),
                          generic_make_algorithm<AlgorithmT>,
                          generic_delete_algorithm<AlgorithmT>,
                          generic_algorithm_init<AlgorithmT>,
                          generic_algorithm_run<AlgorithmT> );
    }


    algorithm_handle make_algorithm(std::string const & algorithm_name);
    void load_plugin(std::string const & plugin_filename);
    void load_plugins_in_directories(std::string const & directory_name, std::string const & delimiter);

    viennamesh_context internal() const;

  private:

    void retain();
    void release();
    void make();

    viennamesh_context ctx;
    std::vector<void *> loaded_plugins;
  };

  inline viennamesh_context internal_context(context_handle const & ctx)
  {
    return ctx.internal();
  }



  template<typename AlgorithmT>
  void register_algorithm(context_handle const & context)
  {
    context.register_algorithm<AlgorithmT>();
  }

  template<typename AlgorithmT>
  void register_algorithm(viennamesh_context ctx)
  {
    context_handle context(ctx);
    context.register_algorithm<AlgorithmT>();
  }



  template<typename DataT>
  void register_data_type(context_handle const & context)
  {
    context.register_data_type<DataT>();
  }

  template<typename DataT>
  void register_data_type(viennamesh_context ctx)
  {
    context_handle context(ctx);
    context.register_data_type<DataT>();
  }



  template<typename FromT, typename ToT>
  void register_conversion(context_handle const & context)
  {
    context.register_conversion<FromT, ToT>();
  }

  template<typename FromT, typename ToT>
  void register_conversion(viennamesh_context ctx)
  {
    context_handle context(ctx);
    context.register_conversion<FromT, ToT>();
  }



}

#endif
