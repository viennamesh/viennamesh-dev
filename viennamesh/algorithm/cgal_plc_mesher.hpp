#ifndef VIENNAMESH_ALGORITHM_CGAL_PLC_MESHER_HPP
#define VIENNAMESH_ALGORITHM_CGAL_PLC_MESHER_HPP

#include "viennamesh/core/algorithm.hpp"
#include "viennamesh/core/dynamic_algorithm.hpp"
#include "viennamesh/mesh/cgal_plc.hpp"


#include <CGAL/Triangulation_conformer_2.h>

namespace viennamesh
{
  template<unsigned int geometric_dimension>
  struct cgal_plc_mesher_tag {};

  typedef cgal_plc_mesher_tag<2> cgal_plc_2d_mesher_tag;
  typedef cgal_plc_mesher_tag<3> cgal_plc_3d_mesher_tag;


  namespace result_of
  {
    template<unsigned int geometric_dimension>
    struct works_in_place< cgal_plc_mesher_tag<geometric_dimension> >
    {
      static const bool value = true;
    };

    template<>
    struct algorithm_info< cgal_plc_mesher_tag<2> >
    {
      static const std::string name() { return "CGAL PLC 2D to Triangle Mesh mesher"; }
    };

    template<>
    struct algorithm_info< cgal_plc_mesher_tag<3> >
    {
      static const std::string name() { return "CGAL PLC 3D to Triangle Mesh mesher"; }
    };



    template<typename mesh_type>
    struct best_matching_native_input_mesh<cgal_plc_2d_mesher_tag, mesh_type>
    {
      typedef cgal_plc_2d_mesh type;
    };

    template<typename mesh_type>
    struct best_matching_native_input_mesh<cgal_plc_3d_mesher_tag, mesh_type>
    {
      typedef cgal_plc_3d_mesh type;
    };



    template<typename mesh_type>
    struct best_matching_native_output_mesh<cgal_plc_2d_mesher_tag, mesh_type>
    {
      typedef cgal_plc_2d_mesh type;
    };

    template<typename mesh_type>
    struct best_matching_native_output_mesh<cgal_plc_3d_mesher_tag, mesh_type>
    {
      typedef cgal_plc_3d_mesh type;
    };



    template<unsigned int geometric_dimension, typename segmentation_type>
    struct best_matching_native_input_segmentation<cgal_plc_mesher_tag<geometric_dimension>, segmentation_type>
    {
      typedef NoSegmentation type;
    };

    template<unsigned int geometric_dimension, typename segmentation_type>
    struct best_matching_native_output_segmentation<cgal_plc_mesher_tag<geometric_dimension>, segmentation_type>
    {
      typedef NoSegmentation type;
    };
  }


  template<>
  struct native_algorithm_impl<cgal_plc_2d_mesher_tag>
  {
    typedef cgal_plc_2d_mesher_tag algorithm_tag;

    template<typename native_mesh_type, typename settings_type>
    static algorithm_feedback run( native_mesh_type & native_mesh, settings_type const & settings )
    {
      algorithm_feedback feedback( result_of::algorithm_info<algorithm_tag>::name() );
      typedef cgal_plc_2d_mesh cgal_mesh_type;

      for (cgal_mesh_type::cell_container::iterator it = native_mesh.cells.begin(); it != native_mesh.cells.end(); ++it)
      {
        double shortes_edge_circumradius_ratio = 0.0;
        settings.copyScalar("shortes_edge_circumradius_ratio", shortes_edge_circumradius_ratio);

        double size_bound = 0.0;
        settings.copyScalar("size_bound", size_bound);

        cgal_plc_3d_element::Criteria crit(shortes_edge_circumradius_ratio, size_bound);
        cgal_plc_3d_element::Mesher m(it->cdt,crit);
        m.set_seeds( it->cgal_list_of_holes.begin(), it->cgal_list_of_holes.end(), false );
        m.init();

//                 CGAL::refine_Delaunay_mesh_2(it->cdt,
//                                              it->cgal_list_of_holes.begin(),
//                                              it->cgal_list_of_holes.end(),
//                                              cgal_plc_2d_element::Criteria(
//                                                 settings.shortes_edge_circumradius_ratio(),
//                                                 settings.size_bound()
//                                             ));
      }

      feedback.set_success();
      return feedback;
    }

  };

  template<>
  struct native_algorithm_impl<cgal_plc_3d_mesher_tag>
  {
    typedef cgal_plc_3d_mesher_tag algorithm_tag;

    template<typename native_mesh_type>
    static algorithm_feedback run( native_mesh_type & native_mesh, ParameterSet const & parameters )
    {
      algorithm_feedback feedback( result_of::algorithm_info<algorithm_tag>::name() );
      typedef cgal_plc_3d_mesh cgal_mesh_type;

      for (cgal_mesh_type::cell_container::iterator it = native_mesh.cells.begin(); it != native_mesh.cells.end(); ++it)
      {
        double shortes_edge_circumradius_ratio = 0.0;
        parameters.copyScalar("shortes_edge_circumradius_ratio", shortes_edge_circumradius_ratio);

        double size_bound = 0.0;
        parameters.copyScalar("size_bound", size_bound);

        cgal_plc_3d_element::Criteria crit(shortes_edge_circumradius_ratio, size_bound);
        cgal_plc_3d_element::Mesher m(it->cdt,crit);
        m.set_seeds( it->cgal_list_of_holes.begin(), it->cgal_list_of_holes.end(), false );
        m.init();



//                 CGAL::refine_Delaunay_mesh_2(it->cdt,
//                                              it->cgal_list_of_holes.begin(),
//                                              it->cgal_list_of_holes.end(),
//                                              cgal_plc_3d_element::Criteria(
//                                                 settings.shortes_edge_circumradius_ratio(),
//                                                 settings.size_bound()
//                                             ));
      }

      feedback.set_success();
      return feedback;
    }

  };

}

#endif
