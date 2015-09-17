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
#include "netgen_make_mesh.hpp"

#include "viennagrid/algorithm/distance.hpp"
#include "viennagrid/algorithm/spanned_volume.hpp"

namespace viennamesh
{
  namespace netgen
  {
    make_mesh::make_mesh() {}

    std::string make_mesh::name() { return "netgen_make_mesh"; }

    bool make_mesh::run(viennamesh::algorithm_handle &)
    {
      data_handle<netgen::mesh> input_mesh = get_input<netgen::mesh>("mesh");
      data_handle<double> cell_size = get_input<double>("cell_size");

      data_handle<netgen::mesh> output_mesh = make_data<netgen::mesh>();
      netgen::mesh & mesh = const_cast<netgen::mesh&>(output_mesh());

      StdCaptureHandle capture_handle;

      mesh = input_mesh();


      ::netgen::MeshingParameters mesh_parameters;

      mesh_parameters.delaunay = true;
      mesh_parameters.checkoverlap = 0;

      if (cell_size.valid())
      {
        ::netgen::Point3d pmin;
        ::netgen::Point3d pmax;
        mesh.GetBox(pmin, pmax);

        double bb_volume = std::abs( (pmax.X() - pmin.X()) * (pmax.Y() - pmin.Y()) * (pmax.Z() - pmin.Z()) );
        double cell_volume = cell_size()*cell_size()*cell_size();
        double max_cell_count = 100000000;

        if ( cell_volume * max_cell_count < bb_volume )
        {
          warning(1) << "Cell size is too small and might result in too much elements" << std::endl;
          warning(1) << "Cell size                = " << cell_size() << std::endl;
          warning(1) << "Mesh max cell count      = " << max_cell_count << std::endl;
          warning(1) << "Mesh bounding box        = " << pmin << "," << pmax << std::endl;
          warning(1) << "Mesh bounding box volume = " << bb_volume << std::endl;
          warning(1) << "Mesh max cell volume     = " << cell_volume * max_cell_count << std::endl;
        }

        mesh_parameters.maxh = cell_size();
      }

      try
      {
        mesh.CalcLocalH(mesh_parameters.grading);
        MeshVolume (mesh_parameters, mesh);
        RemoveIllegalElements (mesh);
        OptimizeVolume (mesh_parameters, mesh);
      }
      catch (::netgen::NgException const & ex)
      {
        error(1) << "Netgen Error: " << ex.What() << std::endl;
        return false;
      }

      set_output("mesh", output_mesh);

      return true;
    }
  }
}

// #endif
