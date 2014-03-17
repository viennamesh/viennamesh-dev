#include "viennamesh/algorithm/netgen/occ_generator.hpp"

#include "viennamesh/algorithm/netgen/mesh.hpp"

#define OCCGEOMETRY
#include "netgen/libsrc/occ/occgeom.hpp"


namespace viennamesh
{
  namespace netgen
  {
    bool occ_mesher::run_impl()
    {
      const_string_parameter_handle filename = get_required_input<string>("default");
      output_parameter_proxy<netgen::output_mesh> output_mesh = output_proxy<netgen::output_mesh>("default");

      ::netgen::OCCGeometry * geometry = ::netgen::LoadOCC_STEP( filename().c_str() );

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

      viennamesh::result_of::const_parameter_handle<double>::type cell_size = get_input<double>("cell_size");
      if (cell_size)
      {
        mp.uselocalh = 1;
        mp.maxh = cell_size();
      }

      mp.curvaturesafety = 2.0;
      copy_input( "curvature_safety_factor", mp.curvaturesafety );

      mp.segmentsperedge = 1.0;
      copy_input( "segments_per_edge", mp.segmentsperedge );

      mp.grading = 0.3;
      copy_input( "grading", mp.grading );

//       mp.Print(std::cout);


      int perfstepsend = 6;

      output_mesh().mesh->geomtype = ::netgen::Mesh::GEOM_OCC;
      ::netgen::occparam.resthcloseedgeenable = 0; //mp.closeedgeenable;
      ::netgen::occparam.resthcloseedgefac = 1.0; //mp.closeedgefact;

      ::netgen::mparam = mp;

      output_mesh().mesh->DeleteMesh();
      ::netgen::OCCSetLocalMeshSize( *geometry, *output_mesh().mesh );


      ::netgen::OCCFindEdges(*geometry, *output_mesh().mesh);

      ::netgen::OCCMeshSurface(*geometry, *output_mesh().mesh, perfstepsend);
      output_mesh().mesh->CalcSurfacesOfNode();

      ::netgen::MeshVolume(mp, *output_mesh().mesh);
      ::netgen::RemoveIllegalElements( *output_mesh().mesh );
      ::netgen::MeshQuality3d( *output_mesh().mesh );

      ::netgen::OptimizeVolume(mp, *output_mesh().mesh );

      return true;
    }
  }
}
