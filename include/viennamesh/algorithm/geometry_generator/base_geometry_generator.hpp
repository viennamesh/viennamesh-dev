#ifndef VIENNAMESH_ALGORITHM_BASE_GEOMETRY_GENERATOR_HPP
#define VIENNAMESH_ALGORITHM_BASE_GEOMETRY_GENERATOR_HPP

#include <stdexcept>
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
    PARAMETER_TYPE_VEC3
  };

  namespace result_of
  {
    template<typename T>
    struct to_parameter;

    template<>
    struct to_parameter<bool>
    {
      static const ParameterType value = PARAMETER_TYPE_BOOL;
    };

    template<>
    struct to_parameter<int>
    {
      static const ParameterType value = PARAMETER_TYPE_INT;
    };

    template<>
    struct to_parameter<double>
    {
      static const ParameterType value = PARAMETER_TYPE_DOUBLE;
    };

    template<>
    struct to_parameter<std::string>
    {
      static const ParameterType value = PARAMETER_TYPE_STRING;
    };

    template<>
    struct to_parameter<vec2>
    {
      static const ParameterType value = PARAMETER_TYPE_VEC2;
    };

    template<>
    struct to_parameter<vec3>
    {
      static const ParameterType value = PARAMETER_TYPE_VEC3;
    };

  }

  ParameterType parameter_type_from_string(std::string const & str);
  std::string parameter_type_to_string(ParameterType ptype);


  struct InputParameter
  {
    ParameterType type;
    std::string default_value_expression;
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

    base_geometry_generator() {}
    virtual ~base_geometry_generator() {}

    virtual void clear_impl() = 0;
    void clear()
    {
      input_parameters.clear();
      restrictions.clear();

      scripts.clear();
      clear_impl();
    }

    void reset()
    {
      interpreter_->reset();
      for (std::map<std::string, InputParameter>::iterator it = input_parameters.begin(); it != input_parameters.end(); ++it)
      {
        it->second.set = false;
      }
    }

    virtual void read_geometry(pugi::xml_node const & node) = 0;
    void read(pugi::xml_node const & node);

    virtual void evaluate_geometry(output_parameter_interface & opi_mesh,
                                   output_parameter_interface & opi_hole_points,
                                   output_parameter_interface & opi_seed_points) = 0;
    void evaluate(output_parameter_interface & opi_mesh,
                  output_parameter_interface & opi_hole_points,
                  output_parameter_interface & opi_seed_points);



    template<typename T>
    void set_parameter(std::string const & parameter_name, T value)
    {
      std::map<std::string, InputParameter>::iterator it = input_parameters.find(parameter_name);
      if (it != input_parameters.end())
      {
        if (it->second.type == result_of::to_parameter<T>::value)
          set_paramater_impl(parameter_name, value);
        else
          throw base_geometry_generator_exception("Parameter type missmatch. Expected bool, got \"" + parameter_type_to_string(it->second.type) + "\"");

        it->second.set = true;
      }
      else
        throw base_geometry_generator_exception("Parameter \"" + parameter_name + "\" not found");
    }

    void set_parameter(std::string const & parameter_name, std::string const & value)
    {
      std::map<std::string, InputParameter>::iterator it = input_parameters.find(parameter_name);
      if (it != input_parameters.end())
      {
        if (it->second.type == PARAMETER_TYPE_BOOL)
          interpreter_->set_bool(parameter_name, value);
        else if (it->second.type == PARAMETER_TYPE_INT)
          interpreter_->set_int(parameter_name, value);
        else if (it->second.type == PARAMETER_TYPE_DOUBLE)
          interpreter_->set_double(parameter_name, value);
        else if (it->second.type == PARAMETER_TYPE_STRING)
          interpreter_->set_string(parameter_name, value);
        else
          throw base_geometry_generator_exception("Parameter \"" + parameter_name + "\" has not supported type \"" + parameter_type_to_string(it->second.type) + "\"");

        it->second.set = true;
      }
      else
        throw base_geometry_generator_exception("Parameter \"" + parameter_name + "\" not found");
    }

    void set_parameters(base_algorithm & algorithm);

    static boost::shared_ptr<base_geometry_generator> make(pugi::xml_node const & node);


  protected:



    void set_paramater_impl(std::string const & parameter_name, bool value)
    { interpreter_->set_bool(parameter_name, value); }
    void set_paramater_impl(std::string const & parameter_name, int value)
    { interpreter_->set_int(parameter_name, value); }
    void set_paramater_impl(std::string const & parameter_name, double value)
    { interpreter_->set_double(parameter_name, value); }
    void set_paramater_impl(std::string const & parameter_name, std::string const & value)
    { interpreter_->set_string(parameter_name, value); }
    void set_paramater_impl(std::string const & parameter_name, vec2 const & value)
    { interpreter_->set_vec2(parameter_name, value); }
    void set_paramater_impl(std::string const & parameter_name, vec3 const & value)
    { interpreter_->set_vec3(parameter_name, value); }


    boost::shared_ptr<vector_interpreter> interpreter_;

    std::map<std::string, InputParameter> input_parameters;
    std::vector<std::string> restrictions;

    std::vector<std::string> scripts;
  };
}

#endif
