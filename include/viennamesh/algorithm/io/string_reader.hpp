#ifndef VIENNAMESH_ALGORITHM_IO_STRING_READER_HPP
#define VIENNAMESH_ALGORITHM_IO_STRING_READER_HPP

/* ============================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                ViennaMesh - The Vienna Meshing Framework
                            -----------------

                    http://viennamesh.sourceforge.net/

   License:         MIT (X11), see file LICENSE in the base directory
=============================================================================== */

#include "viennamesh/core/algorithm.hpp"

namespace viennamesh
{
  namespace io
  {
    class string_reader : public base_algorithm
    {
    public:
      string_reader();

      std::string name() const;
      std::string id() const;

      bool run_impl();

    private:
      required_input_parameter_interface<std::string>   filename;

      output_parameter_interface                        output_string;
    };
  }
}

#endif
