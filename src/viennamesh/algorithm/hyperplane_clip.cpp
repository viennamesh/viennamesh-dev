/* ============================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                ViennaMesh - The Vienna Meshing Framework
                            -----------------

                    http://viennamesh.sourceforge.net/

   License:         MIT (X11), see file LICENSE in the base directory
=============================================================================== */

#include "viennamesh/algorithm/hyperplane_clip.hpp"

#include "viennagrid/algorithm/refine.hpp"

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





  hyperplane_clip::hyperplane_clip() :
    input_mesh(*this, parameter_information("mesh","mesh","The input mesh, segmented and non-segmented triangular 2d, segmented and non-segmented triangular 3d and segmented and non-segmented tetrahedral 3d supported")),
    hyperplane_point(*this, parameter_information("hyperplane_point","dynamic_point","A point of the clipping hyperplane")),
    hyperplane_normal(*this, parameter_information("hyperplane_normal","dynamic_point","The normal vector of the clipping hyperplane")),
    output_mesh(*this, parameter_information("mesh","mesh","The output mesh, same type of mesh as input mesh")) {}

  std::string hyperplane_clip::name() const { return "ViennaGrid Hyperplane Clip"; }
  std::string hyperplane_clip::id() const { return "hyperplane_clip"; }


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
      typename viennamesh::result_of::const_parameter_handle<SegmentedMeshType>::type imp = input_mesh.get<SegmentedMeshType>();
      if (imp)
      {
        output_parameter_proxy<SegmentedMeshType> omp(output_mesh);
        SegmentedMeshType tmp;

        viennagrid::hyperplane_refine(imp().mesh, imp().segmentation,
                        tmp.mesh, tmp.segmentation,
                        hyperplane_point, hyperplane_normal, 1e-8 );

        viennagrid::copy( tmp.mesh, tmp.segmentation,
                          omp().mesh, omp().segmentation,
                          on_positive_hyperplane_side_functor<PointType, double>(hyperplane_point, -hyperplane_normal, 1e-8) );

        return true;
      }
    }

    {
      typename viennamesh::result_of::const_parameter_handle<MeshT>::type imp = input_mesh.get<MeshT>();
      if (imp)
      {
        output_parameter_proxy<MeshT> omp(output_mesh);
        MeshT tmp;

        viennagrid::hyperplane_refine(imp(), tmp, hyperplane_point, hyperplane_normal, 1e-8 );
        viennagrid::copy( tmp, omp(),
                          on_positive_hyperplane_side_functor<PointType, double>(hyperplane_point, -hyperplane_normal, 1e-8) );

        return true;
      }
    }

    return false;
  }

  bool hyperplane_clip::run_impl()
  {
    viennamesh::const_parameter_handle mesh = input_mesh.get();

    unsigned int mesh_geometric_dimension = lexical_cast<unsigned int>( mesh->get_property("geometric_dimension").first );

    if (mesh_geometric_dimension != hyperplane_point().size())
    {
      error(1) << "Dimension missmatch, mesh has geometric dimension " << mesh_geometric_dimension << " but hyperplane point has dimension " << hyperplane_point().size() << std::endl;
      return false;
    }

    if (mesh_geometric_dimension != hyperplane_normal().size())
    {
      error(1) << "Dimension missmatch, mesh has geometric dimension " << mesh_geometric_dimension << " but hyperplane normal has dimension " << hyperplane_normal().size() << std::endl;
      return false;
    }


    if (generic_run<viennagrid::triangular_2d_mesh, viennagrid::triangular_2d_segmentation>(hyperplane_point(), hyperplane_normal()))
      return true;

    if (generic_run<viennagrid::triangular_3d_mesh, viennagrid::triangular_3d_segmentation>(hyperplane_point(), hyperplane_normal()))
      return true;

    if (generic_run<viennagrid::tetrahedral_3d_mesh, viennagrid::tetrahedral_3d_segmentation>(hyperplane_point(), hyperplane_normal()))
      return true;


    error(1) << "Input Parameter 'default' (type: mesh) is missing or of non-convertable type" << std::endl;
    return false;
  }


}
