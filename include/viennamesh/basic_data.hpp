#ifndef _VIENNAMESH_BASIC_DATA_HPP_
#define _VIENNAMESH_BASIC_DATA_HPP_

#include "viennamesh/forwards.hpp"

namespace viennamesh
{
  namespace result_of
  {
    template<>
    struct data_information<int>
    {
      static std::string type_name() { return "int"; }
      static std::string local_binary_format() { return ""; }
    };

    template<>
    struct data_information<double>
    {
      static std::string type_name() { return "double"; }
      static std::string local_binary_format() { return ""; }
    };
  }
}

#endif
