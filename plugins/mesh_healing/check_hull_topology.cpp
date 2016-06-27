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

#include "check_hull_topology.hpp"

namespace viennamesh
{
  check_hull_topology::check_hull_topology() {}
  std::string check_hull_topology::name() { return "check_hull_topology"; }

  bool check_hull_topology::run(viennamesh::algorithm_handle &)
  {
    mesh_handle input_mesh = get_required_input<mesh_handle>("mesh");
    mesh_handle output_mesh = make_data<mesh_handle>();

    typedef viennagrid::mesh                                                            MeshType;

    typedef viennagrid::result_of::const_element_range<MeshType>::type                  ConstElementRangeType;
    typedef viennagrid::result_of::iterator<ConstElementRangeType>::type                ConstElementIteratorType;

    typedef viennagrid::result_of::const_coboundary_range<MeshType>::type               ConstCoboundaryElementRangeType;
    typedef viennagrid::result_of::iterator<ConstCoboundaryElementRangeType>::type      ConstCoboundaryElementIteratorType;

    viennagrid::result_of::element_copy_map<>::type copy_map(output_mesh(), true);


    ConstElementRangeType lines( input_mesh(), 1 );
    for (ConstElementIteratorType lit = lines.begin(); lit != lines.end(); ++lit)
    {
      ConstCoboundaryElementRangeType coboundary_triangles( input_mesh(), *lit, 2 );
      if (coboundary_triangles.size() < 2)
      {
        std::cout << "Line " << *lit << " has less than 2 co-boundary triangles" << std::endl;

        for (ConstCoboundaryElementIteratorType ctit = coboundary_triangles.begin(); ctit != coboundary_triangles.end(); ++ctit)
        {
          copy_map(*ctit);
        }
      }

    }

    set_output( "mesh", output_mesh );

    return true;
  }

}
