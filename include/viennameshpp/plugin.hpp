#ifndef _VIENNAMESH_PLUGIN_PLUGIN_HPP_
#define _VIENNAMESH_PLUGIN_PLUGIN_HPP_

#include "viennagrid/viennagrid.hpp"
#include "viennameshpp/core.hpp"
#include "viennameshpp/exceptions.hpp"

namespace viennamesh
{

  class plugin_algorithm
  {
  public:

    typedef data_handle<viennagrid_mesh> mesh_handle;
    typedef data_handle<viennagrid_quantity_field> quantity_field_handle;

    typedef data_handle<viennamesh_string> string_handle;
    typedef data_handle<viennamesh_point> point_handle;
    typedef data_handle<viennamesh_seed_point> seed_point_handle;


    bool init(viennamesh::algorithm_handle algorithm_in)
    {
      algorithm_wrapper = algorithm_in.internal();
      return true;
    }

    template<typename DataT>
    typename result_of::data_handle<DataT>::type make_data()
    { return algorithm().context().make_data<DataT>(); }

    template<typename DataT>
    typename result_of::data_handle<DataT>::type make_data(DataT const & data)
    { return algorithm().context().make_data<DataT>(data); }



    abstract_data_handle get_input(std::string const & name)
    { return algorithm().get_input(name); }

    abstract_data_handle get_required_input(std::string const & name)
    { return algorithm().get_required_input(name); }

    template<typename DataT>
    typename result_of::data_handle<DataT>::type get_input(std::string const & name)
    { return algorithm().get_input<typename result_of::unpack_data<DataT>::type>(name); }

    template<typename DataT>
    typename result_of::data_handle<DataT>::type get_required_input(std::string const & name)
    { return algorithm().get_required_input<DataT>(name); }



    void set_output(std::string const & name, abstract_data_handle data)
    { algorithm().set_output(name, data); }

    template<typename DataT>
    void set_output(std::string const & name, data_handle<DataT> data)
    { set_output(name, static_cast<abstract_data_handle>(data) ); }

    template<typename DataT>
    void set_output(std::string const & name, DataT data)
    { set_output( name, make_data(data) ); }



    std::string base_path()
    { return algorithm().base_path(); }

    context_handle context() { return algorithm().context(); }


  private:

    algorithm_handle algorithm() { return algorithm_handle(algorithm_wrapper); }
    viennamesh_algorithm_wrapper algorithm_wrapper;
  };

  void plugin_init( context_handle & context );
  std::string plugin_name();
}


DYNAMIC_EXPORT int viennamesh_version();
DYNAMIC_EXPORT viennamesh_error viennamesh_plugin_init(viennamesh_context ctx_);



#endif
