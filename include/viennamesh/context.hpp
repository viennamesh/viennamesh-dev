#ifndef _VIENNAMESH_CONTEXT_HPP_
#define _VIENNAMESH_CONTEXT_HPP_

#include <vector>

#include "viennamesh/forwards.hpp"

namespace viennamesh
{
  class context_handle
  {
  public:

    context_handle() : ctx(0)
    {
      make();
      load_plugins_in_directories(VIENNAMESH_DEFAULT_PLUGIN_DIRECTORY, ";");
    }
    context_handle(viennamesh_context ctx_) : ctx(ctx_) { retain(); }
    context_handle(context_handle const & handle_) : ctx(handle_.ctx) { retain(); }

    ~context_handle()
    {
      release();
    }

    context_handle & operator=(context_handle const & handle_)
    {
      release();
      ctx = handle_.ctx;
      retain();
      return *this;
    }

    void register_data_type(std::string const & data_type,
                            viennamesh_data_make_function make_function,
                            viennamesh_data_delete_function delete_function)
    {
      viennamesh_data_type_register(ctx, data_type.c_str(), make_function, delete_function);
    }

    template<typename DataT>
    void register_data_type(viennamesh_data_make_function make_function,
                            viennamesh_data_delete_function delete_function)
    {
      register_data_type(result_of::data_information<DataT>::type_name(),
                         make_function, delete_function);
    }

    void register_conversion(std::string const & data_type_from,
                             std::string const & data_type_to,
                             viennamesh_data_convert_function convert_function)
    {
      viennamesh_data_conversion_register(ctx, data_type_from.c_str(), data_type_to.c_str(), convert_function);
    }

    template<typename FromT, typename ToT>
    void register_conversion(viennamesh_data_convert_function convert_function)
    {
      register_conversion(result_of::data_information<FromT>::type_name(),
                          result_of::data_information<ToT>::type_name(),
                          convert_function);
    }


    template<typename DataT>
    data_handle<DataT> make_data()
    {
      data_handle<DataT> tmp;
      tmp.make(ctx);
      return tmp;
    }







    void register_algorithm(std::string const & algorithm_name,
                            viennamesh_algorithm_make_function make_function,
                            viennamesh_algorithm_delete_function delete_function,
                            viennamesh_algorithm_init_function init_function,
                            viennamesh_algorithm_run_function run_function)
    {
      viennamesh_algorithm_register(ctx, algorithm_name.c_str(),
                                    make_function, delete_function,
                                    init_function, run_function);
    }


    algorithm_handle make_algorithm(std::string const & algorithm_name);
    void load_plugin(std::string const & plugin_filename);
    void load_plugins_in_directories(std::string const & directory_name, std::string const & delimiter);

    viennamesh_context internal() const { return const_cast<viennamesh_context>(ctx); }

  private:

    void retain()
    {
      if (ctx)
        viennamesh_context_retain(ctx);
    }

    void release()
    {
      if (ctx)
        viennamesh_context_release(ctx);
    }


    void make()
    {
      if (ctx)
        viennamesh_context_retain(ctx);
      viennamesh_context_make(&ctx);
    }

    viennamesh_context ctx;
    std::vector<void *> loaded_plugins;
  };










}

#endif
