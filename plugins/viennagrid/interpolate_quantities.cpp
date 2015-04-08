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

#include "interpolate_quantities.hpp"
#include "viennagridpp/algorithm/quantity_interpolate.hpp"

namespace viennamesh
{

  interpolate_quantities::interpolate_quantities() {}
  std::string interpolate_quantities::name() { return "interpolate_quantities"; }


  bool interpolate_quantities::run(viennamesh::algorithm_handle &)
  {
//     typedef viennagrid::mesh_t MeshType;

    mesh_handle src_mesh = get_required_input<mesh_handle>("src_mesh");
    mesh_handle dst_mesh = get_required_input<mesh_handle>("dst_mesh");

    quantity_field_handle src_quantity_fields = get_input<viennagrid_quantity_field>("quantities");
    quantity_field_handle dst_quantity_fields = make_data<viennagrid::quantity_field>();
    dst_quantity_fields.resize( src_quantity_fields.size() );


    for (int i = 0; i != src_quantity_fields.size(); ++i)
    {
      viennagrid::quantity_field src_qf = src_quantity_fields(i);
      if (src_qf.topologic_dimension() != 0)
      {
        info(1) << "Quantity field \"" << src_qf.name() << "\" has unsupported topologic dimension = " << src_qf.topologic_dimension() << " -> skipping" << std::endl;
        continue;
      }

      info(1) << "Found quantity field \"" << src_qf.name() << "\" with topologic dimension " << src_qf.topologic_dimension() << std::endl;

//       typedef typename viennagrid::result_of::const_cell_range<MeshType>::type ConstCellRangeType;
//       typedef typename viennagrid::result_of::iterator<ConstCellRangeType>::type ConstCellRangeIterator;

      viennagrid::quantity_field dst_qf( 0, src_qf.storage_layout(), src_qf.values_dimension() );
//       dst_qf.set_topologic_dimension(0);
      dst_qf.set_name( src_qf.name() );

      viennagrid::interpolate_vertex_quantity( src_mesh(), src_qf, dst_mesh(), dst_qf, 0 );

      dst_quantity_fields.set(i, dst_qf);
    }


    set_output( "quantities", dst_quantity_fields );

    return true;
  }


}
