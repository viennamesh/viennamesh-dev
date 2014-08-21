#include "viennamesh/utils/interpreter/interpreter.hpp"

#ifdef VIENNAMESH_HAS_PYTHON
  #include "viennamesh/utils/interpreter/python_interpreter.hpp"
#endif

namespace viennamesh
{
  boost::shared_ptr<vector_interpreter> make_vector_interpreter(std::string const & interpreter_type)
  {
#ifdef VIENNAMESH_HAS_PYTHON
    if (interpreter_type == "Python")
      return boost::shared_ptr<vector_interpreter>( new numpy_vector_interpreter() );
#endif

    return boost::shared_ptr<vector_interpreter>();
  }

  boost::shared_ptr<interpreter> make_interpreter(std::string const & interpreter_type)
  {
#ifdef VIENNAMESH_HAS_PYTHON
    if (interpreter_type == "Python")
      return boost::shared_ptr<interpreter>( new python_interpreter() );
#endif

    return make_vector_interpreter(interpreter_type);
  }






  boost::shared_ptr<interpreter> interpreter_factory_t::get_interpreter( std::string const & interpreter_type )
  {
    std::map<std::string, boost::shared_ptr<interpreter> >::iterator it = interpreters.find(interpreter_type);
    if (it != interpreters.end())
      return it->second;

    boost::shared_ptr<interpreter> tmp = make_interpreter(interpreter_type);
    if (tmp)
      interpreters[interpreter_type] = tmp;

    return tmp;
  }

  boost::shared_ptr<vector_interpreter> interpreter_factory_t::get_vector_interpreter( std::string const & interpreter_type )
  {
    std::map<std::string, boost::shared_ptr<vector_interpreter> >::iterator it = vector_interpreters.find(interpreter_type);
    if (it != vector_interpreters.end())
      return it->second;

    boost::shared_ptr<vector_interpreter> tmp = make_vector_interpreter(interpreter_type);
    if (tmp)
      vector_interpreters[interpreter_type] = tmp;

    return tmp;
  }


  interpreter_factory_t & interpreter_factory()
  {
    static interpreter_factory_t factory;
    return factory;
  }

}
