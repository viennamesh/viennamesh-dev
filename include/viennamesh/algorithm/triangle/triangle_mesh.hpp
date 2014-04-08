#ifndef VIENNAMESH_ALGORITHM_TRIANGLE_MESH_HPP
#define VIENNAMESH_ALGORITHM_TRIANGLE_MESH_HPP

#include <cstring>
#include <cstdlib>
#include <list>

#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/mesh/segmented_mesh.hpp"
#include "viennamesh/core/parameter.hpp"

extern "C"
{
  #include "triangle/triangle_interface.h"
}

namespace viennamesh
{
  namespace triangle
  {
    struct base_mesh
    {
      base_mesh()
      {
        triangle_mesh.pointlist = NULL;
        triangle_mesh.pointattributelist = NULL;
        triangle_mesh.pointmarkerlist = NULL;
        triangle_mesh.numberofpoints = 0;
        triangle_mesh.numberofpointattributes = 0;

        triangle_mesh.trianglelist = NULL;
        triangle_mesh.triangleattributelist = NULL;
        triangle_mesh.trianglearealist = NULL;
        triangle_mesh.neighborlist = NULL;
        triangle_mesh.numberoftriangles = 0;
        triangle_mesh.numberofcorners = 0;
        triangle_mesh.numberoftriangleattributes = 0;

        triangle_mesh.segmentlist = NULL;
        triangle_mesh.segmentmarkerlist = NULL;
        triangle_mesh.numberofsegments = 0;

        triangle_mesh.holelist = NULL;
        triangle_mesh.numberofholes = 0;

        triangle_mesh.regionlist = NULL;
        triangle_mesh.numberofregions = 0;

        triangle_mesh.edgelist = NULL;
        triangle_mesh.edgemarkerlist = NULL;
        triangle_mesh.normlist = NULL;
        triangle_mesh.numberofedges = 0;
      }

      ~base_mesh()
      {
        if (triangle_mesh.pointlist) free(triangle_mesh.pointlist);
        if (triangle_mesh.pointattributelist) free(triangle_mesh.pointattributelist);
        if (triangle_mesh.pointmarkerlist) free(triangle_mesh.pointmarkerlist);

        if (triangle_mesh.trianglelist) free(triangle_mesh.trianglelist);
        if (triangle_mesh.triangleattributelist) free(triangle_mesh.triangleattributelist);
        if (triangle_mesh.trianglearealist) free(triangle_mesh.trianglearealist);
        if (triangle_mesh.neighborlist) free(triangle_mesh.neighborlist);

        if (triangle_mesh.segmentlist) free(triangle_mesh.segmentlist);
        if (triangle_mesh.segmentmarkerlist) free(triangle_mesh.segmentmarkerlist);

        if (triangle_mesh.edgelist) free(triangle_mesh.edgelist);
        if (triangle_mesh.edgemarkerlist) free(triangle_mesh.edgemarkerlist);
        if (triangle_mesh.normlist) free(triangle_mesh.normlist);
      }

      void init_points(int num_points)
      {
        if (triangle_mesh.pointlist) free(triangle_mesh.pointlist);
        triangle_mesh.pointlist = (REAL*)malloc( sizeof(REAL) * 2 * num_points);
        triangle_mesh.numberofpoints = num_points;
      }

      void init_segments(int num_segments)
      {
        if (triangle_mesh.segmentlist) free(triangle_mesh.segmentlist);
        triangle_mesh.segmentlist = (int*)malloc( sizeof(int) * 2 * num_segments);
        triangle_mesh.numberofsegments = num_segments;
      }

      triangulateio triangle_mesh;
    };

    class input_mesh : public base_mesh
    {
    public:
      ~input_mesh()
      {
        if (triangle_mesh.holelist) free(triangle_mesh.holelist);
        if (triangle_mesh.regionlist) free(triangle_mesh.regionlist);
      }
    };

    class output_mesh : public base_mesh
    {};

    class input_segmentation
    {
    public:
      std::list<input_mesh> segments;
    };











    template<typename MeshSegmentT>
    void generic_convert( MeshSegmentT const & input, triangle::input_mesh & output )
    {
      typedef MeshSegmentT ViennaGridMeshType;
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
        output.triangle_mesh.pointlist[index*2+0] = viennagrid::point(input, *vit)[0];
        output.triangle_mesh.pointlist[index*2+1] = viennagrid::point(input, *vit)[1];

        vertex_handle_to_tetgen_index_map[ vit.handle() ] = index;
      }


      ConstLineRangeType lines(input);
      output.init_segments( lines.size() );

      index = 0;
      for (ConstCellIteratorType lit = lines.begin(); lit != lines.end(); ++lit, ++index)
      {
        output.triangle_mesh.segmentlist[2*index+0] = vertex_handle_to_tetgen_index_map[ viennagrid::vertices(*lit).handle_at(0) ];
        output.triangle_mesh.segmentlist[2*index+1] = vertex_handle_to_tetgen_index_map[ viennagrid::vertices(*lit).handle_at(1) ];
      }
    }


    inline bool convert( viennagrid::brep_2d_mesh const & input,
                         viennagrid::segmented_mesh<input_mesh, input_segmentation> & output )
    {
      generic_convert(input, output.mesh);
      return true;
    }



    template<typename WrappedMeshConfigT, typename WrappedSegmentationConfigT>
    void generic_convert( viennagrid::segmented_mesh<
                            viennagrid::mesh<WrappedMeshConfigT>,
                            viennagrid::segmentation<WrappedSegmentationConfigT>
                          > const & input,
                          viennagrid::segmented_mesh<triangle::input_mesh, triangle::input_segmentation> & output )
    {
      generic_convert( input.mesh, output.mesh );
      if (input.segmentation.size() <= 1)
        return;

      typedef viennagrid::segmentation<WrappedSegmentationConfigT> ViennaGridSegmentationType;
      for (typename ViennaGridSegmentationType::const_iterator sit = input.segmentation.begin(); sit != input.segmentation.end(); ++sit)
      {
        output.segmentation.segments.push_back( triangle::input_mesh() );
        generic_convert( *sit, output.segmentation.segments.back() );
      }
    }


    inline bool convert( viennagrid::segmented_mesh<viennagrid::brep_2d_mesh, viennagrid::brep_2d_segmentation> const & input,
                         viennagrid::segmented_mesh<input_mesh, input_segmentation> & output )
    {
      generic_convert(input, output);
      return true;
    }





    template<typename MeshT>
    void generic_convert(triangle::output_mesh const & input, MeshT & output)
    {
      typedef typename viennagrid::result_of::point<MeshT>::type PointType;
      typedef typename viennagrid::result_of::vertex_handle<MeshT>::type VertexHandleType;

      std::vector<VertexHandleType> vertex_handles(input.triangle_mesh.numberofpoints);

      for (int i = 0; i < input.triangle_mesh.numberofpoints; ++i)
      {
        PointType point;

        point[0] = input.triangle_mesh.pointlist[2*i+0];
        point[1] = input.triangle_mesh.pointlist[2*i+1];

        vertex_handles[i] = viennagrid::make_vertex( output, point );
      }


      for (int i = 0; i < input.triangle_mesh.numberoftriangles; ++i)
      {
        viennagrid::make_triangle(
          output,
          vertex_handles[ input.triangle_mesh.trianglelist[3*i+0] ],
          vertex_handles[ input.triangle_mesh.trianglelist[3*i+1] ],
          vertex_handles[ input.triangle_mesh.trianglelist[3*i+2] ]
        );
      }
    }

    inline bool convert(triangle::output_mesh const & input, viennagrid::triangular_2d_mesh & output)
    {
      generic_convert(input, output);
      return true;
    }

    inline bool convert(triangle::output_mesh const & input, viennagrid::thin_triangular_2d_mesh & output)
    {
      generic_convert(input, output);
      return true;
    }


    template<typename MeshT, typename SegmentationT>
    void generic_convert(triangle::output_mesh const & input, viennagrid::segmented_mesh<MeshT, SegmentationT> & output)
    {
      if (input.triangle_mesh.numberoftriangleattributes == 0)
      {
        generic_convert(input, output.mesh);
        return;
      }

      typedef typename viennagrid::result_of::point<MeshT>::type PointType;
      typedef typename viennagrid::result_of::vertex_handle<MeshT>::type VertexHandleType;
      typedef typename viennagrid::result_of::cell_handle<MeshT>::type CellHandleType;
      typedef typename viennagrid::result_of::cell<MeshT>::type CellType;

      std::vector<VertexHandleType> vertex_handles(input.triangle_mesh.numberofpoints);

      for (int i = 0; i < input.triangle_mesh.numberofpoints; ++i)
      {
        PointType point;

        point[0] = input.triangle_mesh.pointlist[2*i+0];
        point[1] = input.triangle_mesh.pointlist[2*i+1];

        vertex_handles[i] = viennagrid::make_vertex( output.mesh, point );
      }

      // performance
      viennagrid::get<CellType>(viennagrid::detail::element_segment_mapping_collection(output.segmentation)).resize( input.triangle_mesh.numberoftriangles );


      for (int i = 0; i < input.triangle_mesh.numberoftriangles; ++i)
      {
        CellHandleType cell_handle = viennagrid::make_triangle(
          output.mesh,
          vertex_handles[ input.triangle_mesh.trianglelist[3*i+0] ],
          vertex_handles[ input.triangle_mesh.trianglelist[3*i+1] ],
          vertex_handles[ input.triangle_mesh.trianglelist[3*i+2] ]
        );

        typename SegmentationT::segment_id_type segment_id = input.triangle_mesh.triangleattributelist[i];
        viennagrid::unchecked_add(output.segmentation.get_make_segment(segment_id), cell_handle);
      }
    }

    inline bool convert(triangle::output_mesh const & input, viennagrid::segmented_mesh<viennagrid::triangular_2d_mesh, viennagrid::triangular_2d_segmentation> & output)
    {
      generic_convert(input, output);
      return true;
    }

    inline bool convert(triangle::output_mesh const & input, viennagrid::segmented_mesh<viennagrid::thin_triangular_2d_mesh, viennagrid::thin_triangular_2d_segmentation> & output)
    {
      generic_convert(input, output);
      return true;
    }

    inline bool convert(triangle::output_mesh const & input, viennagrid::segmented_mesh<viennagrid::thin_triangular_2d_mesh, viennagrid::thin_cell_only_triangular_2d_segmentation> & output)
    {
      generic_convert(input, output);
      return true;
    }
  }




  template<>
  struct type_information< viennagrid::segmented_mesh<triangle::input_mesh, triangle::input_segmentation> >
  {
    typedef viennagrid::segmented_mesh<triangle::input_mesh, triangle::input_segmentation> SelfType;

    static void init()
    {
      typedef viennagrid::brep_2d_mesh BRep2DViennaGridMeshType;
      typedef viennagrid::segmented_mesh<viennagrid::brep_2d_mesh, viennagrid::result_of::segmentation<viennagrid::brep_2d_mesh>::type> SegmentedBRep2DViennaGridMeshType;

      converter::get().register_conversion<BRep2DViennaGridMeshType, SelfType>( &triangle::convert );
      converter::get().register_conversion<SegmentedBRep2DViennaGridMeshType, SelfType>( &triangle::convert );

      type_properties::get().set_property<SelfType>( "is_mesh", "true" );
      type_properties::get().set_property<SelfType>( "geometric_dimension", "2" );
      type_properties::get().set_property<SelfType>( "cell_type", "1-simplex" );
      type_properties::get().set_property<SelfType>( "is_segmented", "true" );
    }

    static string name()
    { return "viennagrid::segmented_mesh<triangle::input_mesh, triangle::input_segmentation>"; }

  };


  template<>
  struct type_information< triangle::output_mesh >
  {
    typedef triangle::output_mesh SelfType;

    static void init()
    {
      typedef viennagrid::triangular_2d_mesh Triangular2DViennaGridMeshType;
      typedef viennagrid::segmented_mesh<viennagrid::triangular_2d_mesh, viennagrid::triangular_2d_segmentation> SegmentedTriangular2DViennaGridMeshType;

      typedef viennagrid::thin_triangular_2d_mesh ThinTriangular2DViennaGridMeshType;
      typedef viennagrid::segmented_mesh<viennagrid::thin_triangular_2d_mesh, viennagrid::thin_triangular_2d_segmentation> ThinSegmentedTriangular2DViennaGridMeshType;

      typedef viennagrid::segmented_mesh<viennagrid::thin_triangular_2d_mesh, viennagrid::thin_cell_only_triangular_2d_segmentation> ThinCellOnlySegmentedTriangular2DViennaGridMeshType;


      converter::get().register_conversion<SelfType, Triangular2DViennaGridMeshType>( &triangle::convert );
      converter::get().register_conversion<SelfType, SegmentedTriangular2DViennaGridMeshType>( &triangle::convert );

      converter::get().register_conversion<SelfType, ThinTriangular2DViennaGridMeshType>( &triangle::convert );
      converter::get().register_conversion<SelfType, ThinSegmentedTriangular2DViennaGridMeshType>( &triangle::convert );
      converter::get().register_conversion<SelfType, ThinCellOnlySegmentedTriangular2DViennaGridMeshType>( &triangle::convert );

      type_properties::get().set_property<SelfType>( "is_mesh", "true" );
      type_properties::get().set_property<SelfType>( "geometric_dimension", "2" );
      type_properties::get().set_property<SelfType>( "cell_type", "2-simplex" );
      type_properties::get().set_property<SelfType>( "is_segmented", "true" );
    }

    static string name()
    { return "triangle::output_mesh"; }

  };

}

#endif
