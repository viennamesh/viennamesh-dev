#include "viennamesh/algorithm/extract_hull.hpp"

#include "viennagrid/algorithm/extract_hull.hpp"
#include "viennagrid/algorithm/extract_seed_points.hpp"

namespace viennamesh
{
  template<typename MeshT, typename SegmentationT>
  bool extract_hull::generic_run()
  {
    typedef viennagrid::segmented_mesh<MeshT, SegmentationT> SegmentedMeshType;
    typedef typename viennagrid::result_of::facet_tag<MeshT>::type FacetTagType;

    typedef typename viennamesh::result_of::full_config<
            FacetTagType,
            viennagrid::result_of::geometric_dimension<MeshT>::value
        >::type FacetConfigType;
    typedef viennagrid::mesh<FacetConfigType> FacetMeshType;
    typedef typename viennagrid::result_of::segmentation<FacetMeshType>::type FacetSegmentationType;
    typedef viennagrid::segmented_mesh<FacetMeshType, FacetSegmentationType> SegmentedFacetMeshType;

    typedef typename viennamesh::result_of::point< viennagrid::result_of::geometric_dimension<MeshT>::value >::type PointType;
    typedef typename viennamesh::result_of::seed_point_container<PointType>::type SeedPointContainerType;

    {
      typename viennamesh::result_of::const_parameter_handle<SegmentedMeshType>::type input_mesh = get_input<SegmentedMeshType>("default");
      if (input_mesh)
      {
        output_parameter_proxy<SegmentedFacetMeshType> output_mesh = output_proxy<SegmentedFacetMeshType>( "default" );
        output_parameter_proxy<SeedPointContainerType> seed_points = output_proxy<SeedPointContainerType>( "seed_points" );

        viennagrid::extract_hull(input_mesh().mesh, input_mesh().segmentation,
                                                  output_mesh().mesh, output_mesh().segmentation);
        viennagrid::extract_seed_points( input_mesh().mesh, input_mesh().segmentation, seed_points() );

        return true;
      }
    }

    {
      typename viennamesh::result_of::const_parameter_handle<MeshT>::type input_mesh = get_input<MeshT>("default");
      if (input_mesh)
      {
        output_parameter_proxy<FacetMeshType> output_mesh = output_proxy<FacetMeshType>( "default" );

        viennagrid::extract_hull(input_mesh(), output_mesh());

        return true;
      }
    }

    return false;
  }

  bool extract_hull::run_impl()
  {
    if (generic_run<viennagrid::triangular_2d_mesh, viennagrid::triangular_2d_segmentation>())
      return true;
    if (generic_run<viennagrid::triangular_3d_mesh, viennagrid::triangular_3d_segmentation>())
      return true;

    if (generic_run<viennagrid::quadrilateral_2d_mesh, viennagrid::quadrilateral_2d_segmentation>())
      return true;
    if (generic_run<viennagrid::quadrilateral_3d_mesh, viennagrid::quadrilateral_3d_segmentation>())
      return true;

    if (generic_run<viennagrid::tetrahedral_3d_mesh, viennagrid::tetrahedral_3d_segmentation>())
      return true;

    if (generic_run<viennagrid::hexahedral_3d_mesh, viennagrid::hexahedral_3d_segmentation>())
      return true;


    error(1) << "Input Parameter 'default' (type: mesh) is missing or of non-convertable type" << std::endl;
    return false;
  }

}
