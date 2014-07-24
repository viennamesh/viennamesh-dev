#include "viennamesh/algorithm/geometry_generator/base_geometry_generator.hpp"
#include "viennamesh/utils/interpreter/python_interpreter.hpp"

#include "viennamesh/algorithm/geometry_generator/geometry_generator_brep.hpp"

namespace viennamesh
{

  ParameterType parameter_type_from_string(std::string const & str)
  {
    if (str == "bool")
      return PARAMETER_TYPE_BOOL;
    else if (str == "int")
      return PARAMETER_TYPE_INT;
    else if (str == "double")
      return PARAMETER_TYPE_DOUBLE;
    else if (str == "string")
      return PARAMETER_TYPE_STRING;
    else if (str == "vec2")
      return PARAMETER_TYPE_VEC2;
    else if (str == "vec3")
      return PARAMETER_TYPE_VEC3;

    return PARAMETER_TYPE_UNKOWN;
  }

  std::string parameter_type_to_string(ParameterType ptype)
  {
    switch (ptype)
    {
      case PARAMETER_TYPE_BOOL:
        return "bool";
      case PARAMETER_TYPE_INT:
        return "int";
      case PARAMETER_TYPE_DOUBLE:
        return "double";
      case PARAMETER_TYPE_STRING:
        return "string";
      case PARAMETER_TYPE_VEC2:
        return "vec2";
      case PARAMETER_TYPE_VEC3:
        return "vec3";
      default:
        return "unknown";
    }
  }




  void base_geometry_generator::read(pugi::xml_node const & node)
  {
    clear();

    pugi::xml_attribute xml_scripting_language = node.attribute("scripting_language");
    if (!xml_scripting_language)
      throw base_geometry_generator_exception("XML Attribute scripting_language not found");
    std::string scripting_language = xml_scripting_language.as_string();

    interpreter_ = vector_interpreter::make(scripting_language);
    if (!interpreter_)
      throw base_geometry_generator_exception("Failed to create interpreter: \"" + scripting_language + "\"");


    pugi::xml_node xml_input_parameters = node.child("input_parameters");
    if (!xml_input_parameters)
      throw base_geometry_generator_exception("XML node \"input_parameters\" not found");

    for (pugi::xml_node xml_parameter = xml_input_parameters.child("parameter");
                        xml_parameter;
                        xml_parameter = xml_parameter.next_sibling("parameter"))
    {
      pugi::xml_attribute xml_parameter_name = xml_parameter.attribute("name");
      if (!xml_parameter_name)
        throw base_geometry_generator_exception("Input parameter: XML attribute \"name\" not found");
      std::string parameter_name = xml_parameter_name.as_string();

      pugi::xml_attribute xml_parameter_type = xml_parameter.attribute("type");
      if (!xml_parameter_type)
        throw base_geometry_generator_exception("Input parameter: XML attribute \"type\" not found");
      std::string parameter_type = xml_parameter_type.as_string();

      InputParameter input_parameter;
      input_parameter.type = parameter_type_from_string(parameter_type);

      if (xml_parameter.child_value("default"))
        input_parameter.default_value_expression = xml_parameter.child_value("default");

      input_parameters[parameter_name] = input_parameter;
    }

    for (pugi::xml_node xml_restriction = xml_input_parameters.child("restriction");
                        xml_restriction;
                        xml_restriction = xml_restriction.next_sibling("restriction"))
    {
      restrictions.push_back( xml_restriction.first_child().value() );
    }

    for (pugi::xml_node xml_script = node.child("script");
                        xml_script;
                        xml_script = xml_script.next_sibling("script"))
    {
      scripts.push_back( xml_script.first_child().value() );
    }

    read_geometry(node);
  }



  void base_geometry_generator::evaluate(output_parameter_interface & opi_mesh,
                                         output_parameter_interface & opi_seed_points,
                                         output_parameter_interface & opi_hole_points)
  {
    for (std::map<std::string, InputParameter>::iterator it = input_parameters.begin(); it != input_parameters.end(); ++it)
    {
      if (!it->second.set)
      {


//         throw base_geometry_generator_exception("Parameter " + it->first + " not set");

        if (it->second.default_value_expression.empty())
        {
          throw base_geometry_generator_exception("Parameter without default value was not set");
        }

        set_parameter(it->first, it->second.default_value_expression);
      }
    }


    for (std::size_t i = 0; i < restrictions.size(); ++i)
    {
      if (!interpreter_->evaluate_bool( restrictions[i] ))
      {
        throw base_geometry_generator_exception("Restriction \"" + restrictions[i] + "\" failed");
      }
    }



    for (std::size_t i = 0; i < scripts.size(); ++i)
      interpreter_->run_script( scripts[i] );


    evaluate_geometry(opi_mesh, opi_seed_points, opi_hole_points);
  }


  void base_geometry_generator::set_parameters(base_algorithm & algorithm)
  {
    for (std::map<std::string, InputParameter>::iterator it = input_parameters.begin();
                                                          it != input_parameters.end();
                                                        ++it)
    {
      if (it->second.type == PARAMETER_TYPE_BOOL)
      {
        optional_input_parameter_interface<bool> param(algorithm, parameter_information(it->first));
        if (param.valid())
          set_parameter(it->first, param());
      }
      else if (it->second.type == PARAMETER_TYPE_INT)
      {
        optional_input_parameter_interface<int> param(algorithm, parameter_information(it->first));
        if (param.valid())
          set_parameter(it->first, param());
      }
      else if (it->second.type == PARAMETER_TYPE_DOUBLE)
      {
        optional_input_parameter_interface<double> param(algorithm, parameter_information(it->first));
        if (param.valid())
          set_parameter(it->first, param());
      }
      else
        throw base_geometry_generator_exception("Parameter type \"" + parameter_type_to_string(it->second.type) + "\" unsupported");
    }
  }









  boost::shared_ptr<base_geometry_generator> base_geometry_generator::make(pugi::xml_node const & node)
  {
    pugi::xml_attribute xml_dimension = node.attribute("dimension");
    int dimension = xml_dimension.as_int();

    pugi::xml_attribute xml_geometry_type = node.attribute("geometry_type");
    std::string geometry_type = xml_geometry_type.as_string();
    std::transform(geometry_type.begin(), geometry_type.end(), geometry_type.begin(), ::toupper);

    if (dimension == 3 && geometry_type == "BREP")
    {
      boost::shared_ptr<base_geometry_generator> generator(new geometry_generator_brep::brep_3d_geometry_generator());
      generator->read(node);
      return generator;
    }
    else if (dimension == 2 && geometry_type == "BREP")
    {
      boost::shared_ptr<base_geometry_generator> generator(new geometry_generator_brep::brep_2d_geometry_generator());
      generator->read(node);
      return generator;
    }

    error(1) << "No generator found for dimension " << dimension << " and geometry type \"" << geometry_type << "\"" << std::endl;
    return boost::shared_ptr<base_geometry_generator>();
  }

}
