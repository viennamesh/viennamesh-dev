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

#include "viennamesh/algorithm/remove_duplicate_cells.hpp"
#include "viennamesh/algorithm/healing/remove_duplicate_cells.hpp"

namespace viennamesh
{
  remove_duplicate_cells::remove_duplicate_cells() :
    input_mesh(*this, parameter_information("mesh","mesh","The input mesh, segmented triangular 2d mesh, segmented triangular 3d mesh, segmented quadrilateral 2d mesh, segmented quadrilateral 3d mesh, segmented tetrahedral 3d mesh and segmented hexahedral 3d mesh supported")),
    output_mesh(*this, parameter_information("mesh", "mesh", "The output mesh, same type of mesh as input mesh")) {}

  std::string remove_duplicate_cells::name() const { return "ViennaGrid Remove Duplicate Cells"; }
  std::string remove_duplicate_cells::id() const { return "remove_duplicate_cells"; }



  template<typename MeshT, typename SegmentationT>
  bool remove_duplicate_cells::generic_run()
  {
    typedef viennagrid::segmented_mesh<MeshT, SegmentationT> SegmentedMeshType;

    {
      typename viennamesh::result_of::const_parameter_handle<SegmentedMeshType>::type imp = input_mesh.get<SegmentedMeshType>();

      if (imp)
      {
        output_parameter_proxy<SegmentedMeshType> omp(output_mesh);

        remove_duplicate_cells_heal_functor()( imp(), omp() );

        info(1) << "Cells count before removing degenerate elements: " << viennagrid::cells(imp().mesh).size() << std::endl;
        info(1) << "Cells count after removing degenerate elements: " << viennagrid::cells(omp().mesh).size() << std::endl;

        return true;
      }
    }


    {
      typename viennamesh::result_of::const_parameter_handle<MeshT>::type imp = input_mesh.get<MeshT>();

      if (imp)
      {
        output_parameter_proxy<MeshT> omp(output_mesh);

        remove_duplicate_cells_heal_functor()( imp(), omp() );

        info(1) << "Cells count before removing degenerate elements: " << viennagrid::cells(imp()).size() << std::endl;
        info(1) << "Cells count after removing degenerate elements: " << viennagrid::cells(omp()).size() << std::endl;

        return true;
      }
    }

    return false;
  }

  bool remove_duplicate_cells::run_impl()
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
