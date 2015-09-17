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

#include "scale_quantities.hpp"
#include "viennagrid/algorithm/geometric_transform.hpp"

namespace viennamesh
{
  scale_quantities::scale_quantities() {}
  std::string scale_quantities::name() { return "scale_quantities"; }

  bool scale_quantities::run(viennamesh::algorithm_handle &)
  {
    mesh_handle input_mesh = get_required_input<mesh_handle>("mesh");
    quantity_field_handle input_quantity_field = get_input<viennagrid::quantity_field>("quantities");

    data_handle<double> scale = get_required_input<double>("scale");
    data_handle<double> translate = get_input<double>("translate");


    typedef viennagrid::mesh MeshType;
    typedef viennagrid::result_of::const_element_range<MeshType>::type ConstElementRangeType;
    typedef viennagrid::result_of::iterator<ConstElementRangeType>::type ConstElementIteratorType;

    quantity_field_handle output_quantity_fields = make_data<viennagrid::quantity_field>();
    output_quantity_fields.resize( input_quantity_field.size() );

    for (viennagrid_int i = 0; i != input_quantity_field.size(); ++i)
    {
      output_quantity_fields(i).init( input_quantity_field(i).topologic_dimension(),
                                      input_quantity_field(i).values_per_quantity() );
      output_quantity_fields(i).set_name( input_quantity_field(i).get_name() );

      ConstElementRangeType cells( input_mesh(), input_quantity_field(i).topologic_dimension() );
      for (ConstElementIteratorType cit = cells.begin(); cit != cells.end(); ++cit)
      {
        viennagrid_numeric value = input_quantity_field(i).get(*cit);
        output_quantity_fields(i).set( *cit, value * scale() + translate() );
      }
    }

    set_output( "quantities", output_quantity_fields );

    return true;
  }

}
