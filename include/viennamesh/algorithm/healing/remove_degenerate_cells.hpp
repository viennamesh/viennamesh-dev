#ifndef VIENNAMESH_ALGORITHM_HEALING_REMOVE_DEGENERATE_CELLS_HPP
#define VIENNAMESH_ALGORITHM_HEALING_REMOVE_DEGENERATE_CELLS_HPP

#include "viennagrid/mesh/mesh_operations.hpp"

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


  struct remove_degenerate_cells_heal_functor
  {
    template<typename MeshT>
    bool operator()(MeshT const & input_mesh, MeshT & output_mesh) const
    {
      viennagrid::copy( input_mesh, output_mesh, remove_degenerate_cells_copy_functor() );

      info(1) << "Removed " << viennagrid::cells(input_mesh).size()-viennagrid::cells(output_mesh).size() << " degenerate cells" << std::endl;
      return true;
    }

    template<typename MeshT, typename SegmentationT>
    bool operator()(viennagrid::segmented_mesh<MeshT, SegmentationT> const & input_mesh,
                    viennagrid::segmented_mesh<MeshT, SegmentationT> & output_mesh) const
    {
      viennagrid::copy( input_mesh.mesh, input_mesh.segmentation,
                        output_mesh.mesh, output_mesh.segmentation,
                        remove_degenerate_cells_copy_functor() );

      info(1) << "Removed " << viennagrid::cells(input_mesh.mesh).size()-viennagrid::cells(output_mesh.mesh).size() << " degenerate cells" << std::endl;
      return true;
    }
  };

}

#endif
