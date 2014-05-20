#ifdef VIENNAMESH_WITH_NETGEN

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

#include "viennamesh/algorithm/netgen/netgen_csg_make_mesh.hpp"
#include "viennamesh/algorithm/netgen/netgen_mesh.hpp"

namespace viennamesh
{
  namespace netgen
  {
    csg_make_mesh::csg_make_mesh() :
      input_csg_source(*this, "csg"),
      relative_find_identic_surface_eps(*this, "relative_find_identic_surface_eps", 1e-8),
      cell_size(*this, "cell_size", 1e10),
      grading(*this, "grading", 0.3),
      optimization_steps(*this, "optimization_steps", 3),
      delaunay(*this, "delaunay", true),
      optimize_string(*this, "optimize_string"),
      output_mesh(*this, "mesh") {}

    string csg_make_mesh::name() const { return "Netgen 5.1 CSG mesher"; }
    string csg_make_mesh::id() const { return "netgen_csg_make_mesh"; }

    bool csg_make_mesh::run_impl()
    {
      output_parameter_proxy<netgen::mesh> output(output_mesh);

      std_capture().start();

      std::istringstream csg_stream(input_csg_source());
      ::netgen::CSGeometry * geom = ::netgen::ParseCSG( csg_stream );

      geom->FindIdenticSurfaces(relative_find_identic_surface_eps() * geom->MaxSize());
      ::netgen::MeshingParameters mesh_parameters;

      mesh_parameters.maxh = cell_size();
      mesh_parameters.grading = grading();
      mesh_parameters.optsteps3d = optimization_steps();
      mesh_parameters.delaunay = delaunay();

      if (optimize_string.valid())
        mesh_parameters.optimize3d = optimize_string().c_str();

      geom->GenerateMesh(output().mesh_ptr(), mesh_parameters, 1, 5);

      delete geom;

      std_capture().finish();

      return true;
    }
  }
}

#endif
