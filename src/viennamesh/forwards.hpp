#ifndef _VIENNAMESH_BACKEND_FORWARDS_HPP_
#define _VIENNAMESH_BACKEND_FORWARDS_HPP_

#include "viennamesh/viennamesh.h"
#include <stdexcept>
#include <boost/lexical_cast.hpp>

// #define VIENNAMESH_BACKEND_RETAIN_RELEASE_LOGGING

namespace viennamesh
{
  class data_template_t;
  typedef data_template_t * data_template;

  class algorithm_template_t;
  typedef algorithm_template_t * algorithm_template;
}

#endif
