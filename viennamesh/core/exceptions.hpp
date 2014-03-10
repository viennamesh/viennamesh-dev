#ifndef VIENNAMESH_CORE_EXCEPTIONS_HPP
#define VIENNAMESH_CORE_EXCEPTIONS_HPP

#include <exception>

namespace viennamesh
{
  class algorithm_exception : public std::exception
  {
  public:
    virtual ~algorithm_exception() throw() {}
  };


  class input_parameter_not_found_exception : public algorithm_exception
  {
  public:

    input_parameter_not_found_exception(string const & parameter_name) : parameter_name_(parameter_name) {}

    virtual ~input_parameter_not_found_exception() throw() {}
    virtual const char* what() const throw()
    {
      std::stringstream ss;
      ss << "Input parameter '" << parameter_name_ << "' is missing or of non-convertable type";
      return ss.str().c_str();
    }

  private:
    string parameter_name_;
  };


  class output_not_convertable_to_referenced_value_exception : public algorithm_exception
  {
  public:

    output_not_convertable_to_referenced_value_exception(string const & parameter_name) : parameter_name_(parameter_name) {}

    virtual ~output_not_convertable_to_referenced_value_exception() throw() {}
    virtual const char* what() const throw()
    {
      std::stringstream ss;
      ss << "Output parameter '" << parameter_name_ << "' is not convertable to referenced value";
      return ss.str().c_str();
    }

  private:
    string parameter_name_;
  };
}

#endif
