#ifdef VIENNAMESH_HAS_PYTHON

#include "viennamesh/algorithm/geometry_generator/python_interpreter.hpp"
#include "viennamesh/utils/string_tools.hpp"
using stringtools::lexical_cast;

namespace viennamesh
{

  void python_interpreter::init()
  {
    if (is_initialized())
      return;

    Py_Initialize();

    if (!is_initialized())
      throw python_interpreter_exception("python_interpreter::init - Python initialization failed");

    py_main = PyImport_AddModule("__main__");
    py_globals = PyModule_GetDict(py_main);

    PyObject * m = Py_InitModule("__log_error", log_error_methods);
    if (!m)
      throw python_interpreter_exception("python_interpreter::init - Error initializing error logger");

    char stderr[] = "stderr";
    if (PySys_SetObject( stderr, m ) == -1)
      throw python_interpreter_exception("python_interpreter::init - Error setting error logger");
  }

  void python_interpreter::deinit()
  {
    if (!is_initialized())
      return;

    Py_Finalize();
  }


  void python_interpreter::run_script(std::string const & script)
  {
    if (PyRun_SimpleString( script.c_str() ) == -1)
      throw_script_error("python_interpreter::run_script", script);
  }

  void python_interpreter::set_bool(std::string const & name, bool value)
  {
    if (value)
      run_script(name + " = true");
    else
      run_script(name + " = false");
  }

  void python_interpreter::set_int(std::string const & name, int value)
  { run_script(name + " = int(" + lexical_cast<std::string>(value) + ")"); }

  void python_interpreter::set_double(std::string const & name, double value)
  { run_script(name + " = float(" + lexical_cast<std::string>(value) + ")"); }

  void python_interpreter::set_string(std::string const & name, std::string const & value)
  { run_script(name + " = '" + value + "'"); }


  bool python_interpreter::get_bool(std::string const & name) { return evaluate_bool(name); }
  int python_interpreter::get_int(std::string const & name) { return evaluate_int(name); }
  double python_interpreter::get_double(std::string const & name) { return evaluate_double(name); }
  std::string python_interpreter::get_string(std::string const & name) { return evaluate_string(name); }


  bool python_interpreter::evaluate_bool(std::string const & expression)
  {
    py_object_wrapper py_result = PyRun_String(expression.c_str(), Py_eval_input, py_globals, py_globals);
    if (!py_result)
      throw_script_exception("python_interpreter::evaluate_bool", expression);

    int result = PyObject_IsTrue(py_result);
    if (result == -1)
      throw python_interpreter_exception("python_interpreter::evaluate_bool - failed\n" + python_exception());

    return result == 1;
  }

  int python_interpreter::evaluate_int(std::string const & expression)
  {
    py_object_wrapper py_result = PyRun_String(expression.c_str(), Py_eval_input, py_globals, py_globals);
    if (!py_result)
      throw_script_exception("python_interpreter::evaluate_int", expression);

    int result = PyLong_AsLong(py_result);
    if (PyErr_Occurred())
      throw python_interpreter_exception("python_interpreter::evaluate_int - conversion to int failed");

    return result;
  }

  double python_interpreter::evaluate_double(std::string const & expression)
  {
    py_object_wrapper py_result = PyRun_String(expression.c_str(), Py_eval_input, py_globals, py_globals);
    if (!py_result)
      throw_script_exception("python_interpreter::evaluate_double", expression);

    double result = PyFloat_AsDouble(py_result);
    if (PyErr_Occurred())
      throw python_interpreter_exception("python_interpreter::evaluate_double - conversion to double failed");

    return result;
  }

  std::string python_interpreter::evaluate_string(std::string const & expression)
  {
    py_object_wrapper py_result = PyRun_String(expression.c_str(), Py_eval_input, py_globals, py_globals);
    if (!py_result)
      throw_script_exception("python_interpreter::evaluate_string", expression);

    std::string result = PyString_AsString(py_result);
    if (PyErr_Occurred())
      throw python_interpreter_exception("python_interpreter::evaluate_string - conversion to string failed");

    return result;
  }

  void python_interpreter::throw_script_error(std::string const & function_name, std::string const & script) const
  {
    std::stringstream ss;

    ss << function_name << " - failed\n";
    ss << "##################################################################\n";
    ss << python_error() << "\n";
    ss << "##################################################################\n";
    ss << script << "\n";
    ss << "##################################################################\n";

    throw python_interpreter_exception(ss.str());
  }

  void python_interpreter::throw_script_exception(std::string const & function_name, std::string const & script) const
  {
    std::stringstream ss;

    ss << function_name <<  " - failed\n";
    ss << "##################################################################\n";
    ss << python_exception();
    ss << "##################################################################\n";
    ss << script << "\n";
    ss << "##################################################################";

    throw python_interpreter_exception(ss.str());
  }




  std::string python_interpreter::python_exception() const
  {
    PyObject * ptype;
    PyObject * pvalue;
    PyObject * ptraceback;

    PyErr_Fetch( &ptype, &pvalue, &ptraceback );

    std::stringstream ss;

    if (ptype)
    {
      py_object_wrapper str = PyObject_Str(ptype);
      ss << "Exception type: " << PyString_AsString(str) << "\n";
    }

    if (pvalue)
    {
      py_object_wrapper str = PyObject_Str(pvalue);
      ss << "Exception value: " << PyString_AsString(str) << "\n";
    }

    if (ptraceback)
    {
      py_object_wrapper str = PyObject_Str(ptraceback);
      ss << "Exception traceback: " << PyString_AsString(str) << "\n";
    }

    return ss.str();
  }


  PyObject * python_interpreter::log_error(PyObject *self, PyObject *args)
  {
    const char *what;

    if (!PyArg_ParseTuple(args, "s", &what))
        return NULL;
    python_error_ << what << std::endl;
    return Py_BuildValue("");
  }

  PyMethodDef python_interpreter::log_error_methods[] = {
      {"write", python_interpreter::log_error, METH_VARARGS, "Write something."},
      {NULL, NULL, 0, NULL}
  };
  std::stringstream python_interpreter::python_error_;





  void numpy_vector_interpreter::init()
  {
    python_interpreter::init();

    python_interpreter::run_script("import numpy");
    python_interpreter::run_script("vector = numpy.array");

    index_0 = PyLong_FromLong(0);
    if (!index_0)
      throw python_interpreter_exception("numpy_vector_interpreter::init - creating index 0 failed");

    index_1 = PyLong_FromLong(1);
    if (!index_1)
      throw python_interpreter_exception("numpy_vector_interpreter::init - creating index 1 failed");

    index_2 = PyLong_FromLong(2);
    if (!index_2)
      throw python_interpreter_exception("numpy_vector_interpreter::init - creating index 2 failed");
  }

  void numpy_vector_interpreter::deinit()
  {
    if (!is_initialized())
      return;

    index_0 = py_object_wrapper();
    index_1 = py_object_wrapper();
    index_2 = py_object_wrapper();

    python_interpreter::deinit();
  }

  void numpy_vector_interpreter::set_vec2(std::string const & name, vec2 const & value)
  {
    std::stringstream ss;
    python_interpreter::run_script(name + " = vector([" + lexical_cast<std::string>(value[0]) + "," +
                                      lexical_cast<std::string>(value[1]) + "])");
  }

  void numpy_vector_interpreter::set_vec3(std::string const & name, vec3 const & value)
  {
    python_interpreter::run_script(name + " = vector([" + lexical_cast<std::string>(value[0]) + "," +
                                      lexical_cast<std::string>(value[1]) + "," +
                                      lexical_cast<std::string>(value[2]) + "])");
  }

  interpreter::vec2 numpy_vector_interpreter::get_vec2(std::string const & name) { return evaluate_vec2(name); }
  interpreter::vec3 numpy_vector_interpreter::get_vec3(std::string const & name) { return evaluate_vec3(name); }




  interpreter::vec2 numpy_vector_interpreter::evaluate_vec2(std::string const & expression)
  {
    py_object_wrapper py_result = PyRun_String(expression.c_str(), Py_eval_input, py_globals, py_globals);
    if (!py_result)
      throw_script_exception("numpy_vector_interpreter::evaluate_vec2", expression);

    py_object_wrapper py_x = PyObject_GetItem(py_result, index_0);
    if (!py_x)
      throw python_interpreter_exception("numpy_vector_interpreter::evaluate_vec2 - error accessing x coordinate");

    py_object_wrapper py_y = PyObject_GetItem(py_result, index_1);
    if (!py_y)
      throw python_interpreter_exception("numpy_vector_interpreter::evaluate_vec2 - error accessing y coordinate");


    double x = PyFloat_AsDouble(py_x);
    if (PyErr_Occurred())
      throw python_interpreter_exception("numpy_vector_interpreter::evaluate_double - conversion to double failed");

    double y = PyFloat_AsDouble(py_y);
    if (PyErr_Occurred())
      throw python_interpreter_exception("numpy_vector_interpreter::evaluate_double - conversion to double failed");

    return vec2(x,y);
  }

  interpreter::vec2 numpy_vector_interpreter::evaluate_vec2_coordinates(std::string const & expression)
  {
    return evaluate_vec2("vector([" + expression + "])");
  }

  interpreter::vec3 numpy_vector_interpreter::evaluate_vec3(std::string const & expression)
  {
    py_object_wrapper py_result = PyRun_String(expression.c_str(), Py_eval_input, py_globals, py_globals);
    if (!py_result)
      throw_script_exception("numpy_vector_interpreter::evaluate_vec3", expression);

    py_object_wrapper py_x = PyObject_GetItem(py_result, index_0);
    if (!py_x)
      throw python_interpreter_exception("numpy_vector_interpreter::evaluate_vec3 - error accessing x coordinate");

    py_object_wrapper py_y = PyObject_GetItem(py_result, index_1);
    if (!py_y)
      throw python_interpreter_exception("numpy_vector_interpreter::evaluate_vec3 - error accessing y coordinate");

    py_object_wrapper py_z = PyObject_GetItem(py_result, index_2);
    if (!py_z)
      throw python_interpreter_exception("numpy_vector_interpreter::evaluate_vec3 - error accessing z coordinate");

    double x = PyFloat_AsDouble(py_x);
    if (PyErr_Occurred())
      throw python_interpreter_exception("numpy_vector_interpreter::evaluate_double - conversion to double failed");

    double y = PyFloat_AsDouble(py_y);
    if (PyErr_Occurred())
      throw python_interpreter_exception("numpy_vector_interpreter::evaluate_double - conversion to double failed");

    double z = PyFloat_AsDouble(py_z);
    if (PyErr_Occurred())
      throw python_interpreter_exception("numpy_vector_interpreter::evaluate_double - conversion to double failed");

    return vec3(x,y,z);
  }

  interpreter::vec3 numpy_vector_interpreter::evaluate_vec3_coordinates(std::string const & expression)
  {
    return evaluate_vec3("vector([" + expression + "])");
  }


}

#endif
