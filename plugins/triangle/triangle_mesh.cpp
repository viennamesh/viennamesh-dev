#include "triangle_mesh.hpp"
#include "viennagrid/viennagrid.hpp"



namespace viennamesh
{

  namespace triangle
  {

    void init_points(triangulateio & mesh, int num_points)
    {
      if (mesh.pointlist) free(mesh.pointlist);
      mesh.pointlist = (REAL*)malloc( sizeof(REAL) * 2 * num_points);
      mesh.numberofpoints = num_points;
    }

    void init_segments(triangulateio & mesh, int num_segments)
    {
      if (mesh.segmentlist) free(mesh.segmentlist);
      mesh.segmentlist = (int*)malloc( sizeof(int) * 2 * num_segments);
      mesh.numberofsegments = num_segments;
    }


  }



  viennamesh_error convert(viennagrid::mesh const & input, triangulateio & output)
  {
    typedef viennagrid::mesh                                              MeshType;
    typedef viennagrid::result_of::const_element<MeshType>::type          ConstVertexType;

    typedef viennagrid::result_of::const_vertex_range<MeshType>::type     ConstVertexRangeType;
    typedef viennagrid::result_of::iterator<ConstVertexRangeType>::type   ConstVertexIteratorType;

    typedef viennagrid::result_of::const_element_range<MeshType,1>::type  ConstLineRangeType;
    typedef viennagrid::result_of::iterator<ConstLineRangeType>::type     ConstCellIteratorType;

    std::map<ConstVertexType, int> vertex_handle_to_tetgen_index_map;

    ConstVertexRangeType vertices(input);
    viennamesh::triangle::init_points( output, vertices.size() );

    int index = 0;
    for (ConstVertexIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit, ++index)
    {
      output.pointlist[index*2+0] = viennagrid::get_point(input, *vit)[0];
      output.pointlist[index*2+1] = viennagrid::get_point(input, *vit)[1];

      vertex_handle_to_tetgen_index_map[ *vit ] = index;
    }


    ConstLineRangeType lines(input);
    viennamesh::triangle::init_segments( output, lines.size() );

    index = 0;
    for (ConstCellIteratorType lit = lines.begin(); lit != lines.end(); ++lit, ++index)
    {
      output.segmentlist[2*index+0] = vertex_handle_to_tetgen_index_map[ viennagrid::vertices(*lit)[0] ];
      output.segmentlist[2*index+1] = vertex_handle_to_tetgen_index_map[ viennagrid::vertices(*lit)[1] ];
    }

    return VIENNAMESH_SUCCESS;
  }


  viennamesh_error convert(triangulateio const & input, viennagrid::mesh & output)
  {
    typedef viennagrid::mesh                                    MeshType;

    typedef viennagrid::result_of::element<MeshType>::type      VertexType;
    typedef viennagrid::result_of::element<MeshType>::type      CellType;

    std::vector<VertexType> vertex_handles(input.numberofpoints);

    for (int i = 0; i < input.numberofpoints; ++i)
    {
      vertex_handles[i] = viennagrid::make_vertex( output,
        viennagrid::make_point(input.pointlist[2*i+0], input.pointlist[2*i+1])
      );
    }

    for (int i = 0; i < input.numberoftriangles; ++i)
    {
      CellType cell = viennagrid::make_triangle(
        output,
        vertex_handles[ input.trianglelist[3*i+0] ],
        vertex_handles[ input.trianglelist[3*i+1] ],
        vertex_handles[ input.trianglelist[3*i+2] ]
      );

      if (input.numberoftriangleattributes != 0)
      {
        int segment_id = input.triangleattributelist[i];
        viennagrid::add(output.get_or_create_region(segment_id), cell);
      }
    }

    return VIENNAMESH_SUCCESS;
  }



  template<>
  viennamesh_error internal_convert<viennagrid_mesh, triangle_mesh>(viennagrid_mesh const & input, triangle_mesh & output)
  { return convert( viennagrid::mesh(input), *output ); }

  template<>
  viennamesh_error internal_convert<triangle_mesh, viennagrid_mesh>(triangle_mesh const & input, viennagrid_mesh & output)
  {
    viennagrid::mesh output_pp(output);
    return convert( *input, output_pp );
  }

}
