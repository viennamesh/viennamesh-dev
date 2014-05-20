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

#include "viennamesh/algorithm/io/string_reader.hpp"

namespace viennamesh
{
  namespace io
  {
    string_reader::string_reader() :
      filename(*this, parameter_information("filename","string","The filename of the string to be read")),
      output_string(*this, parameter_information("string","string","The read string")) {}

    std::string string_reader::name() const { return "ViennaGrid String Reader"; }
    std::string string_reader::id() const { return "string_reader"; }

    bool string_reader::run_impl()
    {
      std::string fn = filename();
      if (!base_path().empty())
      {
        info(1) << "Using base path: " << base_path() << std::endl;
        fn = base_path() + "/" + fn;
      }

      std::ifstream file( fn.c_str() );

      if (!file)
      {
        error(1) << "Error reading file \"" << fn << "\"" << std::endl;
        return false;
      }

      output_parameter_proxy<std::string> op(output_string);
      op() = stringtools::read_stream(file);

      return true;
    }
  }
}
