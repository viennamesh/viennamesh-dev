#include "tetgen_mesh.hpp"
#include "viennagrid/viennagrid.hpp"

namespace viennamesh
{
  viennamesh_error convert(viennagrid_plc plc, tetgen::mesh & output)
  {
    viennagrid_dimension geometric_dimension;
    viennagrid_plc_geometric_dimension_get(plc, &geometric_dimension);

    if (geometric_dimension != 3)
      return VIENNAMESH_ERROR_CONVERSION_FAILED;

    viennagrid_element_id vertex_begin;
    viennagrid_element_id vertex_end;
    viennagrid_plc_elements_get(plc, 0, &vertex_begin, &vertex_end);
    viennagrid_int vertex_count = vertex_end-vertex_begin;

    viennagrid_element_id facet_begin;
    viennagrid_element_id facet_end;
    viennagrid_plc_elements_get(plc, 2, &facet_begin, &facet_end);


    output.firstnumber = 0;
    output.numberofpoints = vertex_count;
    output.pointlist = new REAL[ vertex_count * 3 ];

    viennagrid_numeric * coords;
    viennagrid_plc_vertex_coords_pointer(plc, &coords);
    std::copy( coords, coords+vertex_count*3, output.pointlist );


    output.numberoffacets = facet_end-facet_begin;
    output.facetlist = new tetgenio::facet[output.numberoffacets];



    for (int facet_id = facet_begin; facet_id != facet_end; ++facet_id)
    {
      tetgenio::facet & facet = output.facetlist[viennagrid_index_from_element_id(facet_id)];
      facet.holelist = 0;

      viennagrid_int facet_hole_point_count;
      viennagrid_numeric * facet_hole_points;
      viennagrid_plc_facet_hole_points_get(plc, facet_id, &facet_hole_point_count, &facet_hole_points);

      facet.numberofholes = facet_hole_point_count;
      if (facet.numberofholes > 0)
      {
        facet.holelist = new REAL[ 3 * facet.numberofholes ];
        std::copy( facet_hole_points, facet_hole_points + facet.numberofholes*3, facet.holelist );
      }

      viennagrid_int * lines_begin;
      viennagrid_int * lines_end;
      viennagrid_plc_boundary_elements(plc, facet_id, 1, &lines_begin, &lines_end);

      viennagrid_int line_count = lines_end-lines_begin;

      facet.numberofpolygons = line_count;
      facet.polygonlist = new tetgenio::polygon[ line_count ];

      std::size_t polygon_index = 0;
      for (viennagrid_int * line_id_it = lines_begin; line_id_it != lines_end; ++line_id_it, ++polygon_index)
      {
        tetgenio::polygon & polygon = facet.polygonlist[polygon_index];
        polygon.numberofvertices = 2;
        polygon.vertexlist = new int[ 2 ];

        viennagrid_int * line_vertices_begin;
        viennagrid_int * line_vertices_end;
        viennagrid_plc_boundary_elements(plc, *line_id_it, 0, &line_vertices_begin, &line_vertices_end);

        assert( line_vertices_end-line_vertices_begin == 2 );

        polygon.vertexlist[0] = *(line_vertices_begin+0);
        polygon.vertexlist[1] = *(line_vertices_begin+1);
      }
    }

    viennagrid_int hole_point_count;
    viennagrid_numeric * hole_points;
    viennagrid_plc_volumetric_hole_points_get(plc, &hole_point_count, &hole_points);

    output.numberofholes = hole_point_count;
    if (output.numberofholes > 0)
    {
      output.holelist = new REAL[ 3 * output.numberofholes ];
      std::copy( hole_points, hole_points + output.numberofholes*3, output.holelist );
    }
    else
      output.holelist = 0;



    viennagrid_int seed_point_count;
    viennagrid_numeric * seed_points;
    viennagrid_int * seed_point_regions;
    viennagrid_plc_seed_points_get(plc, &seed_point_count, &seed_points, &seed_point_regions);

    output.numberofregions = seed_point_count;
    if (output.numberofregions > 0)
    {
      output.regionlist = new REAL[5 * output.numberofregions];
      for (viennagrid_int i = 0; i != seed_point_count; ++i)
      {
        output.regionlist[5*i+0] = seed_points[3*i+0];
        output.regionlist[5*i+1] = seed_points[3*i+1];
        output.regionlist[5*i+2] = seed_points[3*i+2];
        output.regionlist[5*i+3] = REAL(seed_point_regions[i]);
        output.regionlist[5*i+4] = 0;
      }
    }
    else
      output.regionlist = 0;


    return VIENNAMESH_SUCCESS;
  }



  viennamesh_error convert(viennagrid::mesh const & input, tetgen::mesh & output)
  {
    typedef viennagrid::mesh ViennaGridMeshType;

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




  viennamesh_error convert(tetgen::mesh const & input, viennagrid::mesh & output)
  {
    typedef viennagrid::mesh                                    MeshType;
    typedef viennagrid::result_of::element<MeshType>::type      VertexType;
    typedef viennagrid::result_of::element<MeshType>::type      CellType;

    std::vector<VertexType> vertices(input.numberofpoints);

    for (int i = 0; i < input.numberofpoints; ++i)
    {
      vertices[i] = viennagrid::make_vertex( output,
        viennagrid::make_point(input.pointlist[3*i+0], input.pointlist[3*i+1], input.pointlist[3*i+2])
      );
    }

    for (int i = 0; i < input.numberoftetrahedra; ++i)
    {
      CellType cell = viennagrid::make_tetrahedron(
        output,
        vertices[ input.tetrahedronlist[4*i+0] ],
        vertices[ input.tetrahedronlist[4*i+1] ],
        vertices[ input.tetrahedronlist[4*i+2] ],
        vertices[ input.tetrahedronlist[4*i+3] ]
      );

      if (input.numberoftetrahedronattributes != 0)
      {
        int region_id = input.tetrahedronattributelist[i] + 0.5;
        viennagrid::add(output.get_or_create_region(region_id), cell);
      }
    }

    return VIENNAMESH_SUCCESS;
  }








  template<>
  viennamesh_error internal_convert<viennagrid_plc, tetgen::mesh>(viennagrid_plc const & input, tetgen::mesh & output)
  { return convert( input, output ); }

  template<>
  viennamesh_error internal_convert<viennagrid_mesh, tetgen::mesh>(viennagrid_mesh const & input, tetgen::mesh & output)
  { return convert( viennagrid::mesh(input), output ); }

  template<>
  viennamesh_error internal_convert<tetgen::mesh, viennagrid_mesh>(tetgen::mesh const & input, viennagrid_mesh & output)
  {
    viennagrid::mesh output_pp(output);
    return convert( input, output_pp );
  }

}
