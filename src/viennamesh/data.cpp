#include "viennamesh/data.hpp"

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


}
