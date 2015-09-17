#include "viennameshpp/data.hpp"

namespace viennamesh
{


  abstract_data_handle::abstract_data_handle() : data(0) {}
  abstract_data_handle::abstract_data_handle(viennamesh_data_wrapper data_, bool retain_) : data(data_)
  {
    if (retain_)
      retain();
  }

  abstract_data_handle::abstract_data_handle(abstract_data_handle const & handle_) : data(handle_.data) { retain(); }

  abstract_data_handle::~abstract_data_handle()
  {
    release();
  }

  abstract_data_handle & abstract_data_handle::operator=(abstract_data_handle const & handle_)
  {
    release();
    data = handle_.data;
    retain();
    return *this;
  }

  bool abstract_data_handle::valid() const
  {
    return data != NULL && !empty();
  }

  bool abstract_data_handle::empty() const { return size() == 0; }

  int abstract_data_handle::size() const
  {
    int size_;
    handle_error(viennamesh_data_wrapper_get_size(data, &size_), data);
    return size_;
  }

  void abstract_data_handle::resize(int size_)
  {
    handle_error(viennamesh_data_wrapper_resize(data, size_), data);
  }

  viennamesh_data_wrapper abstract_data_handle::internal() const
  {
    return const_cast<viennamesh_data_wrapper>(data);
  }

  std::string abstract_data_handle::type_name() const
  {
    const char * tmp;
    viennamesh_data_wrapper_get_type_name(internal(), &tmp);
    return tmp;
  }


  void abstract_data_handle::retain()
  {
    if (data)
      handle_error(viennamesh_data_wrapper_retain(data), data);
  }

  void abstract_data_handle::release()
  {
    if (data)
      viennamesh_data_wrapper_release(data);
  }







  // viennagrid::mesh
  viennagrid::mesh to_cpp(viennagrid_mesh & src)
  {
    return viennagrid::mesh(src);
  }

  void to_c(viennagrid::mesh const & src, viennagrid_mesh & dst)
  {
    viennagrid_mesh_release(dst);
    dst = src.internal();
    viennagrid_mesh_retain(dst);
  }


  // viennagrid::quantity_field
  viennagrid::quantity_field to_cpp(viennagrid_quantity_field & src)
  {
    return viennagrid::quantity_field(src);
  }

  void to_c(viennagrid::quantity_field const & src, viennagrid_quantity_field & dst)
  {
    viennagrid_quantity_field_release( dst );
    dst = src.internal();
    viennagrid_quantity_field_retain( dst );
  }


  // viennagrid::point
  viennagrid::point to_cpp(viennamesh_point & src)
  {
    double * values;
    int size;
    viennamesh_point_get(src, &values, &size);
    viennagrid::point result(size);
    std::copy(values, values+size, &result[0]);
    return result;
  }

  void to_c(viennagrid::point const & src, viennamesh_point & dst)
  {
    viennamesh_point_delete(dst);
    viennamesh_point_make(&dst);
    viennamesh_point_set(dst, const_cast<double*>(&src[0]), src.size());
  }


  // viennagrid::seed_point
  seed_point to_cpp(viennamesh_seed_point & src)
  {
    double * values;
    int size;
    int region;
    viennamesh_seed_point_get(src, &values, &size, &region);
    viennagrid::point result(size);
    std::copy(values, values+size, &result[0]);
    return std::make_pair(result, region);
  }

  void to_c(seed_point const & src, viennamesh_seed_point & dst)
  {
    viennamesh_seed_point_delete(dst);
    viennamesh_seed_point_make(&dst);
    viennamesh_seed_point_set(dst, const_cast<double*>(&src.first[0]), src.first.size(), src.second);
  }


  // std::string
  std::string to_cpp(viennamesh_string & src)
  {
    const char * tmp;
    viennamesh_string_get( src, &tmp );
    return tmp;
  }

  void to_c(std::string const & src, viennamesh_string dst)
  {
    viennamesh_string_set( dst, src.c_str() );
  }








}
