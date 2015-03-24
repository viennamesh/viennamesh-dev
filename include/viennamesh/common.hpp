#ifndef _VIENNAMESH_COMMON_HPP_
#define _VIENNAMESH_COMMON_HPP_


#include <string>
#include "viennamesh/backend/api.h"
#include "viennamesh/forwards.hpp"
#include "viennamesh/backend/backend_common.hpp"



namespace viennamesh
{


  inline viennamesh_context internal_context(viennamesh_context ctx) { return ctx; }
  inline viennamesh_context internal_context(viennamesh_algorithm_wrapper algo)
  {
    viennamesh_context ctx;
    viennamesh_algorithm_get_context( algo, &ctx );
    return ctx;
  }
  inline viennamesh_context internal_context(viennamesh_data_wrapper data)
  {
    viennamesh_context ctx;
    viennamesh_data_get_context( data, &ctx );
    return ctx;
  }

  template<typename T>
  viennamesh_context internal_context(T const & something)
  {
    return internal_context( something.internal() );
  }

  template<typename T>
  void handle_error(viennamesh_error error, T const & something)
  {
    return handle_error(error, internal_context(something));
  }



  namespace result_of
  {
    template<typename T>
    struct c_type
    {
      typedef T type;
    };

    template<>
    struct c_type<std::string>
    {
      typedef viennamesh_string type;
    };

    template<>
    struct c_type<mesh_t>
    {
      typedef viennagrid_mesh type;
    };

    template<>
    struct c_type<quantity_field>
    {
      typedef viennagrid_quantity_field type;
    };

    template<>
    struct c_type<point_t>
    {
      typedef viennamesh_point type;
    };

    template<>
    struct c_type<seed_point_t>
    {
      typedef viennamesh_seed_point type;
    };




    template<typename T>
    struct cpp_type
    {
      typedef T type;
    };

    template<>
    struct cpp_type<viennamesh_string>
    {
      typedef std::string type;
    };

    template<>
    struct cpp_type<viennagrid_mesh>
    {
      typedef mesh_t type;
    };

    template<>
    struct cpp_type<viennagrid_quantity_field>
    {
      typedef quantity_field type;
    };

    template<>
    struct cpp_type<viennamesh_point>
    {
      typedef point_t type;
    };

    template<>
    struct cpp_type<viennamesh_seed_point>
    {
      typedef seed_point_t type;
    };






    template<typename T>
    struct cpp_result_type
    {
      typedef T const & type;
    };

    template<>
    struct cpp_result_type<viennamesh_string>
    {
      typedef std::string type;
    };

    template<>
    struct cpp_result_type<viennagrid_mesh>
    {
      typedef mesh_t type;
    };

    template<>
    struct cpp_result_type<viennagrid_quantity_field>
    {
      typedef quantity_field type;
    };

    template<>
    struct cpp_result_type<viennamesh_point>
    {
      typedef point_t type;
    };

    template<>
    struct cpp_result_type<viennamesh_seed_point>
    {
      typedef seed_point_t type;
    };


    template<typename DataT>
    struct data_handle
    {
      typedef viennamesh::data_handle<
        typename result_of::c_type<DataT>::type
      > type;
    };

    template<typename DataT>
    struct data_handle< viennamesh::data_handle<DataT> >
    {
      typedef viennamesh::data_handle<
        typename result_of::c_type<DataT>::type
      > type;
    };



    template<typename DataT>
    struct unpack_data
    {
      typedef typename result_of::c_type<DataT>::type type;
    };

    template<typename DataT>
    struct unpack_data< viennamesh::data_handle<DataT> >
    {
      typedef typename result_of::c_type<DataT>::type type;
    };
  }



  namespace result_of
  {
    template<typename T>
    struct data_information;

    template<>
    struct data_information<bool>
    {
      static std::string type_name() { return "bool"; }
      static viennamesh_data_make_function make_function() { return viennamesh::generic_make<bool>; }
      static viennamesh_data_delete_function delete_function() { return viennamesh::generic_delete<bool>; }
    };

    template<>
    struct data_information<int>
    {
      static std::string type_name() { return "int"; }
      static viennamesh_data_make_function make_function() { return viennamesh::generic_make<int>; }
      static viennamesh_data_delete_function delete_function() { return viennamesh::generic_delete<int>; }
    };

    template<>
    struct data_information<double>
    {
      static std::string type_name() { return "double"; }
      static viennamesh_data_make_function make_function() { return viennamesh::generic_make<double>; }
      static viennamesh_data_delete_function delete_function() { return viennamesh::generic_delete<double>; }
    };

    template<>
    struct data_information<viennamesh_string>
    {
      static std::string type_name() { return "viennamesh_string"; }
      static viennamesh_data_make_function make_function() { return viennamesh::make_string; }
      static viennamesh_data_delete_function delete_function() { return viennamesh::delete_string; }
    };

    template<>
    struct data_information<viennamesh_point>
    {
      static std::string type_name() { return "viennamesh_point"; }
      static viennamesh_data_make_function make_function() { return viennamesh::make_point; }
      static viennamesh_data_delete_function delete_function() { return viennamesh::delete_point; }
    };

    template<>
    struct data_information<viennamesh_seed_point>
    {
      static std::string type_name() { return "viennamesh_seed_point"; }
      static viennamesh_data_make_function make_function() { return viennamesh::make_seed_point; }
      static viennamesh_data_delete_function delete_function() { return viennamesh::delete_seed_point; }
    };

    template<>
    struct data_information<viennagrid_quantity_field>
    {
      static std::string type_name() { return "viennagrid_quantity_field"; }
      static viennamesh_data_make_function make_function() { return viennamesh::make_quantities; }
      static viennamesh_data_delete_function delete_function() { return viennamesh::delete_quantities; }
    };

    template<>
    struct data_information<viennagrid_mesh>
    {
      static std::string type_name() { return "viennagrid_mesh"; }
      static viennamesh_data_make_function make_function() { return viennamesh::make_mesh; }
      static viennamesh_data_delete_function delete_function() { return viennamesh::delete_mesh; }
    };
  }
}

#endif
