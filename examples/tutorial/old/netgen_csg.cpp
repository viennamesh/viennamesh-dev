#include <iostream>
#include <sstream>

#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/io/netgen_reader.hpp"
#include "viennagrid/io/vtk_writer.hpp"

#include "netgen/libsrc/csg/csg.hpp"




namespace netgen
{
  extern CSGeometry * ParseCSG (istream & istr);
}


using namespace netgen;




int main()
{
  std::ifstream geometry_file;
  geometry_file.open("../data/half-trigate.geo");

  CSGeometry * geom = ParseCSG( geometry_file );


  geom->FindIdenticSurfaces(1e-8 * geom->MaxSize());

  Mesh * netgen_mesh = new Mesh();
  netgen::MeshingParameters mparams;

  geom->GenerateMesh(netgen_mesh, mparams, 1, 5);


  {
    typedef viennagrid::tetrahedral_3d_mesh MeshType;
    typedef viennagrid::tetrahedral_3d_segmentation SegmentationType;

    MeshType mesh;
    SegmentationType segmentation(mesh);


    typedef viennagrid::result_of::point<MeshType>::type PointType;
    typedef viennagrid::result_of::vertex_handle<MeshType>::type VertexHandleType;
    typedef SegmentationType::segment_handle_type SegmentType;


    int num_points = netgen_mesh->GetNP();
    int num_tets = netgen_mesh->GetNE();

    std::cout << "Num Points: " << num_points << std::endl;
    std::cout << "Num Tets: " << num_tets << std::endl;

    std::vector<VertexHandleType> vertex_handles( num_points );

    for (int i = 1; i <= num_points; ++i)
    {
      PointType vgrid_point(netgen_mesh->Point(i)[0], netgen_mesh->Point(i)[1], netgen_mesh->Point(i)[2]);
      vertex_handles[i-1] = viennagrid::make_vertex( mesh, vgrid_point );
    }


    for (int i = 0; i < num_tets; ++i)
    {
        ElementIndex ei = i;
        SegmentType segment = segmentation.get_make_segment( (*netgen_mesh)[ei].GetIndex() );
        viennagrid::make_tetrahedron( segment, vertex_handles[(*netgen_mesh)[ei][0]-1], vertex_handles[(*netgen_mesh)[ei][1]-1],
              vertex_handles[(*netgen_mesh)[ei][2]-1], vertex_handles[(*netgen_mesh)[ei][3]-1]);
    }


    viennagrid::io::vtk_writer<MeshType, SegmentationType> writer;
    writer(mesh, segmentation, "out");
  }


}
