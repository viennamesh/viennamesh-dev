#ifndef _VIENNAMESH_FORWARDS_HPP_
#define _VIENNAMESH_FORWARDS_HPP_

#include "viennamesh/backend/api.h"
#include "viennagrid/point.hpp"
#include <string>

namespace viennamesh
{

  class context_handle;

  class abstract_data_handle;
  template<typename DataT>
  class data_handle;

  class algorithm_handle;

  using viennagrid::point_t;


  namespace result_of
  {
    template<typename T>
    struct data_information;
  }

  inline std::string local_binary_format()
  {
    return __VERSION__;
  }
}

#endif
