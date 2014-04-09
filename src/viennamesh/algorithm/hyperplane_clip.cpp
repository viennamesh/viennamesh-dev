#include "viennamesh/algorithm/hyperplane_clip.hpp"

#include "viennagrid/algorithm/hyperplane_refine.hpp"

namespace viennamesh
{
  template<typename PointT, typename NumericConfigT>
  bool on_positive_hyperplane_side( PointT const & hyperplane_point, PointT const & hyperplane_normal,
                                    PointT const & to_test,
                                    NumericConfigT numeric_config)
  {
    return viennagrid::inner_prod( hyperplane_normal, to_test-hyperplane_point ) >
      viennagrid::detail::relative_tolerance(numeric_config, viennagrid::norm_2(to_test-hyperplane_point));
  }

  template<typename PointT, typename NumericConfigT>
  struct on_positive_hyperplane_side_functor
  {
    typedef bool result_type;
    typedef typename viennagrid::result_of::coord<PointT>::type ScalarType;

    on_positive_hyperplane_side_functor(PointT const & hyperplane_point_, PointT const & hyperplane_normal_,
                                        NumericConfigT numeric_config_) :
        hyperplane_point(hyperplane_point_),
        hyperplane_normal(hyperplane_normal_),
        numeric_config(numeric_config_) {}

    template<typename ElementT>
    bool operator()(ElementT const & element) const
    {
      PointT centroid = viennagrid::centroid(element);
      return on_positive_hyperplane_side(hyperplane_point, hyperplane_normal, centroid, numeric_config);
    }

    PointT hyperplane_point;
    PointT hyperplane_normal;
    NumericConfigT numeric_config;
  };



  template<typename MeshT, typename SegmentationT>
  bool hyperplane_clip::generic_run( dynamic_point const & base_hyperplane_point, dynamic_point const & base_hyperplane_normal )
  {
    typedef viennagrid::segmented_mesh<MeshT, SegmentationT> SegmentedMeshType;

    const int geometric_dimension = viennagrid::result_of::geometric_dimension<MeshT>::value;
    typedef typename viennamesh::result_of::point<geometric_dimension>::type PointType;

    PointType hyperplane_point;
    PointType hyperplane_normal;

    std::copy( base_hyperplane_point.begin(), base_hyperplane_point.end(), hyperplane_point.begin() );
    std::copy( base_hyperplane_normal.begin(), base_hyperplane_normal.end(), hyperplane_normal.begin() );

    {
      typename viennamesh::result_of::const_parameter_handle<SegmentedMeshType>::type input_mesh = get_input<SegmentedMeshType>("default");
      if (input_mesh)
      {
        output_parameter_proxy<SegmentedMeshType> output_mesh = output_proxy<SegmentedMeshType>( "default" );
        SegmentedMeshType tmp;

        viennagrid::hyperplane_refine(input_mesh().mesh, input_mesh().segmentation,
                        tmp.mesh, tmp.segmentation,
                        hyperplane_point, hyperplane_normal, 1e-8 );

        viennagrid::copy( tmp.mesh, tmp.segmentation,
                          output_mesh().mesh, output_mesh().segmentation,
                          on_positive_hyperplane_side_functor<PointType, double>(hyperplane_point, -hyperplane_normal, 1e-8) );

        return true;
      }
    }

    {
      typename viennamesh::result_of::const_parameter_handle<MeshT>::type input_mesh = get_input<MeshT>("default");
      if (input_mesh)
      {
        output_parameter_proxy<MeshT> output_mesh = output_proxy<MeshT>( "default" );
        MeshT tmp;

        viennagrid::hyperplane_refine(input_mesh(), tmp, hyperplane_point, hyperplane_normal, 1e-8 );
        viennagrid::copy( tmp, output_mesh(),
                          on_positive_hyperplane_side_functor<PointType, double>(hyperplane_point, -hyperplane_normal, 1e-8) );

        return true;
      }
    }

    return false;
  }

  bool hyperplane_clip::run_impl()
  {
    viennamesh::const_parameter_handle mesh = get_input("default");
    if (!mesh)
    {
      error(1) << "Input Parameter 'default' (type: mesh) is missing" << std::endl;
      return false;
    }

    viennamesh::result_of::const_parameter_handle<dynamic_point>::type base_hyperplane_point = get_required_input<dynamic_point>("hyperplane_point");
    viennamesh::result_of::const_parameter_handle<dynamic_point>::type base_hyperplane_normal = get_required_input<dynamic_point>("hyperplane_normal");


    unsigned int mesh_geometric_dimension = lexical_cast<unsigned int>( mesh->get_property("geometric_dimension").first );

    if (mesh_geometric_dimension != base_hyperplane_point().size())
    {
      error(1) << "Dimension missmatch, mesh has geometric dimension " << mesh_geometric_dimension << " but hyperplane point has dimension " << base_hyperplane_point().size() << std::endl;
      return false;
    }

    if (mesh_geometric_dimension != base_hyperplane_normal().size())
    {
      error(1) << "Dimension missmatch, mesh has geometric dimension " << mesh_geometric_dimension << " but hyperplane normal has dimension " << base_hyperplane_normal().size() << std::endl;
      return false;
    }


    if (generic_run<viennagrid::triangular_2d_mesh, viennagrid::triangular_2d_segmentation>(base_hyperplane_point(), base_hyperplane_normal()))
      return true;

    if (generic_run<viennagrid::triangular_3d_mesh, viennagrid::triangular_3d_segmentation>(base_hyperplane_point(), base_hyperplane_normal()))
      return true;

    if (generic_run<viennagrid::tetrahedral_3d_mesh, viennagrid::tetrahedral_3d_segmentation>(base_hyperplane_point(), base_hyperplane_normal()))
      return true;


    error(1) << "Input Parameter 'default' (type: mesh) is missing or of non-convertable type" << std::endl;
    return false;
  }


}
