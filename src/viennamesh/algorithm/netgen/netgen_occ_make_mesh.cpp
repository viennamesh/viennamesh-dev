#if defined(VIENNAMESH_WITH_NETGEN) && defined(VIENNAMESH_NETGEN_WITH_OPENCASCADE)

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

#include "viennamesh/algorithm/netgen/netgen_occ_make_mesh.hpp"
#include "viennamesh/algorithm/netgen/netgen_mesh.hpp"
#include "viennamesh/algorithm/io/common.hpp"

#define OCCGEOMETRY
#include "netgen/libsrc/occ/occgeom.hpp"


namespace viennamesh
{
  namespace netgen
  {
    occ_make_mesh::occ_make_mesh() :
      filename(*this, parameter_information("filename","string","The filename of the OpenCascade .STEP or .IGES file")),
      filetype(*this, parameter_information("filetype","string","The filetype of the OpenCascade file. Supported filetypes: OCC_STEP, OCC_IGES")),
      cell_size(*this, parameter_information("cell_size","double","The desired maximum size of tetrahedrons, all tetrahedrons will be at most this size")),
      curvature_safety_factor(*this, parameter_information("curvature_safety_factor","double","A safety factor for curvatures"), 2.0),
      segments_per_edge(*this, parameter_information("segments_per_edge","double","An edge should be split into how many segments"), 1.0),
      grading(*this, parameter_information("grading","double","The grading defines change of element size, 0 -> uniform mesh, 1 -> agressive local mesh"), greater_check<double>(0.0), 0.3),
      output_mesh(*this, parameter_information("mesh", "mesh", "The output mesh, netgen::mesh")) {}

    std::string occ_make_mesh::name() const { return "Netgen 5.1 OpenCascade mesher"; }
    std::string occ_make_mesh::id() const { return "netgen_occ_make_mesh"; }

    bool occ_make_mesh::run_impl()
    {
      io::FileType ft;
      if (filetype.valid())
        ft = io::from_string( filetype() );
      else
        ft = io::from_filename( filename() );

      output_parameter_proxy<netgen::mesh> omp(output_mesh);
      ::netgen::OCCGeometry * geometry;

      if (ft == io::OCC_STEP)
        geometry = ::netgen::LoadOCC_STEP( filename().c_str() );
      else if (ft == io::OCC_IGES)
        geometry = ::netgen::LoadOCC_IGES( filename().c_str() );
      else
      {
        error(1) << "File type \"" << io::to_string(ft) << "\" is not supported" << std::endl;
        return false;
      }

      // http://sourceforge.net/p/netgen-mesher/discussion/905307/thread/7176bc7d/
      TopTools_IndexedMapOfShape FMap;
      FMap.Assign( geometry->fmap );
      if (!FMap.Extent())
      {
        std::cout << "Error retrieving Face map... (OpenCascade error)" << endl;
        return false;
      }

      ::netgen::MeshingParameters mp;

      mp.elementorder = 0;
      mp.quad = 0;
      mp.inverttets = 0;
      mp.inverttrigs = 0;

      if (cell_size.valid())
      {
        mp.uselocalh = 1;
        mp.maxh = cell_size();
      }


      mp.curvaturesafety = curvature_safety_factor();
      mp.segmentsperedge = segments_per_edge();
      mp.grading = grading();

      int perfstepsend = 6;

      omp()().geomtype = ::netgen::Mesh::GEOM_OCC;
      ::netgen::occparam.resthcloseedgeenable = 0; //mp.closeedgeenable;
      ::netgen::occparam.resthcloseedgefac = 1.0; //mp.closeedgefact;

      ::netgen::mparam = mp;

      omp()().DeleteMesh();
      ::netgen::OCCSetLocalMeshSize( *geometry, omp()() );


      ::netgen::OCCFindEdges(*geometry, omp()());

      ::netgen::OCCMeshSurface(*geometry, omp()(), perfstepsend);
      omp()().CalcSurfacesOfNode();

      ::netgen::MeshVolume(mp, omp()());
      ::netgen::RemoveIllegalElements( omp()() );
      ::netgen::MeshQuality3d( omp()() );

      ::netgen::OptimizeVolume(mp, omp()() );

      return true;
    }
  }
}

#endif
