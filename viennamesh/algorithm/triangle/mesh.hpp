#ifndef VIENNAMESH_ALGORITHM_TRIANGLE_MESH_HPP
#define VIENNAMESH_ALGORITHM_TRIANGLE_MESH_HPP


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
    struct base_mesh
    {
      base_mesh()
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

      ~base_mesh()
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

      triangulateio mesh;
    };

    class input_mesh : public base_mesh
    {
    public:
      ~input_mesh()
      {
        if (mesh.holelist) free(mesh.holelist);
        if (mesh.regionlist) free(mesh.regionlist);
      }
    };

    class output_mesh : public base_mesh
    {};











    template<typename WrappedContigT>
    bool generic_convert( viennagrid::mesh<WrappedContigT> const & input, triangle::input_mesh & output )
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

    inline bool convert( viennagrid::brep_2d_mesh const & input, input_mesh & output )
    {
      return generic_convert(input, output);
    }

    inline bool convert( viennagrid::segmented_mesh<viennagrid::brep_2d_mesh, viennagrid::result_of::segmentation<viennagrid::brep_2d_mesh>::type> const & input, input_mesh & output )
    {
      return generic_convert(input.mesh, output);
    }






    static bool convert(triangle::output_mesh const & input, viennagrid::triangular_2d_mesh & output)
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

        vertex_handles[i] = viennagrid::make_vertex( output, point );
      }


      for (int i = 0; i < input.mesh.numberoftriangles; ++i)
      {
        viennagrid::make_triangle(
          output,
          vertex_handles[ input.mesh.trianglelist[3*i+0] ],
          vertex_handles[ input.mesh.trianglelist[3*i+1] ],
          vertex_handles[ input.mesh.trianglelist[3*i+2] ]
        );
      }

      return true;
    }



    static bool convert(triangle::output_mesh const & input, viennagrid::segmented_mesh<viennagrid::triangular_2d_mesh, viennagrid::triangular_2d_segmentation> & output)
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
  }




  template<>
  struct static_init_impl< triangle::input_mesh >
  {
    typedef triangle::input_mesh SelfType;

    static void init()
    {
      typedef viennagrid::brep_2d_mesh BRep2DViennaGridMeshType;
      typedef viennagrid::segmented_mesh<viennagrid::brep_2d_mesh, viennagrid::result_of::segmentation<viennagrid::brep_2d_mesh>::type> SegmentedBRep2DViennaGridMeshType;

      converter::get().register_conversion<BRep2DViennaGridMeshType, SelfType>( &triangle::convert );
      converter::get().register_conversion<SegmentedBRep2DViennaGridMeshType, SelfType>( &triangle::convert );

      type_properties::get().set_property<SelfType>( "is_mesh", "true" );
      type_properties::get().set_property<SelfType>( "geometric_dimension", "2" );
      type_properties::get().set_property<SelfType>( "cell_type", "line" );
    }

  };


  template<>
  struct static_init_impl< triangle::output_mesh >
  {
    typedef triangle::output_mesh SelfType;

    static void init()
    {
      typedef viennagrid::triangular_2d_mesh Triangular2DViennaGridMeshType;
      typedef viennagrid::segmented_mesh<viennagrid::triangular_2d_mesh, viennagrid::triangular_2d_segmentation> SegmentedTriangular2DViennaGridMeshType;

      converter::get().register_conversion<SelfType, Triangular2DViennaGridMeshType>( &triangle::convert );
      converter::get().register_conversion<SelfType, SegmentedTriangular2DViennaGridMeshType>( &triangle::convert );

      type_properties::get().set_property<SelfType>( "is_mesh", "true" );
      type_properties::get().set_property<SelfType>( "geometric_dimension", "2" );
      type_properties::get().set_property<SelfType>( "cell_type", "triangle" );
    }
  };

}

#endif
