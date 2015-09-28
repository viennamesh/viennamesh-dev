#ifndef _VIENNAMESH_BACKEND_COMMON_HPP_
#define _VIENNAMESH_BACKEND_COMMON_HPP_

#include <ostream>
#include "viennamesh.h"
#include "viennagrid/viennagrid.hpp"

namespace viennamesh
{
  class exception : public std::runtime_error
  {
  public:
    exception(viennamesh_error error_code_in,
              std::string const & function_in, std::string const & file_in, int line_in,
              std::string const & message_in) :
        std::runtime_error(message_in), error_code_(error_code_in), function_(function_in), file_(file_in), line_(line_in) {}
    virtual ~exception() throw() {}

    viennamesh_error error_code() const { return error_code_; }
    std::string const & function() const { return function_; }
    std::string const & file() const { return file_; }
    int line() const { return line_; }

  private:
    viennamesh_error error_code_;
    std::string function_;
    std::string file_;
    int line_;
  };
}

// inline std::ostream & operator<<(std::ostream & stream, viennamesh_error error)
// {
//   const char * error_string;
//   viennamesh_error_string(error, &error_string);
//
//   stream << error_string;
//   return stream;
// }

inline std::ostream & operator<<(std::ostream & stream, viennamesh::exception const & ex)
{
  const char * error_string = viennamesh_error_string(ex.error_code());
  if (error_string)
    stream << error_string;
  else
    stream << "Invalid error";

  stream << "[" << error_string << "] ";

  if (!ex.file().empty() && ex.line() != -1)
    stream << ex.file() << ":" << ex.line();

  if (!ex.function().empty())
    stream << "[" << ex.function() << "] ";

  stream << ex.what();
  return stream;
}


#define VIENNAMESH_ERROR(ERROR_CODE, MESSAGE) throw viennamesh::exception(ERROR_CODE, __func__, __FILE__, __LINE__, MESSAGE);
// #define HANDLE_ERROR(TO_CHECK, OBJECT) viennamesh::handle_error(TO_CHECK, viennamesh::internal_context(OBJECT))

namespace viennamesh
{
  inline viennamesh_error handle_error(viennamesh_context context)
  {
    if (!context)
      return VIENNAMESH_ERROR_INVALID_CONTEXT;

    try
    {
      throw;
    }
    catch (viennamesh::exception const & ex)
    {
      viennamesh_context_set_error(context, ex.error_code(), ex.function().c_str(), ex.file().c_str(), ex.line(), ex.what());
      return ex.error_code();
    }
    catch (viennagrid::exception const & ex)
    {
      viennamesh_context_set_error(context, VIENNAMESH_VIENNAGRID_ERROR, "", "", -1, ex.what());
      return VIENNAMESH_VIENNAGRID_ERROR;
    }
    catch (...)
    {
      return VIENNAMESH_UNKNOWN_ERROR;
    }
  }

  inline void handle_error(viennamesh_error error, viennamesh_context context)
  {
    if (error != VIENNAMESH_SUCCESS)
    {
      viennamesh_error error_code;
      const char * error_function;
      const char * error_file;
      int error_line;
      const char * error_message;

      if (viennamesh_context_get_error(context, &error_code, &error_function, &error_file, &error_line, &error_message) == VIENNAMESH_SUCCESS)
        throw exception(error_code, error_function, error_file, error_line, error_message);
      else
        throw exception(VIENNAMESH_ERROR_INVALID_CONTEXT, "", "", -1, "invalid context in handle_error");
    }
  }

//   viennamesh_error handle_error(viennamesh_context context);
//   void handle_error(viennamesh_error error, viennamesh_context context);
}



#endif
