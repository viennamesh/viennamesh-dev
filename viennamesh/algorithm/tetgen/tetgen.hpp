#ifndef VIENNAMESH_ALGORITHM_TETGEN_TETGEN_HPP
#define VIENNAMESH_ALGORITHM_TETGEN_TETGEN_HPP

#include "viennamesh/core/algorithm.hpp"

#include "viennamesh/utils/plc_tools.hpp"

#include "tetgen/tetgen.h"



namespace viennamesh
{
  namespace tetgen
  {

    class InputMesh : public tetgenio
    {};

    class OutputMesh : public tetgenio
    {};





    bool convert(viennagrid::plc_3d_mesh const & input, tetgen::InputMesh & output)
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

        int polygon_index = 0;
        for (int polygon_index = 0; polygon_index != polygons.size(); ++polygon_index)
        {
          tetgenio::polygon & polygon = facet.polygonlist[polygon_index];
          polygon.numberofvertices = polygons[polygon_index].size();
          polygon.vertexlist = new int[ polygons[polygon_index].size() ];

          for (int vertex_index = 0; vertex_index != polygons[polygon_index].size(); ++vertex_index)
            polygon.vertexlist[vertex_index] = vertex_handle_to_tetgen_index_map[ polygons[polygon_index][vertex_index] ];
        }
      }

      return true;
    }



    bool convert(SegmentedMesh<viennagrid::plc_3d_mesh, viennagrid::plc_3d_segmentation> const & input, tetgen::InputMesh & output)
    {
      return convert(input.mesh, output );
    }




    bool convert(viennagrid::triangular_3d_mesh const & input, tetgen::InputMesh & output)
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


    bool convert(SegmentedMesh<viennagrid::triangular_3d_mesh, viennagrid::triangular_3d_segmentation> const & input, tetgen::InputMesh & output)
    {
      return convert(input.mesh, output );
    }





    bool convert(tetgen::OutputMesh const & input, viennagrid::tetrahedral_3d_mesh & output)
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




    bool convert(tetgen::OutputMesh const & input, SegmentedMesh<viennagrid::tetrahedral_3d_mesh, viennagrid::tetrahedral_3d_segmentation> & output)
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








    class Algorithm : public BaseAlgorithm
    {
    public:

      bool run_impl()
      {
        ConstDoubleParameterHandle param;

        viennamesh::result_of::const_parameter_handle<tetgen::InputMesh>::type input_mesh = inputs.get<tetgen::InputMesh>("default");

        if (!input_mesh)
        {
          error(1) << "Input Parameter 'default' (type: mesh) is missing or of non-convertable type" << std::endl;
          return false;
        }

        tetgenbehavior tetgen_settings;
  //             tetgen_settings.quiet = 1;
        tetgen_settings.plc = 1;

        param = inputs.get<double>("cell_radius_edge_ratio");
        if (param)
        {
          tetgen_settings.quality = 1;
          tetgen_settings.minratio = param->value;
        }

        param = inputs.get<double>("cell_size");
        if (param)
        {
          tetgen_settings.fixedvolume = 1;
          tetgen_settings.maxvolume = param->value;
        }

        tetgen_settings.steiner = -1;     // Steiner Points?? -1 = unlimited, 0 = no steiner points
  //      tetgen_settings.metric = 1;
  //      const_cast<tetgenio::TetSizeFunc&>(native_input_mesh.tetunsuitable) = test_volume;


        tetgen_settings.useshelles = tetgen_settings.plc || tetgen_settings.refine || tetgen_settings.coarse || tetgen_settings.quality; // tetgen.cxx:3008
        tetgen_settings.goodratio = tetgen_settings.minratio; // tetgen.cxx:3009
        tetgen_settings.goodratio *= tetgen_settings.goodratio; // tetgen.cxx:3010

        // tetgen.cxx:3040
        if (tetgen_settings.fixedvolume || tetgen_settings.varvolume) {
          if (tetgen_settings.quality == 0) {
            tetgen_settings.quality = 1;
          }
        }

        tetgen_settings.goodangle = cos(tetgen_settings.minangle * tetgenmesh::PI / 180.0);   // tetgen.cxx:3046
        tetgen_settings.goodangle *= tetgen_settings.goodangle;                               // tetgen.cxx:3047


        tetgenio & tmp = (tetgenio&)input_mesh->value;

        int old_numberofregions = tmp.numberofregions;
        REAL * old_regionlist = tmp.regionlist;

        tmp.numberofregions = 0;
        tmp.regionlist = NULL;

        int old_numberofholes = tmp.numberofholes;
        REAL * old_holelist = tmp.holelist;

        tmp.numberofholes = 0;
        tmp.holelist = NULL;

        typedef viennamesh::result_of::const_parameter_handle<SeedPoint3DContainer>::type ConstSeedPointContainerHandle;
        ConstSeedPointContainerHandle seed_points_handle = inputs.get<SeedPoint3DContainer>("seed_points");
        if (seed_points_handle && !seed_points_handle->value.empty())
        {
          std::cout << "Found seed points" << std::endl;

          SeedPoint3DContainer const & seed_points = seed_points_handle->value;

          tmp.numberofregions = seed_points.size();
          tmp.regionlist = new REAL[5 * seed_points.size()];

          for (int i = 0; i < seed_points.size(); ++i)
          {
            std::cout << "  Seed Point " << seed_points[i].first << " for segment " << seed_points[i].second << std::endl;
            tmp.regionlist[5*i+0] = seed_points[i].first[0];
            tmp.regionlist[5*i+1] = seed_points[i].first[1];
            tmp.regionlist[5*i+2] = seed_points[i].first[2];
            tmp.regionlist[5*i+3] = REAL(seed_points[i].second);
            tmp.regionlist[5*i+4] = REAL(seed_points[i].second);
          }

          tetgen_settings.regionattrib = 1;
        }


        typedef viennamesh::result_of::const_parameter_handle<Point3DContainer>::type ConstPointContainerHandle;
        ConstPointContainerHandle hole_points_handle = inputs.get<Point3DContainer>("hole_points");
        if (hole_points_handle && !hole_points_handle->value.empty())
        {
          std::cout << "Found hole points" << std::endl;

          Point3DContainer const & hole_points = hole_points_handle->value;

          tmp.numberofholes = hole_points.size();
          tmp.holelist = new REAL[3 * hole_points.size()];

          for (int i = 0; i < hole_points.size(); ++i)
          {
            std::cout << "  Hole Point " << hole_points[i] << std::endl;
            tmp.holelist[3*i+0] = hole_points[i][0];
            tmp.holelist[3*i+1] = hole_points[i][1];
            tmp.holelist[3*i+2] = hole_points[i][2];
          }
        }

        viennamesh::result_of::parameter_handle<tetgen::OutputMesh>::type output_mesh = make_parameter<tetgen::OutputMesh>();


        viennautils::StdCapture capture;
        capture.start();

        tetrahedralize(&tetgen_settings, &tmp, &output_mesh->value);

        capture.finish();
        info(5) << capture.get() << std::endl;

        delete[] tmp.regionlist;
        delete[] tmp.holelist;

        tmp.numberofregions = old_numberofregions;
        tmp.regionlist = old_regionlist;

        tmp.numberofholes = old_numberofholes;
        tmp.holelist = old_holelist;

        outputs.get_create("default") = output_mesh;

        return true;
      }

    private:
    };




  }





  template<>
  struct static_init_impl< tetgen::InputMesh >
  {
    typedef tetgen::InputMesh SelfType;

    static void init()
    {
      typedef viennagrid::plc_3d_mesh PLC3DViennaGridMeshType;
      typedef SegmentedMesh<viennagrid::plc_3d_mesh, viennagrid::plc_3d_segmentation> SegmentedPLC3DViennaGridMeshType;
      typedef viennagrid::triangular_3d_mesh Triangle3DViennaGridMeshType;
      typedef SegmentedMesh<viennagrid::triangular_3d_mesh, viennagrid::triangular_3d_segmentation> SegmentedTriangle3DViennaGridMeshType;

      Converter::get().register_conversion<PLC3DViennaGridMeshType, SelfType>( &tetgen::convert );
      Converter::get().register_conversion<SegmentedPLC3DViennaGridMeshType, SelfType>( &tetgen::convert );
      Converter::get().register_conversion<Triangle3DViennaGridMeshType, SelfType>( &tetgen::convert );
      Converter::get().register_conversion<SegmentedTriangle3DViennaGridMeshType, SelfType>( &tetgen::convert );

      TypeProperties::get().set_property<SelfType>( "is_mesh", "true" );
      TypeProperties::get().set_property<SelfType>( "geometric_dimension", "3" );
      TypeProperties::get().set_property<SelfType>( "cell_type", "plc" );
    }
  };


  template<>
  struct static_init_impl< tetgen::OutputMesh >
  {
    typedef tetgen::OutputMesh SelfType;

    static void init()
    {
      typedef viennagrid::tetrahedral_3d_mesh Tetrahedral3DViennaGridMeshType;
      typedef SegmentedMesh<viennagrid::tetrahedral_3d_mesh, viennagrid::tetrahedral_3d_segmentation> SegmentedTetrahedral3DViennaGridMeshType;

      Converter::get().register_conversion<SelfType, Tetrahedral3DViennaGridMeshType>( &tetgen::convert );
      Converter::get().register_conversion<SelfType, SegmentedTetrahedral3DViennaGridMeshType>( &tetgen::convert );

      TypeProperties::get().set_property<SelfType>( "is_mesh", "true" );
      TypeProperties::get().set_property<SelfType>( "geometric_dimension", "3" );
      TypeProperties::get().set_property<SelfType>( "cell_type", "tetrahedron" );
    }
  };


}



#endif
