#ifndef VIENNAMESH_ALGORITHM_TETGEN_MESH_HPP
#define VIENNAMESH_ALGORITHM_TETGEN_MESH_HPP

#include "viennamesh/utils/plc_tools.hpp"

#define TETLIBRARY
#include "tetgen/tetgen.h"



namespace viennamesh
{
  namespace tetgen
  {

    class input_mesh : public tetgenio
    {};

    class output_mesh : public tetgenio
    {};






    static bool convert(viennagrid::plc_3d_mesh const & input, tetgen::input_mesh & output)
    {
      typedef viennagrid::plc_3d_mesh ViennaGridMeshType;
      typedef viennagrid::result_of::point<ViennaGridMeshType>::type PointType;

      typedef viennagrid::result_of::const_vertex_handle<ViennaGridMeshType>::type ConstVertexHandleType;
      typedef viennagrid::result_of::cell<ViennaGridMeshType>::type CellType;

      typedef viennagrid::result_of::const_vertex_range<ViennaGridMeshType>::type ConstVertexRangeType;
      typedef viennagrid::result_of::iterator<ConstVertexRangeType>::type ConstVertexIteratorType;

      typedef viennagrid::result_of::const_cell_range<ViennaGridMeshType>::type ConstCellRangeType;
      typedef viennagrid::result_of::iterator<ConstCellRangeType>::type ConstCellIteratorType;


      std::map<ConstVertexHandleType, int> vertex_handle_to_tetgen_index_map;

      ConstVertexRangeType vertices(input);

      output.firstnumber = 0;
      output.numberofpoints = vertices.size();
      output.pointlist = new REAL[ output.numberofpoints * 3 ];

      int index = 0;
      for (ConstVertexIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit, ++index)
      {

        output.pointlist[index*3+0] = viennagrid::point(input, *vit)[0];
        output.pointlist[index*3+1] = viennagrid::point(input, *vit)[1];
        output.pointlist[index*3+2] = viennagrid::point(input, *vit)[2];

        vertex_handle_to_tetgen_index_map[ vit.handle() ] = index;
      }


      ConstCellRangeType cells(input);

      output.numberoffacets = cells.size();
      output.facetlist = new tetgenio::facet[output.numberoffacets];
      output.facetmarkerlist = new int[output.numberoffacets];

      index = 0;
      for (ConstCellIteratorType cit = cells.begin(); cit != cells.end(); ++cit, ++index)
      {
        typedef viennagrid::result_of::const_line_range<CellType>::type ConstLineOnCellRange;
        typedef viennagrid::result_of::iterator<ConstLineOnCellRange>::type ConstLineOnCellIterator;

        std::vector<PointType> const & hole_points = viennagrid::hole_points(*cit);


        std::vector< std::vector<ConstVertexHandleType> > polygons = split_plc_into_polygons(*cit);

        if (polygons.empty())
          return false;

        tetgenio::facet & facet = output.facetlist[index];

        facet.numberofpolygons = polygons.size();
        facet.polygonlist = new tetgenio::polygon[ polygons.size() ];

        facet.numberofholes = hole_points.size();
        if (facet.numberofholes > 0)
        {
          facet.holelist = new REAL[ 3 * facet.numberofholes ];
          for (int hole_point_index = 0; hole_point_index != facet.numberofholes; ++hole_point_index)
          {
            facet.holelist[3*hole_point_index+0] = hole_points[hole_point_index][0];
            facet.holelist[3*hole_point_index+1] = hole_points[hole_point_index][1];
            facet.holelist[3*hole_point_index+2] = hole_points[hole_point_index][2];
          }
        }
        else
          facet.holelist = 0;

        for (std::size_t polygon_index = 0; polygon_index != polygons.size(); ++polygon_index)
        {
          tetgenio::polygon & polygon = facet.polygonlist[polygon_index];
          polygon.numberofvertices = polygons[polygon_index].size();
          polygon.vertexlist = new int[ polygons[polygon_index].size() ];

          for (std::size_t vertex_index = 0; vertex_index != polygons[polygon_index].size(); ++vertex_index)
            polygon.vertexlist[vertex_index] = vertex_handle_to_tetgen_index_map[ polygons[polygon_index][vertex_index] ];
        }
      }

      return true;
    }



    inline bool convert(viennagrid::segmented_mesh<viennagrid::plc_3d_mesh, viennagrid::plc_3d_segmentation> const & input, tetgen::input_mesh & output)
    {
      return convert(input.mesh, output );
    }




    static bool convert(viennagrid::triangular_3d_mesh const & input, tetgen::input_mesh & output)
    {
      typedef viennagrid::triangular_3d_mesh ViennaGridMeshType;
      typedef viennagrid::result_of::point<ViennaGridMeshType>::type PointType;

      typedef viennagrid::result_of::const_vertex_handle<ViennaGridMeshType>::type ConstVertexHandleType;

      typedef viennagrid::result_of::const_vertex_range<ViennaGridMeshType>::type ConstVertexRangeType;
      typedef viennagrid::result_of::iterator<ConstVertexRangeType>::type ConstVertexIteratorType;

      typedef viennagrid::result_of::const_cell_range<ViennaGridMeshType>::type ConstCellRangeType;
      typedef viennagrid::result_of::iterator<ConstCellRangeType>::type ConstCellIteratorType;


      std::map<ConstVertexHandleType, int> vertex_handle_to_tetgen_index_map;

      ConstVertexRangeType vertices(input);

      output.firstnumber = 0;
      output.numberofpoints = vertices.size();
      output.pointlist = new REAL[ output.numberofpoints * 3 ];

      int index = 0;
      for (ConstVertexIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit, ++index)
      {
        output.pointlist[index*3+0] = viennagrid::point(input, *vit)[0];
        output.pointlist[index*3+1] = viennagrid::point(input, *vit)[1];
        output.pointlist[index*3+2] = viennagrid::point(input, *vit)[2];

        vertex_handle_to_tetgen_index_map[ vit.handle() ] = index;
      }


      ConstCellRangeType triangles(input);

      output.numberoffacets = triangles.size();
      output.facetlist = new tetgenio::facet[output.numberoffacets];
      output.facetmarkerlist = new int[output.numberoffacets];

      index = 0;
      for (ConstCellIteratorType tit = triangles.begin(); tit != triangles.end(); ++tit, ++index)
      {
        tetgenio::facet & facet = output.facetlist[index];

        facet.numberofpolygons = 1;
        facet.polygonlist = new tetgenio::polygon[1];
        facet.numberofholes = 0;
        facet.holelist = 0;

        tetgenio::polygon & polygon = facet.polygonlist[0];
        polygon.numberofvertices = 3;
        polygon.vertexlist = new int[3];
        polygon.vertexlist[0] = vertex_handle_to_tetgen_index_map[ viennagrid::vertices(*tit).handle_at(0) ];
        polygon.vertexlist[1] = vertex_handle_to_tetgen_index_map[ viennagrid::vertices(*tit).handle_at(1) ];
        polygon.vertexlist[2] = vertex_handle_to_tetgen_index_map[ viennagrid::vertices(*tit).handle_at(2) ];

//         output.facetmarkerlist[index] = 0; // TODO Was tut das?????
      }

      return true;
    }


    inline bool convert(viennagrid::segmented_mesh<viennagrid::triangular_3d_mesh, viennagrid::triangular_3d_segmentation> const & input, tetgen::input_mesh & output)
    {
      return convert(input.mesh, output );
    }





    static bool convert(tetgen::output_mesh const & input, viennagrid::tetrahedral_3d_mesh & output)
    {
      typedef viennagrid::tetrahedral_3d_mesh ViennaGridMeshType;
      typedef viennagrid::result_of::point<ViennaGridMeshType>::type PointType;

      typedef viennagrid::result_of::vertex_handle<ViennaGridMeshType>::type VertexHandleType;

      std::vector<VertexHandleType> vertex_handles(input.numberofpoints);

      for (int i = 0; i < input.numberofpoints; ++i)
      {
        vertex_handles[i] = viennagrid::make_vertex( output, PointType(input.pointlist[3*i+0], input.pointlist[3*i+1], input.pointlist[3*i+2]) );
      }


      for (int i = 0; i < input.numberoftetrahedra; ++i)
      {
        viennagrid::make_tetrahedron(
          output,
          vertex_handles[ input.tetrahedronlist[4*i+0] ],
          vertex_handles[ input.tetrahedronlist[4*i+1] ],
          vertex_handles[ input.tetrahedronlist[4*i+2] ],
          vertex_handles[ input.tetrahedronlist[4*i+3] ]
        );
      }

      return true;
    }




    static bool convert(tetgen::output_mesh const & input, viennagrid::segmented_mesh<viennagrid::tetrahedral_3d_mesh, viennagrid::tetrahedral_3d_segmentation> & output)
    {
      typedef viennagrid::tetrahedral_3d_mesh ViennaGridMeshType;
      typedef viennagrid::result_of::point<ViennaGridMeshType>::type PointType;

      typedef viennagrid::result_of::vertex_handle<ViennaGridMeshType>::type VertexHandleType;

      std::vector<VertexHandleType> vertex_handles(input.numberofpoints);

      for (int i = 0; i < input.numberofpoints; ++i)
      {
        vertex_handles[i] = viennagrid::make_vertex( output.mesh, PointType(input.pointlist[3*i+0], input.pointlist[3*i+1], input.pointlist[3*i+2]) );
      }


        for (int i = 0; i < input.numberoftetrahedra; ++i)
        {
          if (input.numberoftetrahedronattributes == 0)
          {
            viennagrid::make_tetrahedron(
              output.mesh,
              vertex_handles[ input.tetrahedronlist[4*i+0] ],
              vertex_handles[ input.tetrahedronlist[4*i+1] ],
              vertex_handles[ input.tetrahedronlist[4*i+2] ],
              vertex_handles[ input.tetrahedronlist[4*i+3] ]
            );
          }
          else
          {
            viennagrid::tetrahedral_3d_segmentation::segment_id_type segment_id = input.tetrahedronattributelist[i];

            viennagrid::make_tetrahedron(
              output.segmentation.get_make_segment(segment_id),
              vertex_handles[ input.tetrahedronlist[4*i+0] ],
              vertex_handles[ input.tetrahedronlist[4*i+1] ],
              vertex_handles[ input.tetrahedronlist[4*i+2] ],
              vertex_handles[ input.tetrahedronlist[4*i+3] ]
            );
          }
        }

      return true;
    }

  }



  template<>
  struct static_init_impl< tetgen::input_mesh >
  {
    typedef tetgen::input_mesh SelfType;

    static void init()
    {
      typedef viennagrid::plc_3d_mesh PLC3DViennaGridMeshType;
      typedef viennagrid::segmented_mesh<viennagrid::plc_3d_mesh, viennagrid::plc_3d_segmentation> SegmentedPLC3DViennaGridMeshType;
      typedef viennagrid::triangular_3d_mesh Triangle3DViennaGridMeshType;
      typedef viennagrid::segmented_mesh<viennagrid::triangular_3d_mesh, viennagrid::triangular_3d_segmentation> SegmentedTriangle3DViennaGridMeshType;

      converter::get().register_conversion<PLC3DViennaGridMeshType, SelfType>( &tetgen::convert );
      converter::get().register_conversion<SegmentedPLC3DViennaGridMeshType, SelfType>( &tetgen::convert );
      converter::get().register_conversion<Triangle3DViennaGridMeshType, SelfType>( &tetgen::convert );
      converter::get().register_conversion<SegmentedTriangle3DViennaGridMeshType, SelfType>( &tetgen::convert );

      type_properties::get().set_property<SelfType>( "is_mesh", "true" );
      type_properties::get().set_property<SelfType>( "geometric_dimension", "3" );
      type_properties::get().set_property<SelfType>( "cell_type", "plc" );
    }
  };


  template<>
  struct static_init_impl< tetgen::output_mesh >
  {
    typedef tetgen::output_mesh SelfType;

    static void init()
    {
      typedef viennagrid::tetrahedral_3d_mesh Tetrahedral3DViennaGridMeshType;
      typedef viennagrid::segmented_mesh<viennagrid::tetrahedral_3d_mesh, viennagrid::tetrahedral_3d_segmentation> SegmentedTetrahedral3DViennaGridMeshType;

      converter::get().register_conversion<SelfType, Tetrahedral3DViennaGridMeshType>( &tetgen::convert );
      converter::get().register_conversion<SelfType, SegmentedTetrahedral3DViennaGridMeshType>( &tetgen::convert );

      type_properties::get().set_property<SelfType>( "is_mesh", "true" );
      type_properties::get().set_property<SelfType>( "geometric_dimension", "3" );
      type_properties::get().set_property<SelfType>( "cell_type", "tetrahedron" );
    }
  };


}

#endif
