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
        std::cout << "Error retrieving Face map...." << endl;
        return false;
      }

//       cout << "Successfully extracted the Face Map....:" << FMap.Extent() << endl;

//         for(int i = 1; i <= FMap.Extent(); i++)
//         {
//             TopoDS_Face OCCface;
//             OCCface = TopoDS::Face(FMap.FindKey(i));
//
//             GProp_GProps faceProps;
//             BRepGProp::SurfaceProperties(OCCface,faceProps);
//
//             cout << "Index: " << i
//                 << " :: Area: " << faceProps.Mass()
//                 << " :: Hash: " << OCCface.HashCode(1e+6)
//                 << endl;
//         }

      ::netgen::MeshingParameters mp;
      mp.uselocalh = 1;
      mp.curvaturesafety = 2.0;
      mp.segmentsperedge = 2.0;
//         mp.elementsperedge = 2.0;
//         mp.elementspercurve = 2.0;
      mp.maxh = 0.5;
      mp.grading = 0.2;
//         mp.closeedgeenable = 0;
//         mp.closeedgefact = 1.0;
//         mp.optsurfmeshenable = 1;

      output_mesh().mesh->geomtype = ::netgen::Mesh::GEOM_OCC;
      ::netgen::occparam.resthcloseedgeenable = 0; //mp.closeedgeenable;
      ::netgen::occparam.resthcloseedgefac = 1.0; //mp.closeedgefact;

      ::netgen::mparam = mp;

      output_mesh().mesh->DeleteMesh();
      ::netgen::OCCSetLocalMeshSize( *geometry, *output_mesh().mesh );


      ::netgen::OCCFindEdges(*geometry, *output_mesh().mesh);
      int perfstepsend = 4;
      ::netgen::OCCMeshSurface(*geometry, *output_mesh().mesh, perfstepsend);
      output_mesh().mesh->CalcSurfacesOfNode();

      ::netgen::MeshVolume(mp, *output_mesh().mesh);
      ::netgen::RemoveIllegalElements( *output_mesh().mesh );
      ::netgen::OptimizeVolume(mp, *output_mesh().mesh );

//         Ng_OCC_GenerateSurfaceMesh (geometry, mesh, &mesh_parameters);
//         Ng_GenerateVolumeMesh(mesh, &mesh_parameters);

//         Ng_SaveMesh(mesh, "test.mesh");
//         Ng_DeleteMesh(mesh);

//         Ng_Exit();

      return true;
    }
  }
}
