#ifndef VIENNAMESH_ALGORITHM_IO_STRING_READER_HPP
#define VIENNAMESH_ALGORITHM_IO_STRING_READER_HPP

#include "viennamesh/core/algorithm.hpp"

namespace viennamesh
{
  namespace io
  {
    class string_reader : public base_algorithm
    {
    public:
      string_reader();

      string name() const;
      string id() const;

      bool run_impl();

    private:
      required_input_parameter_interface<string>        filename;

      output_parameter_interface                        output_string;
    };
  }
}

#endif
