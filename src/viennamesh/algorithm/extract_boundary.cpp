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

#include "viennamesh/algorithm/extract_boundary.hpp"
#include "viennamesh/algorithm/extract_hole_points.hpp"

#include "viennagrid/algorithm/extract_boundary.hpp"
#include "viennagrid/algorithm/extract_seed_points.hpp"



namespace viennamesh
{

  extract_boundary::extract_boundary() :
    input_mesh(*this, parameter_information("mesh","mesh","The input mesh, segmented and non-segmented triangular 2d mesh and segmented and non-segmented tetrahedral 3d mesh supported")),
    output_mesh(*this, parameter_information("mesh", "mesh", "The output mesh, line 2d for triangular 2d input, triangular 3d for tetrahedral 3d input")),
    output_seed_points(*this, parameter_information("seed_points","seed_point_2d_container|seed_point_3d_container","Extracted seed points")),
    output_hole_points(*this, parameter_information("hole_points","point_2d_container|point_3d_container","Extracted hole points")) {}

  std::string extract_boundary::name() const { return "ViennaGrid Extract Boundary"; }
  std::string extract_boundary::id() const { return "extract_boundary"; }

  template<typename MeshT, typename SegmentationT>
  bool extract_boundary::generic_run()
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
    typedef typename viennamesh::result_of::point_container<PointType>::type HolePointContainerType;


    {
      typename viennamesh::result_of::const_parameter_handle<SegmentedMeshType>::type imp = input_mesh.get<SegmentedMeshType>();
      if (imp)
      {
        output_parameter_proxy<SegmentedFacetMeshType> omp(output_mesh);
        output_parameter_proxy<SeedPointContainerType> ospp(output_seed_points);
        output_parameter_proxy<HolePointContainerType> ohpp(output_hole_points);

        viennagrid::extract_boundary(imp().mesh, imp().segmentation, omp().mesh, omp().segmentation);
        viennagrid::extract_seed_points( imp().mesh, imp().segmentation, ospp() );
        viennagrid::extract_hole_points( imp().mesh, ohpp() );

        return true;
      }
    }

    {
      typename viennamesh::result_of::const_parameter_handle<MeshT>::type imp = input_mesh.get<MeshT>();
      if (imp)
      {
        output_parameter_proxy<FacetMeshType> omp(output_mesh);
        output_parameter_proxy<HolePointContainerType> ohpp(output_hole_points);

        viennagrid::extract_boundary(imp(), omp());
        viennagrid::extract_hole_points( imp(), ohpp() );

        return true;
      }
    }

    return false;
  }

  bool extract_boundary::run_impl()
  {
    if (generic_run<viennagrid::triangular_2d_mesh, viennagrid::triangular_2d_segmentation>())
      return true;
//     if (generic_run<viennagrid::triangular_3d_mesh, viennagrid::triangular_3d_segmentation>())
//       return true;
//
//     if (generic_run<viennagrid::quadrilateral_2d_mesh, viennagrid::quadrilateral_2d_segmentation>())
//       return true;
//     if (generic_run<viennagrid::quadrilateral_3d_mesh, viennagrid::quadrilateral_3d_segmentation>())
//       return true;

    if (generic_run<viennagrid::tetrahedral_3d_mesh, viennagrid::tetrahedral_3d_segmentation>())
      return true;

//     if (generic_run<viennagrid::hexahedral_3d_mesh, viennagrid::hexahedral_3d_segmentation>())
//       return true;


    error(1) << "Input Parameter 'default' (type: mesh) is missing or of non-convertable type" << std::endl;
    return false;
  }

}
