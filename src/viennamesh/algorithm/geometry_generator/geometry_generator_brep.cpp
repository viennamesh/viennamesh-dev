#include "viennamesh/algorithm/geometry_generator/geometry_generator_brep.hpp"

namespace viennamesh
{
  namespace geometry_generator_brep
  {

    void base_brep_geometry_generator::read_geometry(pugi::xml_node const & node)
    {
      pugi::xml_node xml_vertices = node.child("vertices");
      if (!xml_vertices)
        throw geometry_generator_brep_exception("XML node \"vertices\" not found");


      for (pugi::xml_node xml_vertex = xml_vertices.child("vertex");
                          xml_vertex;
                          xml_vertex = xml_vertex.next_sibling("vertex"))
      {
        pugi::xml_attribute xml_vertex_id = xml_vertex.attribute("id");
        int vertex_id = vertices.empty() ? 0 : vertices.rbegin()->first + 1;
        if (xml_vertex_id)
          vertex_id = xml_vertex_id.as_int();

        if ( vertices.find(vertex_id) != vertices.end() )
          throw geometry_generator_brep_exception("Vertex with ID " + lexical_cast<std::string>(vertex_id) + " is already present");

        vertices[vertex_id] = point(xml_vertex);
      }


      pugi::xml_node xml_hole_points = node.child("hole_points");
      if (xml_hole_points)
      {
        for (pugi::xml_node xml_hole_point = xml_hole_points.child("hole_point");
                            xml_hole_point;
                            xml_hole_point = xml_hole_point.next_sibling("hole_point"))
        {
          hole_points.push_back( point(xml_hole_point) );
        }
      }


      pugi::xml_node xml_seed_points = node.child("seed_points");
      if (xml_seed_points)
      {
        for (pugi::xml_node xml_seed_point = xml_seed_points.child("seed_point");
                            xml_seed_point;
                            xml_seed_point = xml_seed_point.next_sibling("seed_point"))
        {
          seed_points.push_back( seed_point(xml_seed_point) );
        }
      }
    }















    void brep_2d_geometry_generator::read_geometry(pugi::xml_node const & node)
    {
      pugi::xml_node xml_geometry = node.child("geometry");
      if (!xml_geometry)
        throw geometry_generator_brep_exception("XML node \"geometry\" not found");

      base_brep_geometry_generator::read_geometry(xml_geometry);

      pugi::xml_node xml_line_strips = xml_geometry.child("line_strips");
      if (!xml_geometry)
        throw geometry_generator_brep_exception("XML node \"line_strips\" not found");

      for (pugi::xml_node xml_line_strip = xml_line_strips.child("line_strip");
                          xml_line_strip;
                          xml_line_strip = xml_line_strip.next_sibling("line_strip"))
      {
        line_strips.push_back( line_strip(xml_line_strip) );
      }
    }


    void brep_2d_geometry_generator::evaluate_geometry(output_parameter_interface & opi_mesh,
                                                       output_parameter_interface & opi_hole_points,
                                                       output_parameter_interface & opi_seed_points)
    {
      output_parameter_proxy<MeshType> mesh(opi_mesh);
      output_parameter_proxy<point_2d_container> hole_points(opi_hole_points);
      output_parameter_proxy<seed_point_2d_container> seed_points(opi_seed_points);


      typedef viennagrid::result_of::vertex_handle<MeshType>::type VertexHandleType;
      std::map<int, VertexHandleType> vertex_handles;

      for (std::map<int, point>::iterator it = vertices.begin(); it != vertices.end(); ++it)
      {
        if (!it->second(interpreter_))
          continue;

        vertex_handles[it->first] = viennagrid::make_unique_vertex( mesh(), it->second.evaluate_vec2(interpreter_) );
      }


      for (std::size_t j = 0; j < line_strips.size(); ++j)
      {
        line_strip const & ls = line_strips[j];

        if ( !ls(interpreter_) )
          continue;

        std::map<int, VertexHandleType>::iterator vhit0;
        std::map<int, VertexHandleType>::iterator vhit1;

        for (std::size_t k = 1; k < ls.size(); ++k)
        {
          vhit0 = vertex_handles.find( ls[k-1] );
          if (vhit0 == vertex_handles.end())
            throw geometry_generator_brep_exception("Vertex with ID " + lexical_cast<std::string>(ls[k-1]) + "was not found (either it was not defined or not evaluated due to depend)");

          vhit1 = vertex_handles.find( ls[k] );
          if (vhit1 == vertex_handles.end())
            throw geometry_generator_brep_exception("Vertex with ID " + lexical_cast<std::string>(ls[k-1]) + "was not found (either it was not defined or not evaluated due to depend)");

          viennagrid::make_line(mesh(), vhit0->second, vhit1->second);
        }

        if (ls.size() > 2)
        {
          vhit0 = vertex_handles.find( ls.back() );
          if (vhit0 == vertex_handles.end())
            throw geometry_generator_brep_exception("Vertex with ID " + lexical_cast<std::string>(ls.back()) + "was not found (either it was not defined or not evaluated due to depend)");

          vhit1 = vertex_handles.find( ls.front() );
          if (vhit1 == vertex_handles.end())
            throw geometry_generator_brep_exception("Vertex with ID " + lexical_cast<std::string>(ls.front()) + "was not found (either it was not defined or not evaluated due to depend)");

          viennagrid::make_line(mesh(), vhit0->second, vhit1->second);
        }
      }


      for (std::size_t i = 0; i < this->hole_points.size(); ++i)
      {
        point const & hole_point = this->hole_points[i];

        if ( !hole_point(interpreter_) )
          continue;

        hole_points().push_back( hole_point.evaluate_vec2(interpreter_) );
      }

      for (std::size_t i = 0; i < this->seed_points.size(); ++i)
      {
        seed_point const & seed_point = this->seed_points[i];

        if ( !seed_point(interpreter_) )
          continue;

        seed_points().push_back( std::make_pair(seed_point.evaluate_vec2(interpreter_), seed_point.segment_id()) );
      }
    }












    void brep_3d_geometry_generator::read_geometry(pugi::xml_node const & node)
    {
      pugi::xml_node xml_geometry = node.child("geometry");
      base_brep_geometry_generator::read_geometry(xml_geometry);

      pugi::xml_node xml_plcs = xml_geometry.child("plcs");
      for (pugi::xml_node xml_plc = xml_plcs.child("plc");
                          xml_plc;
                          xml_plc = xml_plc.next_sibling("plc"))
      {
        plc new_plc;

        for (pugi::xml_node xml_line_strip = xml_plc.child("line_strip");
                            xml_line_strip;
                            xml_line_strip = xml_line_strip.next_sibling("line_strip"))
        {
          new_plc.line_strips.push_back( line_strip(xml_line_strip) );
        }


        for (pugi::xml_node xml_hole_point = xml_plc.child("hole_point");
                            xml_hole_point;
                            xml_hole_point = xml_hole_point.next_sibling("hole_point"))
        {
          new_plc.hole_points.push_back( point(xml_hole_point) );
        }

        plcs.push_back(new_plc);
      }
    }


    void brep_3d_geometry_generator::evaluate_geometry(output_parameter_interface & opi_mesh,
                                                       output_parameter_interface & opi_hole_points,
                                                       output_parameter_interface & opi_seed_points)
    {
      output_parameter_proxy<MeshType> mesh(opi_mesh);
      output_parameter_proxy<point_3d_container> hole_points(opi_hole_points);
      output_parameter_proxy<seed_point_3d_container> seed_points(opi_seed_points);


      typedef viennagrid::result_of::vertex_handle<MeshType>::type VertexHandleType;
      std::map<int, VertexHandleType> vertex_handles;

      for (std::map<int, point>::iterator it = vertices.begin(); it != vertices.end(); ++it)
      {
        if (!it->second(interpreter_))
          continue;

        vertex_handles[it->first] = viennagrid::make_unique_vertex( mesh(), it->second.evaluate_vec3(interpreter_) );
      }

      for (std::size_t i = 0; i < plcs.size(); ++i)
      {
        typedef viennagrid::result_of::line_handle<MeshType>::type LineHandleType;
        std::vector<LineHandleType> line_handles;

        for (std::size_t j = 0; j < plcs[i].line_strips.size(); ++j)
        {
          line_strip const & ls = plcs[i].line_strips[j];

          if ( !ls(interpreter_) )
            continue;

          std::map<int, VertexHandleType>::iterator vhit0;
          std::map<int, VertexHandleType>::iterator vhit1;

          for (std::size_t k = 1; k < ls.size(); ++k)
          {
            vhit0 = vertex_handles.find( ls[k-1] );
            if (vhit0 == vertex_handles.end())
              throw geometry_generator_brep_exception("Vertex with ID " + lexical_cast<std::string>(ls[k-1]) + "was not found (either it was not defined or not evaluated due to depend)");

            vhit1 = vertex_handles.find( ls[k] );
            if (vhit1 == vertex_handles.end())
              throw geometry_generator_brep_exception("Vertex with ID " + lexical_cast<std::string>(ls[k-1]) + "was not found (either it was not defined or not evaluated due to depend)");

            line_handles.push_back( viennagrid::make_line(mesh(), vhit0->second, vhit1->second) );
          }

          if (ls.size() > 2)
          {
            vhit0 = vertex_handles.find( ls.back() );
            if (vhit0 == vertex_handles.end())
              throw geometry_generator_brep_exception("Vertex with ID " + lexical_cast<std::string>(ls.back()) + "was not found (either it was not defined or not evaluated due to depend)");

            vhit1 = vertex_handles.find( ls.front() );
            if (vhit1 == vertex_handles.end())
              throw geometry_generator_brep_exception("Vertex with ID " + lexical_cast<std::string>(ls.front()) + "was not found (either it was not defined or not evaluated due to depend)");

            line_handles.push_back( viennagrid::make_line(mesh(), vhit0->second, vhit1->second) );
          }
        }

        typedef viennagrid::result_of::plc_handle<MeshType>::type PLCHandleType;
        typedef viennagrid::result_of::plc<MeshType>::type PLCType;

        PLCHandleType plc_handle = viennagrid::make_plc(mesh(), line_handles.begin(), line_handles.end());
        PLCType & new_plc = viennagrid::dereference_handle(mesh(), plc_handle);

        for (std::size_t j = 0; j < plcs[i].hole_points.size(); ++j)
        {
          point const & hole_point = plcs[i].hole_points[j];

          if ( !hole_point(interpreter_) )
            continue;

          viennagrid::hole_points(new_plc).push_back( hole_point.evaluate_vec3(interpreter_) );
        }
      }

      for (std::size_t i = 0; i < this->hole_points.size(); ++i)
      {
        point const & hole_point = this->hole_points[i];

        if ( !hole_point(interpreter_) )
          continue;

        hole_points().push_back( hole_point.evaluate_vec3(interpreter_) );
      }

      for (std::size_t i = 0; i < this->seed_points.size(); ++i)
      {
        seed_point const & seed_point = this->seed_points[i];

        if ( !seed_point(interpreter_) )
          continue;

        seed_points().push_back( std::make_pair(seed_point.evaluate_vec3(interpreter_), seed_point.segment_id()) );
      }
    }

  }
}
