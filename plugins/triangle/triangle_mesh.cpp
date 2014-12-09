#include "triangle_mesh.hpp"
#include "viennagrid/core.hpp"

namespace viennamesh
{
  int convert_to_triangle(viennamesh_data input_, viennamesh_data output_)
  {
    typedef viennagrid::mesh_t ViennaGridMeshType;
    typedef viennagrid::result_of::const_element<ViennaGridMeshType>::type ConstVertexType;

    typedef viennagrid::result_of::const_vertex_range<ViennaGridMeshType>::type ConstVertexRangeType;
    typedef viennagrid::result_of::iterator<ConstVertexRangeType>::type ConstVertexIteratorType;

    typedef viennagrid::result_of::const_element_range<ViennaGridMeshType,1>::type ConstLineRangeType;
    typedef viennagrid::result_of::iterator<ConstLineRangeType>::type ConstCellIteratorType;

    ViennaGridMeshType input( *(viennagrid_mesh*)input_ );
    triangle::input_mesh* output = (triangle::input_mesh*)output_;

    std::map<ConstVertexType, int> vertex_handle_to_tetgen_index_map;

    ConstVertexRangeType vertices(input);
    output->init_points( vertices.size() );

    int index = 0;
    for (ConstVertexIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit, ++index)
    {
      output->triangle_mesh.pointlist[index*2+0] = viennagrid::get_point(input, *vit)[0];
      output->triangle_mesh.pointlist[index*2+1] = viennagrid::get_point(input, *vit)[1];

      vertex_handle_to_tetgen_index_map[ *vit ] = index;
    }


    ConstLineRangeType lines(input);
    output->init_segments( lines.size() );

    index = 0;
    for (ConstCellIteratorType lit = lines.begin(); lit != lines.end(); ++lit, ++index)
    {
      output->triangle_mesh.segmentlist[2*index+0] = vertex_handle_to_tetgen_index_map[ viennagrid::vertices(*lit)[0] ];
      output->triangle_mesh.segmentlist[2*index+1] = vertex_handle_to_tetgen_index_map[ viennagrid::vertices(*lit)[1] ];
    }

    return VIENNAMESH_SUCCESS;
  }




  int convert_from_triangle(viennamesh_data input_, viennamesh_data output_)
  {
    typedef viennagrid::mesh_t MeshType;
    typedef viennagrid::result_of::point<MeshType>::type PointType;
    typedef viennagrid::result_of::element<MeshType>::type VertexType;
    typedef viennagrid::result_of::element<MeshType>::type CellType;

    triangle::output_mesh* input = (triangle::output_mesh*)input_;
    MeshType output( *(viennagrid_mesh*)output_ );

    std::vector<VertexType> vertex_handles(input->triangle_mesh.numberofpoints);

    for (int i = 0; i < input->triangle_mesh.numberofpoints; ++i)
    {
      vertex_handles[i] = viennagrid::make_vertex( output,
        viennagrid::make_point(input->triangle_mesh.pointlist[2*i+0], input->triangle_mesh.pointlist[2*i+1])
      );
    }

    for (int i = 0; i < input->triangle_mesh.numberoftriangles; ++i)
    {
      CellType cell = viennagrid::make_triangle(
        output,
        vertex_handles[ input->triangle_mesh.trianglelist[3*i+0] ],
        vertex_handles[ input->triangle_mesh.trianglelist[3*i+1] ],
        vertex_handles[ input->triangle_mesh.trianglelist[3*i+2] ]
      );

      if (input->triangle_mesh.numberoftriangleattributes != 0)
      {
        int segment_id = input->triangle_mesh.triangleattributelist[i];
        viennagrid::add(output.get_make_region(segment_id), cell);
      }
    }

    return VIENNAMESH_SUCCESS;
  }

}
