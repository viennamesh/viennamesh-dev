#ifndef VIENNAMESH_ALGORITHM_HEALING_REMOVE_DUPLICATES_CELLS_HPP
#define VIENNAMESH_ALGORITHM_HEALING_REMOVE_DUPLICATES_CELLS_HPP

#include "viennagrid/mesh/mesh_operations.hpp"

namespace viennamesh
{
  template<typename MeshT>
  struct remove_duplicte_cells_copy_functor
  {
    typedef typename viennagrid::result_of::cell<MeshT>::type CellType;

    remove_duplicte_cells_copy_functor(MeshT const & mesh) : copy_flags(copy_flags_container)
    {
      typedef typename viennagrid::result_of::const_cell_range<MeshT>::type ConstCellRangeType;
      typedef typename viennagrid::result_of::iterator<ConstCellRangeType>::type ConstCellRangeIterator;

      ConstCellRangeType cells(mesh);
      copy_flags_container.resize( cells.size() );

      typedef viennagrid::element_key<CellType> CellKey;
      typedef std::set<CellKey> CellSetType;
      CellSetType cell_set;

      for (ConstCellRangeIterator cit = cells.begin(); cit != cells.end(); ++cit)
      {
        typename CellSetType::const_iterator it = cell_set.find( CellKey(*cit) );
        if (it == cell_set.end())
        {
          copy_flags(*cit) = true;
          cell_set.insert( CellKey(*cit) );
        }
        else
        {
          copy_flags(*cit) = false;
        }
      }
    }

    template<typename CellT>
    bool operator()(CellT const & cell) const
    {
      return copy_flags(cell);
    }

    typedef std::vector<bool> FlagContainerType;
    typedef typename viennagrid::result_of::accessor<FlagContainerType, CellType>::type FlagAccessorType;
    FlagContainerType copy_flags_container;
    FlagAccessorType copy_flags;
  };


  struct remove_duplicate_cells_heal_functor
  {
    template<typename MeshT>
    std::size_t operator()(MeshT const & mesh) const
    {
      typedef typename viennagrid::result_of::const_cell_range<MeshT>::type ConstCellRangeType;
      typedef typename viennagrid::result_of::iterator<ConstCellRangeType>::type ConstCellRangeIterator;

      remove_duplicte_cells_copy_functor<MeshT> f(mesh);
      std::size_t bad_cells_count = 0;
      ConstCellRangeType cells(mesh);
      for (ConstCellRangeIterator cit = cells.begin(); cit != cells.end(); ++cit)
      {
        if ( !f(*cit) )
          return false;
      }

      return true;
    }

    template<typename MeshT>
    std::size_t operator()(MeshT const & input_mesh, MeshT & output_mesh) const
    {
      viennagrid::copy( input_mesh, output_mesh, remove_duplicte_cells_copy_functor<MeshT>(input_mesh) );
      return viennagrid::cells(input_mesh).size() - viennagrid::cells(output_mesh).size();
    }

    template<typename MeshT, typename SegmentationT>
    std::size_t operator()(viennagrid::segmented_mesh<MeshT, SegmentationT> const & input_mesh,
                    viennagrid::segmented_mesh<MeshT, SegmentationT> & output_mesh) const
    {
      viennagrid::copy( input_mesh.mesh, input_mesh.segmentation,
                        output_mesh.mesh, output_mesh.segmentation,
                        remove_duplicte_cells_copy_functor<MeshT>(input_mesh.mesh) );
      return viennagrid::cells(input_mesh.mesh).size() - viennagrid::cells(output_mesh.mesh).size();
    }
  };

}

#endif
