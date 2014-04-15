#if defined(VIENNAMESH_WITH_NETGEN) && defined(VIENNAMESH_NETGEN_WITH_OPENCASCADE)

#include "viennamesh/algorithm/netgen/netgen_occ_mesh_generator.hpp"
#include "viennamesh/algorithm/netgen/netgen_mesh.hpp"

#define OCCGEOMETRY
#include "netgen/libsrc/occ/occgeom.hpp"


namespace viennamesh
{
  namespace netgen
  {
    occ_mesh_generator::occ_mesh_generator() :
      input_geometry_filename(*this, "filename"),
      cell_size(*this, "cell_size"),
      curvature_safety_factor(*this, "curvature_safety_factor", 2.0),
      segments_per_edge(*this, "segments_per_edge", 1.0),
      grading(*this, "grading", 0.3),
      output_mesh(*this, "mesh") {}

    string occ_mesh_generator::name() const { return "Netgen 5.1 OpenCascade mesher"; }
    string occ_mesh_generator::id() const { return "netgen_occ_mesh_generator"; }

    bool occ_mesh_generator::run_impl()
    {
      output_parameter_proxy<netgen::output_mesh> omp(output_mesh);

      ::netgen::OCCGeometry * geometry = ::netgen::LoadOCC_STEP( input_geometry_filename().c_str() );

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

      omp().mesh->geomtype = ::netgen::Mesh::GEOM_OCC;
      ::netgen::occparam.resthcloseedgeenable = 0; //mp.closeedgeenable;
      ::netgen::occparam.resthcloseedgefac = 1.0; //mp.closeedgefact;

      ::netgen::mparam = mp;

      omp().mesh->DeleteMesh();
      ::netgen::OCCSetLocalMeshSize( *geometry, *omp().mesh );


      ::netgen::OCCFindEdges(*geometry, *omp().mesh);

      ::netgen::OCCMeshSurface(*geometry, *omp().mesh, perfstepsend);
      omp().mesh->CalcSurfacesOfNode();

      ::netgen::MeshVolume(mp, *omp().mesh);
      ::netgen::RemoveIllegalElements( *omp().mesh );
      ::netgen::MeshQuality3d( *omp().mesh );

      ::netgen::OptimizeVolume(mp, *omp().mesh );

      return true;
    }
  }
}

#endif
