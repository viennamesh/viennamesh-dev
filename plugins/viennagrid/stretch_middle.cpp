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

#include "stretch_middle.hpp"
#include "viennagrid/algorithm/geometric_transform.hpp"

namespace viennamesh
{
  stretch_middle::stretch_middle() {}
  std::string stretch_middle::name() { return "stretch_middle"; }

  bool stretch_middle::run(viennamesh::algorithm_handle &)
  {
    mesh_handle input_mesh = get_required_input<mesh_handle>("mesh");

    data_handle<double> stretch = get_required_input<double>("stretch");
    data_handle<double> middle_tolerance = get_required_input<double>("middle_tolerance");

    mesh_handle output_mesh = make_data<mesh_handle>();

    if (output_mesh != input_mesh)
      viennagrid::copy( input_mesh(), output_mesh() );

    typedef viennagrid::mesh MeshType;
    typedef viennagrid::result_of::point<MeshType>::type PointType;
    typedef viennagrid::result_of::const_element_range<MeshType>::type ConstElementRangeType;
    typedef viennagrid::result_of::iterator<ConstElementRangeType>::type ConstElementIteratorType;

    ConstElementRangeType vertices( output_mesh(), 0 );
    for (ConstElementIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit)
    {
      PointType pt = viennagrid::get_point(*vit);

      if (std::abs(pt[0]) >= middle_tolerance())
      {
        if (pt[0] < 0)
          pt[0] -= stretch();
        else
          pt[0] += stretch();
      }

      viennagrid::set_point(*vit, pt);
    }

    set_output( "mesh", output_mesh );

    return true;
  }

}
