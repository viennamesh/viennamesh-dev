#ifndef VIENNAMESH_PYTHON_INTERPRETER_HPP
#define VIENNAMESH_PYTHON_INTERPRETER_HPP

#ifdef VIENNAMESH_HAS_PYTHON

#include "viennamesh/utils/interpreter/interpreter.hpp"
#include <Python.h>


namespace viennamesh
{

  class py_object_wrapper
  {
  public:

    py_object_wrapper() : obj(0) {}
    py_object_wrapper(PyObject * obj_) : obj(obj_) {}

    ~py_object_wrapper() { decrease(); }

    py_object_wrapper & operator=(PyObject * obj_)
    {
      decrease();
      obj = obj_;
      return *this;
    }

    py_object_wrapper & operator=(py_object_wrapper const & rhs)
    {
      decrease();
      obj = rhs.obj;
      increase();
      return *this;
    }

    operator PyObject * ()
    {
      return obj;
    }

    operator PyObject const * () const
    {
      return obj;
    }

    operator bool() const
    {
      return obj;
    }


    void increase()
    {
      if (obj)
        Py_INCREF(obj);
    }

    void decrease()
    {
      if (obj)
        Py_DECREF(obj);
    }

  private:

    PyObject * obj;
  };


  class python_interpreter_exception : public std::runtime_error
  {
  public:
    python_interpreter_exception(std::string const & message_) : std::runtime_error(message_) {}
    virtual ~python_interpreter_exception() throw() {}
  };

  class python_interpreter : public virtual interpreter
  {
  public:

    python_interpreter() { init(); }

    virtual ~python_interpreter() { deinit(); }

    void init();
    void deinit();

    void run_script(std::string const & script);

    void set_bool(std::string const & name, bool value);
    void set_bool(std::string const & name, std::string const & value);

    void set_int(std::string const & name, int value);
    void set_int(std::string const & name, std::string const & value);

    void set_double(std::string const & name, double value);
    void set_double(std::string const & name, std::string const & value);

    void set_string(std::string const & name, std::string const & value);

    bool get_bool(std::string const & name);
    int get_int(std::string const & name);
    double get_double(std::string const & name);
    std::string get_string(std::string const & name);


    bool evaluate_bool(std::string const & expression);
    int evaluate_int(std::string const & expression);
    double evaluate_double(std::string const & expression);
    std::string evaluate_string(std::string const & expression);

  protected:


    void throw_script_error(std::string const & function_name, std::string const & script) const;
    void throw_script_exception(std::string const & function_name, std::string const & script) const;

    std::string python_exception() const;


    PyObject * py_main;
    PyObject * py_globals;

    bool is_initialized() const
    {
      return Py_IsInitialized();
    }


    static PyObject * log_error(PyObject *self, PyObject *args);

    static PyMethodDef log_error_methods[];
    static std::stringstream python_error_;
    static std::string python_error()
    {
      std::string tmp = python_error_.str();
      python_error_.str("");
      return tmp;
    }
  };


  class numpy_vector_interpreter_exception : public std::runtime_error
  {
  public:
    numpy_vector_interpreter_exception(std::string const & message_) : std::runtime_error(message_) {}
    virtual ~numpy_vector_interpreter_exception() throw() {}
  };




  class numpy_vector_interpreter : public vector_interpreter, public python_interpreter
  {
  public:

    numpy_vector_interpreter() { init(); }
    ~numpy_vector_interpreter() { deinit(); }

    void init();
    void deinit();

    void set_vec2(std::string const & name, vec2 const & value);
    void set_vec2(std::string const & name, std::string const & value);
    void set_vec2_coordinated(std::string const & name, std::string const & value);

    void set_vec3(std::string const & name, vec3 const & value);
    void set_vec3(std::string const & name, std::string const & value);
    void set_vec3_coordinated(std::string const & name, std::string const & value);

    vec2 get_vec2(std::string const & name);
    vec3 get_vec3(std::string const & name);

    vec2 evaluate_vec2(std::string const & expression);
    vec2 evaluate_vec2_coordinated(std::string const & expression);

    vec3 evaluate_vec3(std::string const & expression);
    vec3 evaluate_vec3_coordinated(std::string const & expression);

  private:

    py_object_wrapper index_0;
    py_object_wrapper index_1;
    py_object_wrapper index_2;
  };


}

#endif

#endif
