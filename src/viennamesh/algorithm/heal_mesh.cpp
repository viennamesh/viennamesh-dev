#include "viennamesh/algorithm/heal_mesh.hpp"

#include "viennamesh/algorithm/healing/healing.hpp"


#include "viennamesh/algorithm/healing/heal_line_line_intersections.hpp"
#include "viennamesh/algorithm/healing/heal_point_line_intersections.hpp"
#include "viennamesh/algorithm/healing/remove_degenerate_cells.hpp"
#include "viennamesh/algorithm/healing/remove_duplicate_cells.hpp"
#include "viennamesh/algorithm/healing/remove_duplicate_points.hpp"



namespace viennamesh
{

  template<typename NumericConfigT>
  struct hull_heal_functor
  {
    hull_heal_functor(NumericConfigT nc) : duplicate_points(nc), line_line_intersection(nc), point_line_intersection(nc) {}


    template<typename MeshT>
    bool operator()(MeshT const & input_mesh, MeshT & output_mesh) const
    {
      MeshT tmp;

      bool good = true;
      good |= duplicate_points(input_mesh, output_mesh);
      good |= degenerate_cells(output_mesh, tmp);
      good |= duplicate_cells(tmp, output_mesh);
      good |= line_line_intersection(output_mesh, tmp);
      good |= point_line_intersection(tmp, output_mesh);

      return good;
    }


    remove_duplicate_points_heal_functor<NumericConfigT> duplicate_points;
    remove_degenerate_cells_heal_functor degenerate_cells;
    remove_duplicate_cells_heal_functor duplicate_cells;

    line_line_intersection_heal_functor<NumericConfigT> line_line_intersection;
    point_line_intersection_heal_functor<NumericConfigT> point_line_intersection;
  };




  heal_mesh::heal_mesh() :
    input_mesh(*this, parameter_information("mesh","mesh","The input mesh, segmented triangular 3d mesh supported")),
    tolerance(*this, parameter_information("tolerance","double","The tolerance"), 1e-6),
    max_heal_iteration_count(*this, parameter_information("max_heal_iteration_count","int","Maximum count of healing iterations"), 10),
    output_mesh(*this, parameter_information("mesh", "mesh", "The output mesh, plc 3d mesh")) {}


  std::string heal_mesh::name() const { return "ViennaMesh Mesh Healer"; }
  std::string heal_mesh::id() const { return "heal_mesh"; }


  bool heal_mesh::run_impl()
  {
    typedef viennagrid::triangular_3d_mesh InputMeshType;
    typedef viennagrid::triangular_3d_mesh OutputMeshType;

    viennamesh::result_of::const_parameter_handle<InputMeshType>::type imp = input_mesh.get<InputMeshType>();

    if (imp)
    {
      output_parameter_proxy<OutputMeshType> omp(output_mesh);

      iteratively_heal( imp(), omp(), max_heal_iteration_count(), hull_heal_functor<double>( tolerance() ) );

      return true;
    }

    error(1) << "Input Parameter 'default' (type: mesh) is missing or of non-convertable type" << std::endl;
    return false;
  }


}
