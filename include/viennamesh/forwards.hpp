#ifndef _VIENNAMESH_FORWARDS_HPP_
#define _VIENNAMESH_FORWARDS_HPP_

#include "viennamesh/backend/api.h"
#include "viennagrid/point.hpp"
#include "viennagrid/quantity_field.hpp"
#include "viennagrid/mesh/mesh.hpp"
#include <string>

namespace viennamesh
{
  class context_handle;

  class abstract_data_handle;
  template<typename DataT>
  class data_handle;

  class algorithm_handle;

  using viennagrid::mesh_t;
  using viennagrid::quantity_field;
  using viennagrid::point_t;
  using viennagrid::seed_point_t;

  typedef std::vector<point_t> point_container_t;
  typedef std::vector<seed_point_t> seed_point_container_t;


  namespace result_of
  {
    template<typename T>
    struct data_information;
  }
}

#endif
