#ifndef _VIENNAMESH_FORWARDS_HPP_
#define _VIENNAMESH_FORWARDS_HPP_

#include "viennamesh/viennamesh.h"
#include "viennagrid/viennagrid.hpp"
#include <string>

#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/optional.hpp>

namespace viennamesh
{
  using boost::lexical_cast;

  using boost::shared_ptr;
  using boost::make_shared;

  using boost::function;
  using boost::bind;
  using ::_1;
  using ::_2;

  using boost::optional;



  class context_handle;

  class abstract_data_handle;
  template<typename DataT>
  class data_handle;

  class algorithm_handle;

  using viennagrid::mesh;
  using viennagrid::quantity_field;
  using viennagrid::point;
  using viennagrid::seed_point;

  typedef std::vector<point> point_container;
  typedef std::vector<seed_point> seed_point_container;


  namespace result_of
  {
    template<typename T>
    struct data_information;
  }
}

#endif
