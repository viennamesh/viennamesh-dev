#ifndef VIENNAMESH_CORE_EXCEPTIONS_HPP
#define VIENNAMESH_CORE_EXCEPTIONS_HPP

#include <exception>
#include <stdexcept>

namespace viennamesh
{
  class input_parameter_not_found_exception : public std::runtime_error
  {
  public:
    input_parameter_not_found_exception(string const & message_) : std::runtime_error(message_) {}
    virtual ~input_parameter_not_found_exception() throw() {}
  };

  class output_not_convertable_to_referenced_value_exception : public std::runtime_error
  {
  public:

    output_not_convertable_to_referenced_value_exception(string const & message_) : std::runtime_error(message_) {}
    virtual ~output_not_convertable_to_referenced_value_exception() throw() {}
  };
}

#endif
