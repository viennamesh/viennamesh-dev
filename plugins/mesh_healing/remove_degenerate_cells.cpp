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

#include "remove_degenerate_cells.hpp"

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

  remove_degenerate_cells::remove_degenerate_cells() {}
  std::string remove_degenerate_cells::name() { return "remove_degenerate_cells"; }

  bool remove_degenerate_cells::run(viennamesh::algorithm_handle &)
  {
    mesh_handle input_mesh = get_required_input<mesh_handle>("mesh");
    mesh_handle output_mesh = make_data<mesh_handle>();

    info(1) << "Cells count before removing degenerate cells: " << viennagrid::cells(input_mesh()).size() << std::endl;
    viennagrid::copy( input_mesh(), output_mesh(), remove_degenerate_cells_copy_functor() );
    info(1) << "Cells count after removing degenerate cells: " << viennagrid::cells(output_mesh()).size() << std::endl;

    set_output( "mesh", output_mesh );

    return true;
  }

}
