#ifndef VIENNAMESH_ALGORITHM_TRIANGLE_TRIANGLE_HPP
#define VIENNAMESH_ALGORITHM_TRIANGLE_TRIANGLE_HPP

#include "viennamesh/core/algorithm.hpp"


// defines for triangle library
#define REAL double
#define VOID int
#define ANSI_DECLARATORS
#define TRILIBRARY
extern "C"
{
  #include "triangle/triangle.h"
}



namespace viennamesh
{
  namespace triangle
  {
    struct BaseMesh
    {
      BaseMesh()
      {
        mesh.pointlist = NULL;
        mesh.pointattributelist = NULL;
        mesh.pointmarkerlist = NULL;
        mesh.numberofpoints = 0;
        mesh.numberofpointattributes = 0;

        mesh.trianglelist = NULL;
        mesh.triangleattributelist = NULL;
        mesh.trianglearealist = NULL;
        mesh.neighborlist = NULL;
        mesh.numberoftriangles = 0;
        mesh.numberofcorners = 0;
        mesh.numberoftriangleattributes = 0;

        mesh.segmentlist = NULL;
        mesh.segmentmarkerlist = NULL;
        mesh.numberofsegments = 0;

        mesh.holelist = NULL;
        mesh.numberofholes = 0;

        mesh.regionlist = NULL;
        mesh.numberofregions = 0;

        mesh.edgelist = NULL;
        mesh.edgemarkerlist = NULL;
        mesh.normlist = NULL;
        mesh.numberofedges = 0;
      }

      ~BaseMesh()
      {
        if (mesh.pointlist) free(mesh.pointlist);
        if (mesh.pointattributelist) free(mesh.pointattributelist);
        if (mesh.pointmarkerlist) free(mesh.pointmarkerlist);

        if (mesh.trianglelist) free(mesh.trianglelist);
        if (mesh.triangleattributelist) free(mesh.triangleattributelist);
        if (mesh.trianglearealist) free(mesh.trianglearealist);
        if (mesh.neighborlist) free(mesh.neighborlist);

        if (mesh.segmentlist) free(mesh.segmentlist);
        if (mesh.segmentmarkerlist) free(mesh.segmentmarkerlist);

        if (mesh.holelist) free(mesh.holelist);

        if (mesh.regionlist) free(mesh.regionlist);

        if (mesh.edgelist) free(mesh.edgelist);
        if (mesh.edgemarkerlist) free(mesh.edgemarkerlist);
        if (mesh.normlist) free(mesh.normlist);
      }

      void init_points(int num_points)
      {
        if (mesh.pointlist) free(mesh.pointlist);
        mesh.pointlist = (REAL*)malloc( sizeof(REAL) * 2 * num_points);
        mesh.numberofpoints = num_points;
      }

      void init_segments(int num_segments)
      {
        if (mesh.segmentlist) free(mesh.segmentlist);
        mesh.segmentlist = (int*)malloc( sizeof(int) * 2 * num_segments);
        mesh.numberofsegments = num_segments;
      }

      void init_regions(int num_regions)
      {
        if (mesh.regionlist) free(mesh.regionlist);
        mesh.regionlist = (REAL*)malloc( sizeof(REAL) * 4 * num_regions);
        mesh.numberofregions = num_regions;
      }

      void init_holes(int num_holes)
      {
        if (mesh.holelist) free(mesh.holelist);
        mesh.holelist = (REAL*)malloc( sizeof(REAL) * 2 * num_holes);
        mesh.numberofholes = num_holes;
      }

      triangulateio mesh;
    };

    class InputMesh : public BaseMesh
    {};

    class OutputMesh : public BaseMesh
    {};




    template<typename WrappedContigT>
    bool generic_convert( viennagrid::mesh<WrappedContigT> const & input, triangle::InputMesh & output )
    {
      typedef viennagrid::mesh<WrappedContigT> ViennaGridMeshType;
      typedef typename viennagrid::result_of::const_vertex_handle<ViennaGridMeshType>::type ConstVertexHandleType;

      typedef typename viennagrid::result_of::const_vertex_range<ViennaGridMeshType>::type ConstVertexRangeType;
      typedef typename viennagrid::result_of::iterator<ConstVertexRangeType>::type ConstVertexIteratorType;

      typedef typename viennagrid::result_of::const_line_range<ViennaGridMeshType>::type ConstLineRangeType;
      typedef typename viennagrid::result_of::iterator<ConstLineRangeType>::type ConstCellIteratorType;


      std::map<ConstVertexHandleType, int> vertex_handle_to_tetgen_index_map;

      ConstVertexRangeType vertices(input);
      output.init_points( vertices.size() );

      int index = 0;
      for (ConstVertexIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit, ++index)
      {
        output.mesh.pointlist[index*2+0] = viennagrid::point(input, *vit)[0];
        output.mesh.pointlist[index*2+1] = viennagrid::point(input, *vit)[1];

        vertex_handle_to_tetgen_index_map[ vit.handle() ] = index;
      }


      ConstLineRangeType lines(input);
      output.init_segments( lines.size() );

      index = 0;
      for (ConstCellIteratorType lit = lines.begin(); lit != lines.end(); ++lit, ++index)
      {
        output.mesh.segmentlist[2*index+0] = vertex_handle_to_tetgen_index_map[ viennagrid::vertices(*lit).handle_at(0) ];
        output.mesh.segmentlist[2*index+1] = vertex_handle_to_tetgen_index_map[ viennagrid::vertices(*lit).handle_at(1) ];
      }

      return true;
    }

    bool convert( viennagrid::plc_2d_mesh const & input, InputMesh & output )
    {
      return generic_convert(input, output);
    }

    bool convert( viennagrid::line_2d_mesh const & input, InputMesh & output )
    {
      return generic_convert(input, output);
    }






    bool convert(triangle::OutputMesh const & input, viennagrid::triangular_2d_mesh & output)
    {
      typedef viennagrid::triangular_2d_mesh ViennaGridMeshType;
      typedef viennagrid::result_of::point<ViennaGridMeshType>::type PointType;

      typedef viennagrid::result_of::vertex_handle<ViennaGridMeshType>::type VertexHandleType;

      std::vector<VertexHandleType> vertex_handles(input.mesh.numberofpoints);

      std::cout << "Num attribs: " << input.mesh.numberoftriangleattributes << std::endl;

      for (int i = 0; i < input.mesh.numberofpoints; ++i)
      {
        PointType point;

        point[0] = input.mesh.pointlist[2*i+0];
        point[1] = input.mesh.pointlist[2*i+1];

        vertex_handles[i] = viennagrid::make_vertex( output, point );
      }


      for (int i = 0; i < input.mesh.numberoftriangles; ++i)
      {
        if (input.mesh.numberoftriangleattributes > 0)
          std::cout << input.mesh.triangleattributelist[i] << std::endl;

        viennagrid::make_triangle(
          output,
          vertex_handles[ input.mesh.trianglelist[3*i+0] ],
          vertex_handles[ input.mesh.trianglelist[3*i+1] ],
          vertex_handles[ input.mesh.trianglelist[3*i+2] ]
        );
      }

      return true;
    }



    static bool convert(triangle::OutputMesh const & input, SegmentedMesh<viennagrid::triangular_2d_mesh, viennagrid::triangular_2d_segmentation> & output)
    {
      typedef viennagrid::triangular_2d_mesh ViennaGridMeshType;
      typedef viennagrid::result_of::point<ViennaGridMeshType>::type PointType;

      typedef viennagrid::result_of::vertex_handle<ViennaGridMeshType>::type VertexHandleType;

      std::vector<VertexHandleType> vertex_handles(input.mesh.numberofpoints);

      for (int i = 0; i < input.mesh.numberofpoints; ++i)
      {
        PointType point;

        point[0] = input.mesh.pointlist[2*i+0];
        point[1] = input.mesh.pointlist[2*i+1];

        vertex_handles[i] = viennagrid::make_vertex( output.mesh, point );
      }


      for (int i = 0; i < input.mesh.numberoftriangles; ++i)
      {
        if (input.mesh.numberoftriangleattributes == 0)
        {
          viennagrid::make_triangle(
            output.mesh,
            vertex_handles[ input.mesh.trianglelist[3*i+0] ],
            vertex_handles[ input.mesh.trianglelist[3*i+1] ],
            vertex_handles[ input.mesh.trianglelist[3*i+2] ]
          );
        }
        else
        {
          viennagrid::triangular_2d_segmentation::segment_id_type segment_id = input.mesh.triangleattributelist[i];

          viennagrid::make_triangle(
            output.segmentation.get_make_segment(segment_id),
            vertex_handles[ input.mesh.trianglelist[3*i+0] ],
            vertex_handles[ input.mesh.trianglelist[3*i+1] ],
            vertex_handles[ input.mesh.trianglelist[3*i+2] ]
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

        viennamesh::result_of::const_parameter_handle<triangle::InputMesh>::type input_mesh = inputs.get<triangle::InputMesh>("default");

        if (!input_mesh)
        {
          error(1) << "Input Parameter 'default' (type: mesh) is missing or of non-convertable type" << std::endl;
          return false;
        }

        std::ostringstream options;
        options << "zpDq";
        param = inputs.get<double>("min_angle");
        if (param)
          options << param->value;

        triangulateio tmp = input_mesh->value.mesh;


        typedef viennamesh::result_of::const_parameter_handle<SeedPoint2DContainer>::type ConstSeedPointContainerHandle;

        ConstSeedPointContainerHandle seed_points_handle = inputs.get<SeedPoint2DContainer>("seed_points");
        if (seed_points_handle)
        {
          info(5) << "Using seed points" << std::endl;

          SeedPoint2DContainer const & seed_points = seed_points_handle->value;

          tmp.numberofregions = seed_points.size();
          if (tmp.regionlist) free(tmp.regionlist);
          tmp.regionlist = (REAL*)malloc( 4*sizeof(REAL)*tmp.numberofregions );

          for (unsigned int i = 0; i < seed_points.size(); ++i)
          {
            tmp.regionlist[4*i+0] = seed_points[i].first[0];
            tmp.regionlist[4*i+1] = seed_points[i].first[1];
            tmp.regionlist[4*i+2] = REAL(seed_points[i].second);
            tmp.regionlist[4*i+3] = REAL(seed_points[i].second);
          }

          options << "A";
        }

        param = inputs.get<double>("cell_size");
        if (param)
          options << "a" << param->value;

        char * buffer = (char *)malloc( options.str().length() * sizeof(char) );
        std::strcpy(buffer, options.str().c_str());

        viennamesh::result_of::parameter_handle<triangle::OutputMesh>::type output_mesh = make_parameter<triangle::OutputMesh>();

        viennautils::StdCapture capture;
        capture.start();

        triangulate( buffer, &tmp, &output_mesh->value.mesh, NULL);

        capture.finish();
        info(5) << capture.get() << std::endl;

        free(buffer);

        outputs.get_create("default") = output_mesh;

        return true;
      }

    private:
    };

  }


  template<>
  struct static_init_impl< triangle::InputMesh >
  {
    typedef triangle::InputMesh SelfType;

    static void init()
    {
      typedef viennagrid::plc_2d_mesh PLC2DViennaGridMeshType;
      typedef viennagrid::line_2d_mesh Line2DViennaGridMeshType;

      Converter::get().register_conversion<PLC2DViennaGridMeshType, SelfType>( &triangle::convert );
      Converter::get().register_conversion<Line2DViennaGridMeshType, SelfType>( &triangle::convert );

      TypeProperties::get().set_property<SelfType>( "is_mesh", "true" );
      TypeProperties::get().set_property<SelfType>( "geometric_dimension", "2" );
      TypeProperties::get().set_property<SelfType>( "cell_type", "line" );
    }

  };


  template<>
  struct static_init_impl< triangle::OutputMesh >
  {
    typedef triangle::OutputMesh SelfType;

    static void init()
    {
        typedef viennagrid::triangular_2d_mesh Triangular2DViennaGridMeshType;
        typedef SegmentedMesh<viennagrid::triangular_2d_mesh, viennagrid::triangular_2d_segmentation> SegmentedTriangular2DViennaGridMeshType;

        Converter::get().register_conversion<SelfType, Triangular2DViennaGridMeshType>( &triangle::convert );
        Converter::get().register_conversion<SelfType, SegmentedTriangular2DViennaGridMeshType>( &triangle::convert );

      TypeProperties::get().set_property<SelfType>( "is_mesh", "true" );
      TypeProperties::get().set_property<SelfType>( "geometric_dimension", "2" );
      TypeProperties::get().set_property<SelfType>( "cell_type", "triangle" );
    }
  };

}

#endif
