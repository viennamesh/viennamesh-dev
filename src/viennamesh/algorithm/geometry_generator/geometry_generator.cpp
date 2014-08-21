#ifdef VIENNAMESH_WITH_GEOMETRY_GENERATOR

#include "viennamesh/algorithm/geometry_generator/geometry_generator.hpp"
#include "viennamesh/algorithm/geometry_generator/base_geometry_generator.hpp"

#include "viennagrid/mesh/element_creation.hpp"

#include "viennamesh/utils/string_tools.hpp"
using stringtools::lexical_cast;

namespace viennamesh
{

  geometry_generator::geometry_generator() :
    base_algorithm(true),
    filename(*this, parameter_information("filename","string","The filename of the input template")),
    output_mesh(*this, parameter_information("mesh", "mesh", "The output mesh, same type of mesh as input mesh")),
    output_hole_points(*this, parameter_information("hole_points","point_1d_container|point_2d_container|point_3d_container","The read hole points")),
    output_seed_points(*this, parameter_information("seed_points","seed_point_1d_container|seed_point_2d_container|seed_point_3d_container","The read seed points")) {}


  std::string geometry_generator::name() const { return "ViennaMesh Templated Based Geometry Generator"; }
  std::string geometry_generator::id() const { return "geometry_generator"; }


  bool geometry_generator::run_impl()
  {
    pugi::xml_document xml_template;
    pugi::xml_parse_result result = xml_template.load_file( filename().c_str() );


    boost::shared_ptr<base_geometry_generator> generator = base_geometry_generator::make(xml_template.first_child());
    if (!generator)
      return false;

    generator->set_parameters(*this);
    generator->evaluate(output_mesh, output_hole_points, output_seed_points);

    return true;
  }

}

#endif
