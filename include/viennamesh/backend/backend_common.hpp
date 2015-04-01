#ifndef _VIENNAMESH_BACKEND_COMMON_HPP_
#define _VIENNAMESH_BACKEND_COMMON_HPP_

#include <ostream>
#include "viennamesh/backend/backend_forwards.hpp"

namespace viennamesh
{
  class exception : public std::runtime_error
  {
  public:
    exception(viennamesh_error error_code_in,
              std::string const & function_in, std::string const & file_in, int line_in,
              std::string const & message_in) :
        std::runtime_error(message_in), error_code_(error_code_in), function_(function_in), file_(file_in), line_(line_in) {}
    virtual ~exception() throw() {}


    viennamesh_error error_code() const { return error_code_; }
    std::string const & function() const { return function_; }
    std::string const & file() const { return file_; }
    int line() const { return line_; }

  private:
    viennamesh_error error_code_;
    std::string function_;
    std::string file_;
    int line_;
  };
}

std::ostream & operator<<(std::ostream & stream, viennamesh_error error);
std::ostream & operator<<(std::ostream & stream, viennamesh::exception const & ex);


#define VIENNAMESH_ERROR(ERROR_CODE, MESSAGE) throw viennamesh::exception(ERROR_CODE, __func__, __FILE__, __LINE__, MESSAGE);
// #define HANDLE_ERROR(TO_CHECK, OBJECT) viennamesh::handle_error(TO_CHECK, viennamesh::internal_context(OBJECT))

namespace viennamesh
{
  viennamesh_error handle_error(viennamesh_context context);
  void handle_error(viennamesh_error error, viennamesh_context context);
}








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
  }

  template<typename DataT>
  viennamesh_error trivial_data_delete(DataT data)
  {
    delete data;
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
  { return make_viennamesh_data<viennagrid_quantity_field>(data, viennagrid_quantity_field_make); }
  inline viennamesh_error delete_quantities(viennamesh_data data)
  { return delete_viennamesh_data<viennagrid_quantity_field>(data, viennagrid_quantity_field_release); }

  inline viennamesh_error make_mesh(viennamesh_data * data)
  {
    viennagrid_mesh_hierarchy mesh_hierarchy;
    viennagrid_mesh * mesh = new viennagrid_mesh;

    viennagrid_mesh_hierarchy_create(&mesh_hierarchy);
    viennagrid_mesh_hierarchy_get_root(mesh_hierarchy, mesh);

    *data = mesh;

    return VIENNAMESH_SUCCESS;
  }

  inline viennamesh_error delete_mesh(viennamesh_data data)
  {
    viennagrid_mesh * mesh = (viennagrid_mesh *)data;

    viennagrid_mesh_hierarchy mesh_hierarchy;
    viennagrid_mesh_get_mesh_hierarchy(*mesh, &mesh_hierarchy);
    viennagrid_mesh_hierarchy_release(mesh_hierarchy);

    delete mesh;

    return VIENNAMESH_SUCCESS;
  }

}


#endif
