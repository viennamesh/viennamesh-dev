#ifdef VIENNAMESH_WITH_GEOMETRY_GENERATOR

#include "viennamesh/algorithm/geometry_generator/geometry_generator.hpp"
#include "viennamesh/algorithm/geometry_generator/python_interpreter.hpp"

#include "pugixml/pugixml.hpp"

#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/mesh/element_creation.hpp"

#include "viennamesh/utils/string_tools.hpp"
using stringtools::lexical_cast;

namespace viennamesh
{

  enum ParameterType
  {
    PARAMETER_TYPE_UNKOWN,
    PARAMETER_TYPE_BOOL,
    PARAMETER_TYPE_INT,
    PARAMETER_TYPE_DOUBLE,
    PARAMETER_TYPE_VEC2,
    PARAMETER_TYPE_VEC3,
  };

  ParameterType parameter_type_from_string(std::string const & str)
  {
    if (str == "bool")
      return PARAMETER_TYPE_BOOL;
    else if (str == "int")
      return PARAMETER_TYPE_INT;
    else if (str == "double")
      return PARAMETER_TYPE_DOUBLE;
    else if (str == "vec2")
      return PARAMETER_TYPE_VEC2;
    else if (str == "vec3")
      return PARAMETER_TYPE_VEC3;

    return PARAMETER_TYPE_UNKOWN;
  }



  struct InputParameter
  {
    ParameterType type;
    std::string default_value;
    bool set;
  };


  struct Vertex
  {
    Vertex() {}
    Vertex(bool coordinated_, std::string const & expression_) : coordinated(coordinated_), expression(expression_) {}

    bool coordinated;
    std::string expression;

    typedef viennagrid::result_of::vertex_handle<viennagrid::brep_3d_mesh>::type VertexHandleType;
    VertexHandleType vertex_handle;
  };



  struct PLC_line_strip
  {
    std::vector<int> vertices;
    std::string depend;
  };

  struct PLC
  {
    std::vector<PLC_line_strip> line_strips;
  };



  class GeometryGeneratorBREP3D
  {
  public:

    GeometryGeneratorBREP3D() : interpreter_(0) {}

    typedef viennagrid::config::point_type_2d vec2;
    typedef viennagrid::config::point_type_3d vec3;

    void read(pugi::xml_node const & node)
    {
      clear();

      pugi::xml_attribute xml_dimension = node.attribute("dimension");
      int dimension = xml_dimension.as_int();

      pugi::xml_attribute xml_geometry_type = node.attribute("geometry_type");
      std::string geometry_type = xml_geometry_type.as_string();

      pugi::xml_attribute xml_scripting_language = node.attribute("scripting_language");
      std::string scripting_language = xml_scripting_language.as_string();

      if (scripting_language == "Python")
        interpreter_ = new numpy_vector_interpreter();


      pugi::xml_node xml_input_parameters = node.child("input_parameters");


      for (pugi::xml_node xml_parameter = xml_input_parameters.child("parameter");
                          xml_parameter;
                          xml_parameter = xml_parameter.next_sibling("parameter"))
      {
        pugi::xml_attribute xml_parameter_name = xml_parameter.attribute("name");
        std::string parameter_name = xml_parameter_name.as_string();

        pugi::xml_attribute xml_parameter_type = xml_parameter.attribute("type");
        std::string parameter_type = xml_parameter_type.as_string();

        InputParameter input_parameter;
        input_parameter.type = parameter_type_from_string(parameter_type);

        if (xml_parameter.child_value("default"))
          input_parameter.default_value = xml_parameter.child_value("default");

        input_parameters[parameter_name] = input_parameter;
      }



      for (pugi::xml_node xml_restriction = xml_input_parameters.child("restriction");
                          xml_restriction;
                          xml_restriction = xml_restriction.next_sibling("restriction"))
      {
        restrictions.push_back( xml_restriction.value() );
      }




      for (pugi::xml_node xml_script = node.child("script");
                          xml_script;
                          xml_script = xml_script.next_sibling("script"))
      {
        scripts.push_back( xml_script.value() );
      }




      pugi::xml_node xml_geometry = node.child("geometry");
      pugi::xml_node xml_vertices = xml_geometry.child("vertices");
      pugi::xml_node xml_plcs = xml_geometry.child("plcs");

      for (pugi::xml_node xml_vertex = xml_vertices.child("vertex");
                          xml_vertex;
                          xml_vertex = xml_vertex.next_sibling("vertex"))
      {
        pugi::xml_attribute xml_vertex_type = xml_vertex.attribute("type");
        std::string vertex_type = xml_vertex_type.value();

        pugi::xml_attribute xml_vertex_id = xml_vertex.attribute("id");
        int vertex_id = xml_vertex_id.as_int();

        std::string expression = xml_vertex.first_child().value();

        vertices[vertex_id] = Vertex( vertex_type == "coordinated", expression );
      }

      for (pugi::xml_node xml_plc = xml_plcs.child("plc");
                          xml_plc;
                          xml_plc = xml_plc.next_sibling("plc"))
      {
        PLC plc;

        for (pugi::xml_node xml_line_strip = xml_plc.child("line_strip");
                            xml_line_strip;
                            xml_line_strip = xml_line_strip.next_sibling("line_strip"))
        {
          PLC_line_strip line_strip;

          std::list<std::string> vertices = stringtools::split_string( xml_line_strip.first_child().value(), " " );
          for (std::list<std::string>::const_iterator sit = vertices.begin(); sit != vertices.end(); ++sit)
          {
            line_strip.vertices.push_back( stringtools::lexical_cast<int>(*sit) );
          }

          pugi::xml_attribute xml_depend = xml_line_strip.attribute("depend");
          if (xml_depend)
            line_strip.depend = xml_depend.as_string();

          plc.line_strips.push_back(line_strip);
        }

        plcs.push_back(plc);
      }
    }



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



    void set_parameters(base_algorithm & algorithm)
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
          error(1) << "Parameter type \"" << it->second.type << "\" unsupported" << std::endl;
      }
    }


    void run( viennagrid::brep_3d_mesh & mesh )
    {
      for (std::map<std::string, InputParameter>::iterator it = input_parameters.begin(); it != input_parameters.end(); ++it)
      {
        if (!it->second.set)
        {
          std::cout << "ERROR: parameter " << it->first << " not set" << std::endl;
          return;

          if (it->second.default_value.empty())
          {
            std::cout << "ERROR: parameter without default value was not set" << std::endl;
            return;
          }
        }
      }

      for (std::size_t i = 0; i < scripts.size(); ++i)
        interpreter_->run_script( scripts[i] );

      for (std::map<int, Vertex>::iterator it = vertices.begin(); it != vertices.end(); ++it)
      {
        vec3 point;
        if (it->second.coordinated)
          point = interpreter_->evaluate_vec3_coordinates( it->second.expression );
        else
          point = interpreter_->evaluate_vec3( it->second.expression );

        it->second.vertex_handle = viennagrid::make_unique_vertex( mesh, point );

//         std::cout << "[" << it->first << "] Added vertex " << point << std::endl;
      }

      for (std::size_t i = 0; i < plcs.size(); ++i)
      {
        PLC const & plc = plcs[i];

        typedef viennagrid::result_of::line_handle<viennagrid::brep_3d_mesh>::type LineHandleType;
        std::vector<LineHandleType> line_handles;

//         std::cout << "Added PLC" << std::endl;

        for (std::size_t j = 0; j < plc.line_strips.size(); ++j)
        {
          PLC_line_strip const & line_strip = plc.line_strips[j];

          if ( !line_strip.depend.empty() && !interpreter_->evaluate_bool(line_strip.depend) )
            continue;

          typedef viennagrid::result_of::vertex_handle<viennagrid::brep_3d_mesh>::type VertexHandleType;

          for (std::size_t k = 1; k < line_strip.vertices.size(); ++k)
          {
            VertexHandleType v0 = vertices[line_strip.vertices[k-1]].vertex_handle;
            VertexHandleType v1 = vertices[line_strip.vertices[k]].vertex_handle;

            line_handles.push_back( viennagrid::make_line(mesh, v0, v1) );

//             std::cout << "  line [" << line_strip.vertices[k-1] << "," << line_strip.vertices[k] << "]" << std::endl;
          }

          if (line_strip.vertices.size() > 2)
          {
            VertexHandleType v0 = vertices[line_strip.vertices.back()].vertex_handle;
            VertexHandleType v1 = vertices[line_strip.vertices.front()].vertex_handle;

            line_handles.push_back( viennagrid::make_line(mesh, v0, v1) );

//             std::cout << "  line [" << line_strip.vertices.back() << "," << line_strip.vertices.front() << "]" << std::endl;
          }
        }

        viennagrid::make_plc(mesh, line_handles.begin(), line_handles.end());
      }
    }





  private:

    void set_paramater_impl(std::map<std::string, InputParameter>::iterator it, std::string const & parameter_name, bool value)
    {
      if (it->second.type == PARAMETER_TYPE_BOOL)
        interpreter_->set_bool(parameter_name, value);
      else
      {
        std::cout << "ERROR: parameter type missmatch" << std::endl;
        // TODO: throw
      }
    }

    void set_paramater_impl(std::map<std::string, InputParameter>::iterator it, std::string const & parameter_name, int value)
    {
      if (it->second.type == PARAMETER_TYPE_INT)
        interpreter_->set_int(parameter_name, value);
      else
      {
        std::cout << "ERROR: parameter type missmatch" << std::endl;
        // TODO: throw
      }
    }

    void set_paramater_impl(std::map<std::string, InputParameter>::iterator it, std::string const & parameter_name, double value)
    {
      if (it->second.type == PARAMETER_TYPE_DOUBLE)
        interpreter_->set_double(parameter_name, value);
      else
      {
        std::cout << "ERROR: parameter type missmatch" << std::endl;
        // TODO: throw
      }
    }

    void set_paramater_impl(std::map<std::string, InputParameter>::iterator it, std::string const & parameter_name, vec2 value)
    {
      if (it->second.type == PARAMETER_TYPE_VEC2)
        interpreter_->set_vec2(parameter_name, value);
      else
      {
        std::cout << "ERROR: parameter type missmatch" << std::endl;
        // TODO: throw
      }
    }

    void set_paramater_impl(std::map<std::string, InputParameter>::iterator it, std::string const & parameter_name, vec3 value)
    {
      if (it->second.type == PARAMETER_TYPE_VEC3)
        interpreter_->set_vec3(parameter_name, value);
      else
      {
        std::cout << "ERROR: parameter type missmatch" << std::endl;
        // TODO: throw
      }
    }

  //   template<typename T>
  //   void set_parameter(std::string const & parameter_name, T const & value)
  //   {
  //     std::map<std::string, InputParameter>::iterator it = input_parameters.find(parameter_name);
  //     if (it != input_parameters.end())
  //       it->second.last_value = value;
  //     else
  //     {
  //       std::cout << "ERROR" << std::endl;
  //     }
  //   }

    void reset()
    {
      interpreter_->reset();
      for (std::map<std::string, InputParameter>::iterator it = input_parameters.begin(); it != input_parameters.end(); ++it)
      {
        it->second.set = false;
      }
    }

    void clear()
    {
      delete interpreter_;

      input_parameters.clear();
      restrictions.clear();

      scripts.clear();

      vertices.clear();
      plcs.clear();
    }

    vector_interpreter * interpreter_;

    std::map<std::string, InputParameter> input_parameters;
    std::vector<std::string> restrictions;

    std::vector<std::string> scripts;

    std::map<int, Vertex> vertices;
    std::vector<PLC> plcs;
  };
















  geometry_generator::geometry_generator() :
    base_algorithm(true),
    filename(*this, parameter_information("filename","string","The filename of the input template")),
    output_mesh(*this, parameter_information("mesh", "mesh", "The output mesh, same type of mesh as input mesh")) {}


  std::string geometry_generator::name() const { return "ViennaMesh Templated Based Geometry Generator"; }
  std::string geometry_generator::id() const { return "geometry_generator"; }


  bool geometry_generator::run_impl()
  {
    pugi::xml_document xml_template;
    pugi::xml_parse_result result = xml_template.load_file( filename().c_str() );

    GeometryGeneratorBREP3D generator;
    generator.read(xml_template.first_child());


//     optional_input_parameter_interface<double> cube1_length(*this, parameter_information("cube1_length"));
//     generator.set_parameter("cube1_length", cube1_length());
//
//     optional_input_parameter_interface<double> cube2_length(*this, parameter_information("cube2_length"));
//     generator.set_parameter("cube2_length", cube2_length());

    output_parameter_proxy<viennagrid::brep_3d_mesh> omp(output_mesh);

    generator.set_parameters(*this);
    generator.run( omp() );

    return true;
  }

}

#endif
