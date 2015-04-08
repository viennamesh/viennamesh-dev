// #include "common.hpp"
//
// std::ostream & operator<<(std::ostream & stream, viennamesh_error error)
// {
//   switch (error)
//   {
//     case VIENNAMESH_SUCCESS:
//       stream << "No error";
//       break;
//     case VIENNAMESH_UNKNOWN_ERROR:
//       stream << "Unknown error";
//       break;
//     case VIENNAMESH_ERROR_INVALID_CONTEXT:
//       stream << "Invalid context";
//       break;
//     case VIENNAMESH_ERROR_ERROR_MANAGEMENT:
//       stream << "Error management error";
//       break;
//     case VIENNAMESH_ERROR_INVALID_ARGUMENT:
//       stream << "Invalid argument";
//       break;
//     case VIENNAMESH_ERROR_DIFFERENT_CONTEXT:
//       stream << "Different context";
//       break;
//     case VIENNAMESH_ERROR_DATA_TYPE_NOT_REGISTERED:
//       stream << "Data type not registered";
//       break;
//     case VIENNAMESH_ERROR_NO_CONVERSION_TO_DATA_TYPE:
//       stream << "Conversion to data type not available";
//       break;
//     case VIENNAMESH_ERROR_CONVERSION_FAILED:
//       stream << "Conversion failed";
//       break;
//     case VIENNAMESH_ERROR_ALGORITHM_ALREADY_REGISTERED:
//       stream << "Algorithm already registered";
//       break;
//     case VIENNAMESH_ERROR_ALGORITHM_NOT_REGISTERED:
//       stream << "Algorithm not registered";
//       break;
//     case VIENNAMESH_ERROR_ALGORITHM_RUN_FAILED:
//       stream << "Algorithm run failed";
//       break;
//     case VIENNAMESH_ERROR_REQUIRED_INPUT_PARAMETER_NOT_FOUND:
//       stream << "Required input parameter not found";
//       break;
//     case VIENNAMESH_ERROR_REQUIRED_INPUT_PARAMETER_NOT_FOUND_OR_NOT_CONVERTABLE:
//       stream << "Required input parameter not found or not convertable";
//       break;
//     case VIENNAMESH_ERROR_SIZING_FUNCTION:
//       stream << "Sizing function error";
//       break;
//   }
//
//   return stream;
// }
//
// std::ostream & operator<<(std::ostream & stream, viennamesh::exception const & ex)
// {
//   stream << "[" << ex.error_code() << "] ";
//
//   if (!ex.file().empty() && ex.line() != -1)
//     stream << ex.file() << ":" << ex.line();
//
//   if (!ex.function().empty())
//     stream << "[" << ex.function() << "] ";
//
//   stream << ex.what();
//   return stream;
// }
//
//
//
//
//
// namespace viennamesh
// {
//   viennamesh_error handle_error(viennamesh_context context)
//   {
//     if (!context)
//       return VIENNAMESH_ERROR_INVALID_CONTEXT;
//
//     try
//     {
//       throw;
//     }
//     catch (viennamesh::exception const & ex)
//     {
//       viennamesh_context_set_error(context, ex.error_code(), ex.function().c_str(), ex.file().c_str(), ex.line(), ex.what());
//
//       return ex.error_code();
//     }
//     catch (...)
//     {
//       return VIENNAMESH_UNKNOWN_ERROR;
//     }
//   }
//
//   void handle_error(viennamesh_error error, viennamesh_context context)
//   {
//     if (error != VIENNAMESH_SUCCESS)
//     {
//       viennamesh_error error_code;
//       const char * error_function;
//       const char * error_file;
//       int error_line;
//       const char * error_message;
//
//       viennamesh_context_get_error(context, &error_code, &error_function, &error_file, &error_line, &error_message);
//       throw exception(error_code, error_function, error_file, error_line, error_message);
//     }
//   }
// }
