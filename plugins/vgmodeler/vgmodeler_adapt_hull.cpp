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


#include "vgmodeler_adapt_hull.hpp"
#include "vgmodeler.hpp"

namespace viennamesh
{
  namespace vgmodeler
  {

    adapt_hull::adapt_hull() {}

    std::string adapt_hull::name() { return "vgmodeler_adapt_hull"; }

    bool adapt_hull::run(viennamesh::algorithm_handle &)
    {
      typedef viennagrid::mesh_t MeshType;

      mesh_handle input_mesh = get_input<MeshType>("mesh");
      data_handle<double> cell_size = get_input<double>("cell_size");
      data_handle<double> max_angle = get_input<double>("max_angle");

      mesh_handle output_mesh = make_data<MeshType>();

      ::vgmodeler::hull_adaptor adaptor;
      if (cell_size.valid())
        adaptor.maxsize() = cell_size();

      if (max_angle.valid())
        adaptor.maxangle() = max_angle();

      adaptor.process( input_mesh(), output_mesh() );

      set_output("mesh", output_mesh);

      return true;
    }

  }
}
