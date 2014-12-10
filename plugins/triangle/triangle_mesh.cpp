#include "triangle_mesh.hpp"
#include "viennagrid/core.hpp"
#include "viennagrid/algorithm/plane_to_2d_projector.hpp"

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






  void convert_to_triangle_3d_cell(viennagrid::element_t const & input, triangle::input_cell_3d & output)
  {
    typedef viennagrid::element_t CellType;

    typedef viennagrid::result_of::point<CellType>::type PointType;
    typedef viennagrid::result_of::const_element<CellType>::type ConstVertexType;

    typedef viennagrid::result_of::const_vertex_range<CellType>::type ConstVertexRangeType;
    typedef viennagrid::result_of::iterator<ConstVertexRangeType>::type ConstVertexIteratorType;

    typedef viennagrid::result_of::const_element_range<CellType, 1>::type ConstLineRangeType;
    typedef viennagrid::result_of::iterator<ConstLineRangeType>::type ConstCellIteratorType;


    std::map<ConstVertexType, int> vertex_handle_to_tetgen_index_map;

    ConstVertexRangeType vertices(input);
    output.plc.init_points( vertices.size() );

    std::vector<point_t> plc_points_3d( vertices.size() );
    std::vector<point_t> plc_points_2d( vertices.size() );

    int index = 0;
    for (ConstVertexIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit, ++index)
      plc_points_3d[index] = viennagrid::get_point(*vit);

    viennagrid::plane_to_2d_projector<PointType> projection_functor;
    projection_functor.init( plc_points_3d.begin(), plc_points_3d.end(), 1e-6 );
    projection_functor.project( plc_points_3d.begin(), plc_points_3d.end(), plc_points_2d.begin() );


    output.vertex_ids.resize(vertices.size());

    index = 0;
    for (ConstVertexIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit, ++index)
    {
      output.plc.triangle_mesh.pointlist[index*2+0] = plc_points_2d[index][0];
      output.plc.triangle_mesh.pointlist[index*2+1] = plc_points_2d[index][1];

      vertex_handle_to_tetgen_index_map[ *vit ] = index;

      output.vertex_ids[index] = (*vit).id();
    }

    std::vector<point_t> const & hole_points_3d = viennagrid::hole_points(input);
    output.hole_points_2d.resize(hole_points_3d.size());
    projection_functor.project( hole_points_3d.begin(), hole_points_3d.end(), output.hole_points_2d.begin() );

    ConstLineRangeType lines(input);
    output.plc.init_segments( lines.size() );

    index = 0;
    for (ConstCellIteratorType lit = lines.begin(); lit != lines.end(); ++lit, ++index)
    {
      output.plc.triangle_mesh.segmentlist[2*index+0] = vertex_handle_to_tetgen_index_map[ viennagrid::vertices(*lit)[0] ];
      output.plc.triangle_mesh.segmentlist[2*index+1] = vertex_handle_to_tetgen_index_map[ viennagrid::vertices(*lit)[1] ];
    }
  }


  int convert_to_triangle_3d(viennamesh_data input_, viennamesh_data output_)
  {
    typedef viennagrid::mesh_t MeshType;

    MeshType input( *(viennagrid_mesh*)input_ );
    triangle::input_mesh_3d * output = (triangle::input_mesh_3d*)output_;

    if ( input.region_count() > 1 )
      output->is_segmented = true;

    typedef viennagrid::result_of::element<MeshType>::type CellType;

    typedef viennagrid::result_of::const_vertex_range<MeshType>::type ConstVertexRangeType;
    typedef viennagrid::result_of::iterator<ConstVertexRangeType>::type ConstVertexIteratorType;

    typedef viennagrid::result_of::const_cell_range<MeshType>::type ConstCellRangeType;
    typedef viennagrid::result_of::iterator<ConstCellRangeType>::type ConstCellIteratorType;

    typedef viennagrid::result_of::region_range<MeshType, CellType>::type RegionRangeType;
    typedef viennagrid::result_of::iterator<RegionRangeType>::type RegionRangeIterator;

    ConstVertexRangeType vertices(input);
    for (ConstVertexIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit)
      output->vertex_points_3d[ (*vit).id() ] = viennagrid::get_point(*vit);

    ConstCellRangeType cells(input);
    output->cells.resize(cells.size());

    int index = 0;
    for (ConstCellIteratorType cit = cells.begin(); cit != cells.end(); ++cit, ++index)
    {
      convert_to_triangle_3d_cell(*cit, output->cells[index]);

      RegionRangeType regions(input, *cit);
      for (RegionRangeIterator rit = regions.begin(); rit != regions.end(); ++rit)
        output->cells[index].segment_ids.push_back( (*rit).id() );
    }

    return VIENNAMESH_SUCCESS;
  }


  int convert_from_triangle_3d(viennamesh_data input_, viennamesh_data output_)
  {
    typedef viennagrid::mesh_t MeshType;
    typedef viennagrid::result_of::element<MeshType>::type VertexType;
    typedef viennagrid::result_of::element<MeshType>::type CellType;

    triangle::output_mesh_3d * input = (triangle::output_mesh_3d*)input_;
    MeshType output( *(viennagrid_mesh*)output_ );

    std::map<int, VertexType> vertex_handles;

    for (std::map<int, point_t>::const_iterator vpit = input->vertex_points_3d.begin(); vpit != input->vertex_points_3d.end(); ++vpit)
      vertex_handles[ vpit->first ] = viennagrid::make_vertex( output, vpit->second );

    for (unsigned int i = 0; i < input->cells.size(); ++i)
    {
      for (int j = 0; j < input->cells[i].plc.triangle_mesh.numberoftriangles; ++j)
      {
        CellType cell = viennagrid::make_triangle(
          output,
          vertex_handles[ input->cells[i].vertex_ids[ input->cells[i].plc.triangle_mesh.trianglelist[3*j+0] ] ],
          vertex_handles[ input->cells[i].vertex_ids[ input->cells[i].plc.triangle_mesh.trianglelist[3*j+1] ] ],
          vertex_handles[ input->cells[i].vertex_ids[ input->cells[i].plc.triangle_mesh.trianglelist[3*j+2] ] ]
        );

        for (unsigned int k = 0; k < input->cells[j].segment_ids.size(); ++k)
        {
          if (input->is_segmented)
            viennagrid::add(output.get_make_region(input->cells[j].segment_ids[k]), cell);
        }
      }
    }

    return VIENNAMESH_SUCCESS;
  }

}
