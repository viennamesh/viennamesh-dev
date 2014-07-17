#ifndef VIENNAMESH_INTERPRETER_HPP
#define VIENNAMESH_INTERPRETER_HPP


#include <viennagrid/config/default_configs.hpp>


namespace viennamesh
{

  class interpreter_exception : public std::runtime_error
  {
  public:
    interpreter_exception(std::string const & message_) : std::runtime_error(message_) {}
    virtual ~interpreter_exception() throw() {}
  };


  class interpreter
  {
  public:

    typedef viennagrid::config::point_type_2d vec2;
    typedef viennagrid::config::point_type_3d vec3;

    interpreter() {}
    virtual ~interpreter() {}

    virtual void init() = 0;
    virtual void deinit() = 0;

    void reset()
    {
      deinit();
      init();
    }

    virtual void run_script(std::string const & script) = 0;

    virtual void set_bool(std::string const & name, bool value) = 0;
    virtual void set_int(std::string const & name, int value) = 0;
    virtual void set_double(std::string const & name, double value) = 0;
    virtual void set_string(std::string const & name, std::string const & value) = 0;

    virtual bool get_bool(std::string const & name) = 0;
    virtual int get_int(std::string const & name) = 0;
    virtual double get_double(std::string const & name) = 0;
    virtual std::string get_string(std::string const & name) = 0;

    virtual bool evaluate_bool(std::string const & expression) = 0;
    virtual int evaluate_int(std::string const & expression) = 0;
    virtual double evaluate_double(std::string const & expression) = 0;
    virtual std::string evaluate_string(std::string const & expression) = 0;

  private:

  };


  class vector_interpreter : public virtual interpreter
  {
  public:

    virtual ~vector_interpreter() {}

    virtual void set_vec2(std::string const & name, vec2 const & value) = 0;
    virtual void set_vec3(std::string const & name, vec3 const & value) = 0;

    virtual vec2 get_vec2(std::string const & name) = 0;
    virtual vec3 get_vec3(std::string const & name) = 0;

    virtual vec2 evaluate_vec2(std::string const & expression) = 0;
    virtual vec2 evaluate_vec2_coordinates(std::string const & expression) = 0;
    virtual vec3 evaluate_vec3(std::string const & expression) = 0;
    virtual vec3 evaluate_vec3_coordinates(std::string const & expression) = 0;

  private:

  };

}

#endif
