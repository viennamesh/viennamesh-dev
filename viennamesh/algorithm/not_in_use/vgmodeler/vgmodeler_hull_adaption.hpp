#ifndef VIENNAMESH_ALGORITHM_VGMODELER_HULL_ADAPTION_HPP
#define VIENNAMESH_ALGORITHM_VGMODELER_HULL_ADAPTION_HPP

#include "viennamesh/core/dynamic_algorithm.hpp"
#include "vgmodeler/vgmodeler.hpp"


namespace viennamesh
{
  struct vgmodeler_hull_adaption_tag {};

  namespace result_of
  {
    template<>
    struct works_in_place<vgmodeler_hull_adaption_tag>
    {
      static const bool value = false;
    };

    template<>
    struct algorithm_info<vgmodeler_hull_adaption_tag>
    {
      static const std::string name() { return "VGModeler/Netgen Triangle Hull Adaption"; }
    };

    template<typename mesh_type>
    struct best_matching_native_input_mesh<vgmodeler_hull_adaption_tag, mesh_type>
    {
      typedef viennagrid::triangular_3d_mesh type;
    };

    template<typename mesh_type>
    struct best_matching_native_output_mesh<vgmodeler_hull_adaption_tag, mesh_type>
    {
      typedef viennagrid::triangular_3d_mesh type;
    };

    template<typename segmentation_type>
    struct best_matching_native_input_segmentation<vgmodeler_hull_adaption_tag, segmentation_type>
    {
      typedef viennagrid::triangular_hull_3d_segmentation type;
    };

    template<typename segmentation_type>
    struct best_matching_native_output_segmentation<vgmodeler_hull_adaption_tag, segmentation_type>
    {
      typedef viennagrid::triangular_hull_3d_segmentation type;
    };
  }


  template<>
  struct native_algorithm_impl<vgmodeler_hull_adaption_tag>
  {
    typedef vgmodeler_hull_adaption_tag algorithm_tag;

    template<typename native_input_mesh_type, typename input_segmentation_type, typename native_output_mesh_type, typename output_segmentation_type>
    static algorithm_feedback run( native_input_mesh_type const & native_input_mesh,
                                   input_segmentation_type const & input_segmentation,
                                   native_output_mesh_type & native_output_mesh,
                                   output_segmentation_type & output_segmentation,
                                   ConstParameterSet const & parameters )
    {
      algorithm_feedback feedback( result_of::algorithm_info<algorithm_tag>::name() );
      vgmodeler::hull_adaptor adaptor;

      parameters.copy_if_present( "cell_size", adaptor.maxsize() );
//       parameters.copyScalar("cell_size", adaptor.maxsize());

      adaptor.process( native_input_mesh, input_segmentation, native_output_mesh, output_segmentation );

      feedback.set_success();
      return feedback;
    }

  };

}

#endif
