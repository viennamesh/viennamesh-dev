#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/mesh/segmented_mesh.hpp"

#include "viennagrid/io/bnd_reader.hpp"
#include "viennagrid/io/vtk_writer.hpp"


int main( int argc, char** argv )
{
  if (argc != 3)
  {
    std::cout << "Usage: bnd_to_vtk [input.bnd] [output]" << std::endl;
    return 0;
  }

  typedef viennagrid::segmented_mesh<viennagrid::triangular_3d_mesh, viennagrid::triangular_3d_segmentation> SegmentedTriangleMeshType;

  SegmentedTriangleMeshType triangle_mesh;

  {
    viennagrid::io::bnd_reader reader;
    reader( triangle_mesh.mesh, triangle_mesh.segmentation, argv[1] );
  }

  {
    viennagrid::io::vtk_writer<viennagrid::triangular_3d_mesh> writer;
    writer( triangle_mesh.mesh, triangle_mesh.segmentation, argv[2] );
  }
}
