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
    hull_heal_functor(NumericConfigT nc) : duplicate_points_healer(nc), point_line_intersection_healer(nc), line_line_intersection_healer(nc) {}


    template<typename MeshT>
    bool operator()(MeshT const & mesh)
    {

      if ( !duplicate_points_healer(mesh) )
        return false;

      if ( !degenerate_cells_healer(mesh) )
        return false;

      if ( !duplicate_cells_healer(mesh) )
        return false;

      if ( !point_line_intersection_healer(mesh) )
        return false;

      if ( !line_line_intersection_healer(mesh) )
        return false;

      return true;
    }

    template<typename MeshT>
    std::size_t operator()(MeshT const & input_mesh, MeshT & output_mesh) const
    {
      std::size_t total_bads = 0;

      MeshT tmp;

      viennagrid::copy(input_mesh, tmp);

      MeshT * src = &tmp;
      MeshT * dst = &output_mesh;

      total_bads += apply_topologic(src, dst);

      if ( !line_line_intersection_healer(*src) )
      {
        std::size_t bads = line_line_intersection_healer(*src, *dst);
        std::swap(src, dst);
        info(1) << bads << " line-line intersection bads" << std::endl;
        total_bads += bads;
      }

      total_bads += apply_topologic(src, dst);

      if ( !point_line_intersection_healer(*src) )
      {
        std::size_t bads = point_line_intersection_healer(*src, *dst);
        std::swap(src, dst);
        info(1) << bads << " point-line intersection bads" << std::endl;
        total_bads += bads;
      }

      total_bads += apply_topologic(src, dst);

      if (src != &output_mesh)
        viennagrid::copy(*src, output_mesh);

      return total_bads;
    }


    template<typename MeshT>
    std::size_t apply_topologic( MeshT* & src, MeshT* & dst ) const
    {
      std::size_t total_bads = 0;

      if ( !duplicate_points_healer(*src) )
      {
        std::size_t bads = duplicate_points_healer(*src, *dst);
        std::swap(src, dst);
        info(1) << "Healed " << bads << " duplicate point bads" << std::endl;
        total_bads += bads;
      }

      if ( !degenerate_cells_healer(*src) )
      {
        std::size_t bads = degenerate_cells_healer(*src, *dst);
        std::swap(src, dst);
        info(1) << "Healed " << bads << " degenerate cell bads" << std::endl;
        total_bads += bads;
      }

      if ( !duplicate_cells_healer(*src) )
      {
        std::size_t bads = duplicate_cells_healer(*src, *dst);
        std::swap(src, dst);
        info(1) << "Healed " << bads << " duplicate cell bads" << std::endl;
        total_bads += bads;
      }

      return total_bads;
    }


    remove_duplicate_points_heal_functor<NumericConfigT> duplicate_points_healer;
    remove_degenerate_cells_heal_functor degenerate_cells_healer;
    remove_duplicate_cells_heal_functor duplicate_cells_healer;

    point_line_intersection_heal_functor<NumericConfigT> point_line_intersection_healer;
    line_line_intersection_heal_functor<NumericConfigT> line_line_intersection_healer;

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
