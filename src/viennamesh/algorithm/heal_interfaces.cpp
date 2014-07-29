#include "viennamesh/algorithm/heal_interfaces.hpp"

#include "viennamesh/algorithm/healing/healing.hpp"
#include "viennamesh/algorithm/healing/heal_line_line_intersections.hpp"
#include "viennamesh/algorithm/healing/heal_point_line_intersections.hpp"



namespace viennamesh
{
  heal_interfaces::heal_interfaces() :
    input_mesh(*this, parameter_information("mesh","mesh","The input mesh, segmented triangular 3d mesh supported")),
    tolerance(*this, parameter_information("tolerance","double","The tolerance"), 1e-6),
    output_mesh(*this, parameter_information("mesh", "mesh", "The output mesh, plc 3d mesh")) {}


  std::string heal_interfaces::name() const { return "ViennaMesh Interface Healer"; }
  std::string heal_interfaces::id() const { return "heal_interfaces"; }


  bool heal_interfaces::run_impl()
  {
    typedef viennagrid::triangular_3d_mesh InputMeshType;
    typedef viennagrid::triangular_3d_mesh OutputMeshType;

    viennamesh::result_of::const_parameter_handle<InputMeshType>::type imp = input_mesh.get<InputMeshType>();

    if (imp)
    {
      output_parameter_proxy<OutputMeshType> omp(output_mesh);

      iteratively_heal( imp(), omp(), point_line_intersection_heal_functor<double>( tolerance() ) );
      iteratively_heal( imp(), omp(), line_line_intersection_heal_functor<double>( tolerance() ) );

      return true;
    }

    error(1) << "Input Parameter 'default' (type: mesh) is missing or of non-convertable type" << std::endl;
    return false;
  }


}
