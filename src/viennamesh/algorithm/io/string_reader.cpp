#include "viennamesh/algorithm/io/string_reader.hpp"

namespace viennamesh
{
  namespace io
  {
    string_reader::string_reader() :
      filename(*this, "filename"),
      output_string(*this, "string") {}

    string string_reader::name() const { return "ViennaGrid String Reader"; }
    string string_reader::id() const { return "string_reader"; }

    bool string_reader::run_impl()
    {
      std::ifstream file( filename().c_str() );

      if (!file)
        return false;

      output_parameter_proxy<string> op(output_string);
      op() = stringtools::read_stream(file);

      return true;
    }
  }
}
