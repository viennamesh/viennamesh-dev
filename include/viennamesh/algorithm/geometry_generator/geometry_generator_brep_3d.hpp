#ifndef VIENNAMESH_ALGORITHM_GEOMETRY_GENERATOR_BREP_3D_HPP
#define VIENNAMESH_ALGORITHM_GEOMETRY_GENERATOR_BREP_3D_HPP

#include "viennamesh/algorithm/geometry_generator/base_geometry_generator.hpp"

namespace viennamesh
{
  namespace geometry_generator_brep_3d
  {

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



    class geometry_generator : public base_geometry_generator
    {
    public:

      geometry_generator() {}

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

  }
}

#endif
