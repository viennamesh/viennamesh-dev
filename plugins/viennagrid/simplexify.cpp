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

#include "simplexify.hpp"
#include "viennagrid/algorithm/refine.hpp"

namespace viennamesh
{


  template<bool element_is_const, typename CopyMapT>
  void simplexify_impl(viennagrid::base_element<element_is_const> const & element,
                  viennagrid::quadrilateral_tag,
                  CopyMapT & copy_map)
  {
    typedef viennagrid::base_element<element_is_const> ElementType;

    ElementType vertices[4];
    for (int i = 0; i != 4; ++i)
      vertices[i] = copy_map( viennagrid::vertices(element)[i] );

    ElementType t0 = viennagrid::make_triangle( copy_map.dst_mesh(), vertices[0], vertices[1], vertices[2] );
    ElementType t1 = viennagrid::make_triangle( copy_map.dst_mesh(), vertices[1], vertices[2], vertices[3] );

    assert( viennagrid::detail::is_equal(1e-6,
                                         viennagrid::volume(element),
                                         viennagrid::volume(t0)+viennagrid::volume(t1)) );
  }


  template<bool element_is_const, typename CopyMapT>
  void simplexify_impl(viennagrid::base_element<element_is_const> const & element,
                  CopyMapT & copy_map)
  {
    if (element.tag().is_quadrilateral())
      simplexify_impl(element, viennagrid::quadrilateral_tag(), copy_map);
    else
    {
      assert(false);
    }
  }



  void simplexify_impl(viennagrid::mesh_t const & src_mesh, viennagrid::mesh_t const & dst_mesh)
  {
    typedef viennagrid::mesh_t MeshType;
    typedef typename viennagrid::result_of::const_cell_range<MeshType>::type ConstCellRangeType;
    typedef typename viennagrid::result_of::iterator<ConstCellRangeType>::type ConstCellIterator;

    typedef typename viennagrid::result_of::element_copy_map<>::type CopyMapType;

    CopyMapType copy_map(dst_mesh);
    ConstCellRangeType cells(src_mesh);
    for (ConstCellIterator cit = cells.begin(); cit != cells.end(); ++cit)
    {
      if ( (*cit).tag().is_simplex() )
        copy_map( *cit );
      else
        simplexify_impl( *cit, copy_map );
    }
  }




  simplexify::simplexify() {}
  std::string simplexify::name() { return "simplexify"; }

  bool simplexify::run(viennamesh::algorithm_handle &)
  {
    mesh_handle input_mesh = get_required_input<mesh_handle>("mesh");
    mesh_handle output_mesh = make_data<mesh_handle>();

    simplexify_impl( input_mesh(), output_mesh() );

    set_output( "mesh", output_mesh );

    return true;
  }


}
