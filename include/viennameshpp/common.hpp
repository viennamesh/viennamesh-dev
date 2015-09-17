#ifndef _VIENNAMESH_COMMON_HPP_
#define _VIENNAMESH_COMMON_HPP_

#include "viennameshpp/forwards.hpp"

#include "viennamesh/viennamesh.h"
#include "viennamesh/cpp_error.hpp"

#include <string>


namespace viennamesh
{
  template<typename T>
  viennamesh_error generic_make(viennamesh_data * data)
  {
    T * tmp = new T;
    *data = tmp;
    return VIENNAMESH_SUCCESS;
  }

  template<typename T>
  viennamesh_error generic_delete(viennamesh_data data)
  {
    delete (T*)data;
    return VIENNAMESH_SUCCESS;
  }


  template<typename FromT, typename ToT>
  viennamesh_error internal_convert(FromT const & from_, ToT & to_);

  inline viennamesh_error internal_convert(int from, double & to)
  {
    to = from;
    return VIENNAMESH_SUCCESS;
  }

  inline viennamesh_error internal_convert(double from, int & to)
  {
    to = from+0.5;
    return VIENNAMESH_SUCCESS;
  }

  template<typename FromT, typename ToT>
  viennamesh_error generic_convert(viennamesh_data from_, viennamesh_data to_)
  {
    return internal_convert(*static_cast<FromT const *>(from_), *static_cast<ToT*>(to_));
  }









  template<typename DataT>
  viennamesh_error trivial_data_make(DataT * data)
  {
    *data = new DataT;
    return VIENNAMESH_SUCCESS;
  }

  template<typename DataT>
  viennamesh_error trivial_data_delete(DataT data)
  {
    delete data;
    return VIENNAMESH_SUCCESS;
  }



  template<typename DataT, typename MakeFunctionT>
  viennamesh_error make_viennamesh_data(viennamesh_data * data, MakeFunctionT make_function_)
  {
    DataT * internal_data = new DataT;
    make_function_(internal_data);
    *data = internal_data;

    return VIENNAMESH_SUCCESS;
  }

  template<typename DataT, typename MakeFunctionT>
  viennamesh_error delete_viennamesh_data(viennamesh_data data, MakeFunctionT free_function_)
  {
    DataT * internal_data = (DataT*)data;
    free_function_(*internal_data);
    delete internal_data;

    return VIENNAMESH_SUCCESS;
  }



  inline viennamesh_error make_string(viennamesh_data * data)
  { return make_viennamesh_data<viennamesh_string>(data, viennamesh_string_make); }
  inline viennamesh_error delete_string(viennamesh_data data)
  { return delete_viennamesh_data<viennamesh_string>(data, viennamesh_string_delete); }


  inline viennamesh_error make_point(viennamesh_data * data)
  { return make_viennamesh_data<viennamesh_point>(data, viennamesh_point_make); }
  inline viennamesh_error delete_point(viennamesh_data data)
  { return delete_viennamesh_data<viennamesh_point>(data, viennamesh_point_delete); }

  inline viennamesh_error make_seed_point(viennamesh_data * data)
  { return make_viennamesh_data<viennamesh_seed_point>(data, viennamesh_seed_point_make); }
  inline viennamesh_error delete_seed_point(viennamesh_data data)
  { return delete_viennamesh_data<viennamesh_seed_point>(data, viennamesh_seed_point_delete); }

  inline viennamesh_error make_quantities(viennamesh_data * data)
  { return make_viennamesh_data<viennagrid_quantity_field>(data, viennagrid_quantity_field_create); }
  inline viennamesh_error delete_quantities(viennamesh_data data)
  { return delete_viennamesh_data<viennagrid_quantity_field>(data, viennagrid_quantity_field_release); }

  inline viennamesh_error make_mesh(viennamesh_data * data)
  {
    viennagrid_mesh * mesh = new viennagrid_mesh;
    viennagrid_mesh_create(mesh);
    viennagrid_mesh_property_set(*mesh, VIENNAGRID_PROPERTY_BOUNDARY_LAYOUT, VIENNAGRID_BOUNDARY_LAYOUT_SPARSE);

    *data = mesh;

    return VIENNAMESH_SUCCESS;
  }

  inline viennamesh_error delete_mesh(viennamesh_data data)
  {
    viennagrid_mesh * mesh = (viennagrid_mesh *)data;
    viennagrid_mesh_release(*mesh);

    delete mesh;

    return VIENNAMESH_SUCCESS;
  }

  inline viennamesh_error make_plc(viennamesh_data * data)
  {
    viennagrid_plc * plc = new viennagrid_plc;
    viennagrid_plc_create(plc);

    *data = plc;

    return VIENNAMESH_SUCCESS;
  }

  inline viennamesh_error delete_plc(viennamesh_data data)
  {
    viennagrid_plc * plc = (viennagrid_plc *)data;

    viennagrid_plc_release(*plc);
    delete plc;

    return VIENNAMESH_SUCCESS;
  }



  std::string extract_filename( std::string const & path );
  std::string extract_path( std::string const & path );

}






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
    struct c_type<mesh>
    {
      typedef viennagrid_mesh type;
    };

    template<>
    struct c_type<quantity_field>
    {
      typedef viennagrid_quantity_field type;
    };

    template<>
    struct c_type<point>
    {
      typedef viennamesh_point type;
    };

    template<>
    struct c_type<seed_point>
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
      typedef mesh type;
    };

    template<>
    struct cpp_type<viennagrid_quantity_field>
    {
      typedef quantity_field type;
    };

    template<>
    struct cpp_type<viennamesh_point>
    {
      typedef point type;
    };

    template<>
    struct cpp_type<viennamesh_seed_point>
    {
      typedef seed_point type;
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
      typedef mesh type;
    };

    template<>
    struct cpp_result_type<viennagrid_quantity_field>
    {
      typedef quantity_field type;
    };

    template<>
    struct cpp_result_type<viennamesh_point>
    {
      typedef point type;
    };

    template<>
    struct cpp_result_type<viennamesh_seed_point>
    {
      typedef seed_point type;
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

    template<>
    struct data_information<viennagrid_plc>
    {
      static std::string type_name() { return "viennagrid_plc"; }
      static viennamesh_data_make_function make_function() { return viennamesh::make_plc; }
      static viennamesh_data_delete_function delete_function() { return viennamesh::delete_plc; }
    };
  }
}

#endif
