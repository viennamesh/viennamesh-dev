#include "viennamesh/utils/interpreter/interpreter.hpp"

#ifdef VIENNAMESH_HAS_PYTHON
  #include "viennamesh/utils/interpreter/python_interpreter.hpp"
#endif

namespace viennamesh
{
  boost::shared_ptr<interpreter> interpreter::make(std::string const & interpreter_type)
  {
#ifdef VIENNAMESH_HAS_PYTHON
    if (interpreter_type == "Python")
      return boost::shared_ptr<interpreter>( new python_interpreter() );
#endif

    return vector_interpreter::make(interpreter_type);
  }

  boost::shared_ptr<vector_interpreter> vector_interpreter::make(std::string const & interpreter_type)
  {
#ifdef VIENNAMESH_HAS_PYTHON
    if (interpreter_type == "Python")
      return boost::shared_ptr<vector_interpreter>( new numpy_vector_interpreter() );
#endif

    return boost::shared_ptr<vector_interpreter>();
  }
}
