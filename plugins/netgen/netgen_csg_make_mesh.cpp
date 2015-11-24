// #ifdef VIENNAMESH_WITH_TETGEN

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

#include "netgen_mesh.hpp"
#include "netgen_csg_make_mesh.hpp"

#include "viennagrid/algorithm/distance.hpp"
#include "viennagrid/algorithm/spanned_volume.hpp"

namespace viennamesh
{
  namespace netgen
  {
    csg_make_mesh::csg_make_mesh() {}

    std::string csg_make_mesh::name() { return "netgen_csg_make_mesh"; }

    bool csg_make_mesh::run(viennamesh::algorithm_handle &)
    {

//       output_parameter_proxy<netgen::mesh> output(output_mesh);

      data_handle<viennamesh_string> csg_source = get_required_input<viennamesh_string>("csg_source");

      data_handle<double> cell_size = get_input<double>("cell_size");
      data_handle<double> grading = get_input<double>("grading");
      data_handle<int> optimization_steps = get_input<int>("optimization_steps");
      data_handle<bool> delaunay = get_input<bool>("delaunay");
      data_handle<viennamesh_string> optimize_string = get_input<viennamesh_string>("optimize_string");
      data_handle<double> relative_find_identic_surface_eps = get_input<double>("relative_find_identic_surface_eps");


      data_handle<netgen::mesh> output_mesh = make_data<netgen::mesh>();


      StdCaptureHandle capture_handle;

      ::netgen::CSGeometry * geom;

      try
      {
        std::istringstream csg_stream(csg_source());
        geom = ::netgen::ParseCSG( csg_stream );

        if (relative_find_identic_surface_eps.valid())
          geom->FindIdenticSurfaces(relative_find_identic_surface_eps() * geom->MaxSize());
        else
          geom->FindIdenticSurfaces(1e-8 * geom->MaxSize());
        ::netgen::MeshingParameters mesh_parameters;

        if (cell_size.valid())
          mesh_parameters.maxh = cell_size();

        if (grading.valid())
          mesh_parameters.grading = grading();
        else
          mesh_parameters.grading = 0.3;

        if (optimization_steps.valid())
          mesh_parameters.optsteps3d = optimization_steps();
        else
          mesh_parameters.optsteps3d = 3;

        if (delaunay.valid())
          mesh_parameters.delaunay = delaunay();
        else
          mesh_parameters.delaunay = true;

        if (optimize_string.valid())
          mesh_parameters.optimize3d = optimize_string().c_str();

        netgen::mesh * om = &const_cast<netgen::mesh&>(output_mesh());

        geom->GenerateMesh(om, mesh_parameters, 1, 5);

        delete geom;

        set_output("mesh", output_mesh);
      }
      catch (::netgen::NgException const & ex)
      {
        error(1) << "Netgen Error: " << ex.What() << std::endl;
        delete geom;
        return false;
      }

      return true;
    }

  }
}

// #endif
