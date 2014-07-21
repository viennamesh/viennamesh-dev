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
    PARAMETER_TYPE_STRING,
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



  struct InputParameter
  {
    ParameterType type;
    std::string default_value;
    bool set;
  };


//   class python_interpreter_exception : public std::runtime_error
//   {
//   public:
//     python_interpreter_exception(std::string const & message_) : std::runtime_error(message_) {}
//     virtual ~python_interpreter_exception() throw() {}
//   };



  class base_geometry_generator
  {
  public:

    typedef viennagrid::config::point_type_2d vec2;
    typedef viennagrid::config::point_type_3d vec3;


    base_geometry_generator() : interpreter_(0) {}
    virtual ~base_geometry_generator() {}

    virtual void clear_impl() = 0;
    void clear()
    {
      delete interpreter_;

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
    void read(pugi::xml_node const & node)
    {
      clear();

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
        restrictions.push_back( xml_restriction.first_child().value() );
      }




      for (pugi::xml_node xml_script = node.child("script");
                          xml_script;
                          xml_script = xml_script.next_sibling("script"))
      {
        scripts.push_back( xml_script.first_child().value() );
      }
    }

    virtual void run_impl(output_parameter_interface & opi) = 0;
    void run(output_parameter_interface & opi)
    {
      for (std::map<std::string, InputParameter>::iterator it = input_parameters.begin(); it != input_parameters.end(); ++it)
      {
        if (!it->second.set)
        {
          error(1) << "ERROR: parameter " << it->first << " not set" << std::endl;
          //TODO throw

          if (it->second.default_value.empty())
          {
            error(1) << "ERROR: parameter without default value was not set" << std::endl;
            //TODO throw
          }
        }
      }


      for (std::size_t i = 0; i < restrictions.size(); ++i)
      {
        if (!interpreter_->evaluate_bool( restrictions[i] ))
        {
          error(1) << "ERROR: Restriction \"" << restrictions[i] << "\" failed" << std::endl;
          //TODO throw
        }
      }



      for (std::size_t i = 0; i < scripts.size(); ++i)
        interpreter_->run_script( scripts[i] );


      run_impl(opi);
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





    vector_interpreter * interpreter_;

    std::map<std::string, InputParameter> input_parameters;
    std::vector<std::string> restrictions;

    std::vector<std::string> scripts;
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



  class geometry_generator_brep_3d : public base_geometry_generator
  {
  public:

    geometry_generator_brep_3d() {}

    void read_impl(pugi::xml_node const & node)
    {
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


    void run_impl(output_parameter_interface & opi)
    {
      output_parameter_proxy<viennagrid::brep_3d_mesh> mesh(opi);

      for (std::map<int, Vertex>::iterator it = vertices.begin(); it != vertices.end(); ++it)
      {
        vec3 point;
        if (it->second.coordinated)
          point = interpreter_->evaluate_vec3_coordinates( it->second.expression );
        else
          point = interpreter_->evaluate_vec3( it->second.expression );

        it->second.vertex_handle = viennagrid::make_unique_vertex( mesh(), point );
      }

      for (std::size_t i = 0; i < plcs.size(); ++i)
      {
        PLC const & plc = plcs[i];

        typedef viennagrid::result_of::line_handle<viennagrid::brep_3d_mesh>::type LineHandleType;
        std::vector<LineHandleType> line_handles;

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

            line_handles.push_back( viennagrid::make_line(mesh(), v0, v1) );
          }

          if (line_strip.vertices.size() > 2)
          {
            VertexHandleType v0 = vertices[line_strip.vertices.back()].vertex_handle;
            VertexHandleType v1 = vertices[line_strip.vertices.front()].vertex_handle;

            line_handles.push_back( viennagrid::make_line(mesh(), v0, v1) );
          }
        }

        viennagrid::make_plc(mesh(), line_handles.begin(), line_handles.end());
      }
    }

  private:

    void clear_impl()
    {
      vertices.clear();
      plcs.clear();
    }

    std::map<int, Vertex> vertices;
    std::vector<PLC> plcs;
  };




  boost::shared_ptr<base_geometry_generator> base_geometry_generator::make(pugi::xml_node const & node)
  {
    pugi::xml_attribute xml_dimension = node.attribute("dimension");
    int dimension = xml_dimension.as_int();

    pugi::xml_attribute xml_geometry_type = node.attribute("geometry_type");
    std::string geometry_type = xml_geometry_type.as_string();
    std::transform(geometry_type.begin(), geometry_type.end(), geometry_type.begin(), ::toupper);

    if (dimension == 3 && geometry_type == "BREP")
    {
      boost::shared_ptr<base_geometry_generator> generator(new geometry_generator_brep_3d);
      generator->read(node);
      return generator;
    }

    error(1) << "No generator found for dimension " << dimension << " and geometry type \"" << geometry_type << "\"" << std::endl;
    return boost::shared_ptr<base_geometry_generator>();
  }











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


    boost::shared_ptr<base_geometry_generator> generator = base_geometry_generator::make(xml_template.first_child());

//     GeometryGeneratorBREP3D generator;
//     generator.read(xml_template.first_child());

//     output_parameter_proxy<viennagrid::brep_3d_mesh> omp(output_mesh);

    generator->set_parameters(*this);
    generator->run(output_mesh);

    return true;
  }

}

#endif
