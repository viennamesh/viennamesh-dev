#include "tetgen_mesh.hpp"
#include "viennagridpp/core.hpp"

namespace viennamesh
{
  viennamesh_error convert(viennagrid::mesh_t const & input, tetgen::mesh & output)
  {
    typedef viennagrid::mesh_t ViennaGridMeshType;

    typedef viennagrid::result_of::const_element<ViennaGridMeshType>::type ConstVertexType;
    typedef viennagrid::result_of::const_element<ViennaGridMeshType>::type ConstCellType;

    typedef viennagrid::result_of::const_element_range<ViennaGridMeshType, 2>::type ConstCellRangeType;
    typedef viennagrid::result_of::iterator<ConstCellRangeType>::type ConstCellIteratorType;

    std::map<ConstVertexType, int> vertex_handle_to_tetgen_index_map;

    output.firstnumber = 0;
    output.numberofpoints = 0;
    output.pointlist = new REAL[ viennagrid::vertices(input).size() * 3 ];

    int index = 0;

    ConstCellRangeType cells(input);

    output.numberoffacets = cells.size();
    output.facetlist = new tetgenio::facet[output.numberoffacets];

    index = 0;
    for (ConstCellIteratorType cit = cells.begin(); cit != cells.end(); ++cit, ++index)
    {
      typedef viennagrid::result_of::const_element_range<ConstCellType, 1>::type ConstLineOnCellRange;
      typedef viennagrid::result_of::iterator<ConstLineOnCellRange>::type ConstLineOnCellIterator;

      tetgenio::facet & facet = output.facetlist[index];
      facet.holelist = 0;

      if ( (*cit).tag().is_plc() )
        tetgen::set_hole_points( facet, viennagrid::hole_points(*cit) );

      ConstLineOnCellRange lines(*cit);
      facet.numberofpolygons = lines.size();
      facet.polygonlist = new tetgenio::polygon[ lines.size() ];

      std::size_t polygon_index = 0;
      for (ConstLineOnCellIterator lcit = lines.begin(); lcit != lines.end(); ++lcit, ++polygon_index)
      {
        tetgenio::polygon & polygon = facet.polygonlist[polygon_index];
        polygon.numberofvertices = 2;
        polygon.vertexlist = new int[ 2 ];

        for (int i = 0; i < 2; ++i)
        {
          std::map<ConstVertexType, int>::iterator vit = vertex_handle_to_tetgen_index_map.find( viennagrid::vertices(*lcit)[i] );
          if (vit != vertex_handle_to_tetgen_index_map.end())
          {
            polygon.vertexlist[i] = vit->second;
          }
          else
          {
            output.pointlist[output.numberofpoints*3+0] = viennagrid::get_point(viennagrid::vertices(*lcit)[i])[0];
            output.pointlist[output.numberofpoints*3+1] = viennagrid::get_point(viennagrid::vertices(*lcit)[i])[1];
            output.pointlist[output.numberofpoints*3+2] = viennagrid::get_point(viennagrid::vertices(*lcit)[i])[2];

            polygon.vertexlist[i] = output.numberofpoints;
            vertex_handle_to_tetgen_index_map[viennagrid::vertices(*lcit)[i]] = output.numberofpoints;

            ++output.numberofpoints;
          }
        }
      }
    }

    return VIENNAMESH_SUCCESS;
  }


  viennamesh_error convert(tetgen::mesh const & input, viennagrid::mesh_t & output)
  {
    typedef viennagrid::mesh_t MeshType;
    typedef viennagrid::result_of::point<MeshType>::type PointType;
    typedef viennagrid::result_of::element<MeshType>::type VertexType;
    typedef viennagrid::result_of::element<MeshType>::type CellType;

    std::vector<VertexType> vertex_handles(input.numberofpoints);

    for (int i = 0; i < input.numberofpoints; ++i)
    {
      vertex_handles[i] = viennagrid::make_vertex( output,
        viennagrid::make_point(input.pointlist[3*i+0], input.pointlist[3*i+1], input.pointlist[3*i+2])
      );
    }

    for (int i = 0; i < input.numberoftetrahedra; ++i)
    {
      CellType cell = viennagrid::make_tetrahedron(
        output,
        vertex_handles[ input.tetrahedronlist[4*i+0] ],
        vertex_handles[ input.tetrahedronlist[4*i+1] ],
        vertex_handles[ input.tetrahedronlist[4*i+2] ],
        vertex_handles[ input.tetrahedronlist[4*i+3] ]
      );

      if (input.numberoftetrahedronattributes != 0)
      {
        int segment_id = input.tetrahedronattributelist[i] + 0.5;
        viennagrid::add(output.get_make_region(segment_id), cell);
      }
    }

    return VIENNAMESH_SUCCESS;
  }








  template<>
  viennamesh_error internal_convert<viennagrid_mesh, tetgen::mesh>(viennagrid_mesh const & input, tetgen::mesh & output)
  { return convert( viennagrid::mesh_t(input), output ); }

  template<>
  viennamesh_error internal_convert<tetgen::mesh, viennagrid_mesh>(tetgen::mesh const & input, viennagrid_mesh & output)
  {
    viennagrid::mesh_t output_pp(output);
    return convert( input, output_pp );
  }

}
