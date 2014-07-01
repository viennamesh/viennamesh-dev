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

#include "viennamesh/algorithm/remove_degenerate_cells.hpp"

namespace viennamesh
{

  struct remove_degenerate_cells_copy_functor
  {
    template<typename CellT>
    bool operator()(CellT const & cell) const
    {
      typedef typename viennagrid::result_of::const_vertex_range<CellT>::type ConstVertexRangeType;
      typedef typename viennagrid::result_of::iterator<ConstVertexRangeType>::type ConstVertexRangeIterator;

      ConstVertexRangeType vertices(cell);
      for (ConstVertexRangeIterator vit1 = vertices.begin(); vit1 != vertices.end(); ++vit1)
      {
        ConstVertexRangeIterator vit2 = vit1; ++vit2;
        for (; vit2 != vertices.end(); ++vit2)
        {
          if ( (*vit1).id() == (*vit2).id() )
            return false;
        }
      }

      return true;
    }
  };



  remove_degenerate_cells::remove_degenerate_cells() :
    input_mesh(*this, parameter_information("mesh","mesh","The input mesh, segmented triangular 2d mesh, segmented triangular 3d mesh, segmented quadrilateral 2d mesh, segmented quadrilateral 3d mesh, segmented tetrahedral 3d mesh and segmented hexahedral 3d mesh supported")),
    output_mesh(*this, parameter_information("mesh", "mesh", "The output mesh, same type of mesh as input mesh")) {}

  std::string remove_degenerate_cells::name() const { return "ViennaGrid Remove Degenerated Cells"; }
  std::string remove_degenerate_cells::id() const { return "remove_degenerate_cells"; }



  template<typename MeshT, typename SegmentationT>
  bool remove_degenerate_cells::generic_run()
  {
    typedef viennagrid::segmented_mesh<MeshT, SegmentationT> SegmentedMeshType;

    {
      typename viennamesh::result_of::const_parameter_handle<SegmentedMeshType>::type imp = input_mesh.get<SegmentedMeshType>();

      if (imp)
      {
        output_parameter_proxy<SegmentedMeshType> omp(output_mesh);

        viennagrid::copy( imp().mesh, imp().segmentation, omp().mesh, omp().segmentation, remove_degenerate_cells_copy_functor() );

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

        viennagrid::copy( imp(), omp(), remove_degenerate_cells_copy_functor() );

        info(1) << "Cells count before removing degenerate elements: " << viennagrid::cells(imp()).size() << std::endl;
        info(1) << "Cells count after removing degenerate elements: " << viennagrid::cells(omp()).size() << std::endl;

        return true;
      }
    }




//     if (imp)
//     {
//       output_parameter_proxy<SegmentedMeshType> omp(output_mesh);
//
//
//
// //       map_segments_impl( imp().mesh, imp().segmentation, omp().mesh, omp().segmentation, segment_mapping );
//
//       return true;
//     }

    return false;
  }

  bool remove_degenerate_cells::run_impl()
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
