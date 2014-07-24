#ifndef VIENNAMESH_ALGORITHM_GEOMETRY_GENERATOR_BREP_HPP
#define VIENNAMESH_ALGORITHM_GEOMETRY_GENERATOR_BREP_HPP

#include "viennamesh/algorithm/geometry_generator/base_geometry_generator.hpp"

namespace viennamesh
{
  namespace geometry_generator_brep
  {

    class geometry_generator_brep_exception : public base_geometry_generator_exception
    {
    public:
      geometry_generator_brep_exception(std::string const & message_) : base_geometry_generator_exception(message_) {}
      virtual ~geometry_generator_brep_exception() throw() {}
    };




    class depend
    {
    public:

      void read(pugi::xml_node const & xml)
      {
        pugi::xml_attribute xml_depend = xml.attribute("depend");
        if (xml_depend)
          depend_ = xml_depend.as_string();
      }

      bool operator()(boost::shared_ptr<vector_interpreter> const & interpreter_) const
      {
        if ( !depend_.empty() )
          return interpreter_->evaluate_bool(depend_);

        return true;
      }

    private:
      std::string depend_;
    };

    class point : public depend
    {
    public:

      typedef viennagrid::config::point_type_2d vec2;
      typedef viennagrid::config::point_type_3d vec3;

      point() {}
      point(pugi::xml_node const & xml) { read(xml); }

      void read(pugi::xml_node const & xml)
      {
        depend::read(xml);

        pugi::xml_attribute xml_vertex_is_coordinated = xml.attribute("coordinated");
        if (xml_vertex_is_coordinated && xml_vertex_is_coordinated.as_bool())
          coordinated_ = true;

        expression_ = xml.first_child().value();
        if (expression_.empty())
          throw geometry_generator_brep_exception("A point as empty expression.");
      }

      vec2 evaluate_vec2(boost::shared_ptr<vector_interpreter> const & interpreter_) const
      {
        if (coordinated_)
          return interpreter_->evaluate_vec2_coordinated( expression_ );
        else
          return interpreter_->evaluate_vec2( expression_ );
      }

      vec3 evaluate_vec3(boost::shared_ptr<vector_interpreter> const & interpreter_) const
      {
        if (coordinated_)
          return interpreter_->evaluate_vec3_coordinated( expression_ );
        else
          return interpreter_->evaluate_vec3( expression_ );
      }

    private:
      bool coordinated_;
      std::string expression_;
    };

    class line_strip : public depend
    {
    public:

      line_strip() {}
      line_strip(pugi::xml_node const & xml) { read(xml); }

      void read(pugi::xml_node const & xml)
      {
        depend::read(xml);

        pugi::xml_attribute xml_vertex_count = xml.attribute("count");
        if (!xml_vertex_count)
          throw geometry_generator_brep_exception("A line strip does not have a count attribute");

        if (xml_vertex_count.as_int(-1) < 0)
          throw geometry_generator_brep_exception("A line strip does have an invalid count attribute");

        std::list<std::string> vertex_strings = stringtools::split_string( xml.first_child().value(), " " );
        if (xml_vertex_count.as_int() != vertex_strings.size())
          throw geometry_generator_brep_exception("Line strip vertex count missmatch. Expected: " + lexical_cast<std::string>(xml_vertex_count.as_int()) + ", got " + lexical_cast<std::string>(vertex_strings.size()) + ": \"" + xml.first_child().value() + "\"");

        if (vertex_strings.size() < 2)
          throw geometry_generator_brep_exception(std::string("A PLC has less than 2 vertices: \"") + xml.first_child().value() + "\"");

        for (std::list<std::string>::const_iterator sit = vertex_strings.begin(); sit != vertex_strings.end(); ++sit)
          vertices.push_back( stringtools::lexical_cast<int>(*sit) );
      }

      std::size_t size() const { return vertices.size(); }
      int const & operator[](std::size_t index) const { return vertices[index]; }
      int const & front() const { return vertices.front(); }
      int const & back() const { return vertices.back(); }

    private:

      std::vector<int> vertices;
    };

    class seed_point : public point
    {
    public:

      seed_point() {}
      seed_point(pugi::xml_node const & xml) { read(xml); }

      void read(pugi::xml_node const & xml)
      {
        point::read(xml);

        pugi::xml_attribute xml_segment_id = xml.attribute("segment_id");
        if (!xml_segment_id)
          throw geometry_generator_brep_exception("A seed point has no segment ID attribute!");

        segment_id_ = xml_segment_id.as_int();
      }

      int segment_id() const { return segment_id_; }

    private:
      int segment_id_;
    };

    struct plc
    {
      std::vector<line_strip> line_strips;
      std::vector<point> hole_points;
    };








    class base_brep_geometry_generator : public base_geometry_generator
    {
    public:

      base_brep_geometry_generator() {}

      void read_geometry(pugi::xml_node const & node);

    protected:

      void clear_impl()
      {
        vertices.clear();
        plcs.clear();
        hole_points.clear();
        seed_points.clear();
      }

      std::map<int, point> vertices;
      std::vector<line_strip> plcs;

      std::vector<point> hole_points;
      std::vector<seed_point> seed_points;
    };






    class brep_2d_geometry_generator : public base_brep_geometry_generator
    {
    public:

      typedef viennagrid::brep_2d_mesh MeshType;

      brep_2d_geometry_generator() {}

      void read_geometry(pugi::xml_node const & node);
      void evaluate_geometry(output_parameter_interface & opi_mesh,
                             output_parameter_interface & opi_hole_points,
                             output_parameter_interface & opi_seed_points);

    private:

      void clear_impl()
      {
        base_brep_geometry_generator::clear_impl();
        line_strips.clear();
      }

      std::vector<line_strip> line_strips;
    };




    class brep_3d_geometry_generator : public base_brep_geometry_generator
    {
    public:

      typedef viennagrid::brep_3d_mesh MeshType;

      brep_3d_geometry_generator() {}

      void read_geometry(pugi::xml_node const & node);
      void evaluate_geometry(output_parameter_interface & opi_mesh,
                             output_parameter_interface & opi_hole_points,
                             output_parameter_interface & opi_seed_points);

    private:

      void clear_impl()
      {
        base_brep_geometry_generator::clear_impl();
        plcs.clear();
      }

      std::vector<plc> plcs;
    };

  }
}

#endif
