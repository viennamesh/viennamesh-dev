#ifndef _VIENNAMESH_BACKEND_FORWARDS_HPP_
#define _VIENNAMESH_BACKEND_FORWARDS_HPP_

#include "viennamesh/backend/api.h"

namespace viennamesh
{
  class error
  {
  public:
    error(int error_code_) : error_code(error_code_) {}
    operator int() const { return error_code; }

  private:
    int error_code;
  };

  class binary_format_template_t;
  typedef binary_format_template_t * binary_format_template;

  class data_template_t;
  typedef data_template_t * data_template;

  class algorithm_template_t;
  typedef algorithm_template_t * algorithm_template;
}

#endif
