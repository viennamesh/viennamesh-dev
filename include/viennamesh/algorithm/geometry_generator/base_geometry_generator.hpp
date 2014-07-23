#ifndef VIENNAMESH_ALGORITHM_BASE_GEOMETRY_GENERATOR_HPP
#define VIENNAMESH_ALGORITHM_BASE_GEOMETRY_GENERATOR_HPP

#include <stdexcept>
#include <CGAL/Direction_2.h>
#include "pugixml/pugixml.hpp"
#include "viennagrid/config/default_configs.hpp"

#include "viennamesh/core/parameter_interface.hpp"
#include "viennamesh/utils/interpreter/interpreter.hpp"

namespace viennamesh
{
  enum ParameterType
  {
    PARAMETER_TYPE_UNKOWN,
    PARAMETER_TYPE_BOOL,
    PARAMETER_TYPE_INT,
    PARAMETER_TYPE_DOUBLE,
    PARAMETER_TYPE_STRING,
    PARAMETER_TYPE_VEC2,
    PARAMETER_TYPE_VEC3,
  };

  ParameterType parameter_type_from_string(std::string const & str);
  std::string parameter_type_to_string(ParameterType ptype);


  struct InputParameter
  {
    ParameterType type;
    std::string default_value;
    bool set;
  };


  class base_geometry_generator_exception : public std::runtime_error
  {
  public:
    base_geometry_generator_exception(std::string const & message_) : std::runtime_error(message_) {}
    virtual ~base_geometry_generator_exception() throw() {}
  };



  class base_geometry_generator
  {
  public:

    typedef viennagrid::config::point_type_2d vec2;
    typedef viennagrid::config::point_type_3d vec3;


    base_geometry_generator() {}
    virtual ~base_geometry_generator() {}

    virtual void clear_impl() = 0;
    void clear()
    {
      input_parameters.clear();
      restrictions.clear();

      scripts.clear();
    }

    void reset()
    {
      interpreter_->reset();
      for (std::map<std::string, InputParameter>::iterator it = input_parameters.begin(); it != input_parameters.end(); ++it)
      {
        it->second.set = false;
      }
    }

    virtual void read_impl(pugi::xml_node const & node) = 0;
    void read(pugi::xml_node const & node);

    virtual void run_impl(output_parameter_interface & opi) = 0;
    void run(output_parameter_interface & opi);

    template<typename T>
    void set_parameter(std::string const & parameter_name, T value)
    {
      std::map<std::string, InputParameter>::iterator it = input_parameters.find(parameter_name);
      if (it != input_parameters.end())
      {
        set_paramater_impl(it, parameter_name, value);
        it->second.set = true;
      }
      else
        std::cout << "ERROR: parameter \"" << parameter_name << "\" not found" << std::endl;
    }

    void set_parameters(base_algorithm & algorithm);

    static boost::shared_ptr<base_geometry_generator> make(pugi::xml_node const & node);


  protected:

    void set_paramater_impl(std::map<std::string, InputParameter>::iterator it, std::string const & parameter_name, bool value)
    {
      if (it->second.type == PARAMETER_TYPE_BOOL)
        interpreter_->set_bool(parameter_name, value);
      else
      {
        error(1) << "ERROR: parameter type missmatch. Expected bool, got \"" << parameter_type_to_string(it->second.type) << "\"" << std::endl;
        // TODO: throw
      }
    }

    void set_paramater_impl(std::map<std::string, InputParameter>::iterator it, std::string const & parameter_name, int value)
    {
      if (it->second.type == PARAMETER_TYPE_INT)
        interpreter_->set_int(parameter_name, value);
      else
      {
        error(1) << "ERROR: parameter type missmatch. Expected int, got \"" << parameter_type_to_string(it->second.type) << "\"" << std::endl;
        // TODO: throw
      }
    }

    void set_paramater_impl(std::map<std::string, InputParameter>::iterator it, std::string const & parameter_name, double value)
    {
      if (it->second.type == PARAMETER_TYPE_DOUBLE)
        interpreter_->set_double(parameter_name, value);
      else
      {
        error(1) << "ERROR: parameter type missmatch. Expected double, got \"" << parameter_type_to_string(it->second.type) << "\"" << std::endl;
        // TODO: throw
      }
    }

    void set_paramater_impl(std::map<std::string, InputParameter>::iterator it, std::string const & parameter_name, vec2 value)
    {
      if (it->second.type == PARAMETER_TYPE_VEC2)
        interpreter_->set_vec2(parameter_name, value);
      else
      {
        error(1) << "ERROR: parameter type missmatch. Expected vec2, got \"" << parameter_type_to_string(it->second.type) << "\"" << std::endl;
        // TODO: throw
      }
    }

    void set_paramater_impl(std::map<std::string, InputParameter>::iterator it, std::string const & parameter_name, vec3 value)
    {
      if (it->second.type == PARAMETER_TYPE_VEC3)
        interpreter_->set_vec3(parameter_name, value);
      else
      {
        error(1) << "ERROR: parameter type missmatch. Expected vec3, got \"" << parameter_type_to_string(it->second.type) << "\"" << std::endl;
        // TODO: throw
      }
    }





    boost::shared_ptr<vector_interpreter> interpreter_;

    std::map<std::string, InputParameter> input_parameters;
    std::vector<std::string> restrictions;

    std::vector<std::string> scripts;
  };
}

#endif
