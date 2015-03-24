#ifndef _VIENNAMESH_BACKEND_FORWARDS_HPP_
#define _VIENNAMESH_BACKEND_FORWARDS_HPP_

#include "viennamesh/backend/api.h"

// #define VIENNAMESH_BACKEND_RETAIN_RELEASE_LOGGING

namespace viennamesh
{
  class error_t
  {
  public:
    error_t(viennamesh_error error_code_) : error_code(error_code_) {}
    operator viennamesh_error() const { return error_code; }

  private:
    viennamesh_error error_code;
  };

  class data_template_t;
  typedef data_template_t * data_template;

  class algorithm_template_t;
  typedef algorithm_template_t * algorithm_template;
}

#endif
