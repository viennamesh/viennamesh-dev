#ifndef VIENNAMESH_ALGORITHM_TRIANGLE_3D_MESH_HPP
#define VIENNAMESH_ALGORITHM_TRIANGLE_3D_MESH_HPP

#include <cstring>
#include <cstdlib>
#include <list>

#include "viennamesh/algorithm/triangle/triangle_mesh.hpp"
#include "viennamesh/algorithm/plane_to_2d_projector.hpp"


namespace viennamesh
{
  namespace triangle
  {
    template<typename TriangleMeshT>
    struct base_cell_3d
    {
      TriangleMeshT plc;

      std::vector<point_2d> hole_points_2d;

      std::vector<int> vertex_ids;
      std::vector<int> segment_ids;
    };

    typedef base_cell_3d<input_mesh> input_cell_3d;
    typedef base_cell_3d<output_mesh> output_cell_3d;

    template<typename CellT>
    class mesh_3d
    {
    public:
      mesh_3d() : is_segmented(false) {}

      std::vector<CellT> cells;
      std::map<int, point_3d> vertex_points_3d;

      bool is_segmented;
    };

    typedef mesh_3d<input_cell_3d> input_mesh_3d;
    typedef mesh_3d<output_cell_3d> output_mesh_3d;






    template<typename CellT>
    void generic_convert_3d_plc(CellT const & input, triangle::input_cell_3d & output)
    {
      typedef typename viennagrid::result_of::point<CellT>::type PointType;
      typedef typename viennagrid::result_of::const_vertex_handle<CellT>::type ConstVertexHandleType;

      typedef typename viennagrid::result_of::const_vertex_range<CellT>::type ConstVertexRangeType;
      typedef typename viennagrid::result_of::iterator<ConstVertexRangeType>::type ConstVertexIteratorType;

      typedef typename viennagrid::result_of::const_line_range<CellT>::type ConstLineRangeType;
      typedef typename viennagrid::result_of::iterator<ConstLineRangeType>::type ConstCellIteratorType;


      std::map<ConstVertexHandleType, int> vertex_handle_to_tetgen_index_map;

      ConstVertexRangeType vertices(input);
      output.plc.init_points( vertices.size() );

      std::vector<point_3d> plc_points_3d( vertices.size() );
      std::vector<point_2d> plc_points_2d( vertices.size() );

      int index = 0;
      for (ConstVertexIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit, ++index)
        plc_points_3d[index] = viennagrid::point(*vit);

      viennagrid::plane_to_2d_projector<PointType> projection_functor;
      projection_functor.init( plc_points_3d.begin(), plc_points_3d.end(), 1e-6 );
      projection_functor.project( plc_points_3d.begin(), plc_points_3d.end(), plc_points_2d.begin() );


      output.vertex_ids.resize(vertices.size());

      index = 0;
      for (ConstVertexIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit, ++index)
      {
        output.plc.triangle_mesh.pointlist[index*2+0] = plc_points_2d[index][0];
        output.plc.triangle_mesh.pointlist[index*2+1] = plc_points_2d[index][1];

        vertex_handle_to_tetgen_index_map[ vit.handle() ] = index;

        output.vertex_ids[index] = vit->id().get();
      }

      std::vector<point_3d> const & hole_points_3d = viennagrid::hole_points(input);
      output.hole_points_2d.resize(hole_points_3d.size());
      projection_functor.project( hole_points_3d.begin(), hole_points_3d.end(), output.hole_points_2d.begin() );

      ConstLineRangeType lines(input);
      output.plc.init_segments( lines.size() );

      index = 0;
      for (ConstCellIteratorType lit = lines.begin(); lit != lines.end(); ++lit, ++index)
      {
        output.plc.triangle_mesh.segmentlist[2*index+0] = vertex_handle_to_tetgen_index_map[ viennagrid::vertices(*lit).handle_at(0) ];
        output.plc.triangle_mesh.segmentlist[2*index+1] = vertex_handle_to_tetgen_index_map[ viennagrid::vertices(*lit).handle_at(1) ];
      }
    }


    template<typename MeshSegmentT>
    void generic_convert_3d( MeshSegmentT const & input, triangle::input_mesh_3d & output )
    {
      output.is_segmented = false;

      typedef MeshSegmentT ViennaGridMeshType;

      typedef typename viennagrid::result_of::const_vertex_range<ViennaGridMeshType>::type ConstVertexRangeType;
      typedef typename viennagrid::result_of::iterator<ConstVertexRangeType>::type ConstVertexIteratorType;

      typedef typename viennagrid::result_of::const_cell_range<ViennaGridMeshType>::type ConstCellRangeType;
      typedef typename viennagrid::result_of::iterator<ConstCellRangeType>::type ConstCellIteratorType;

      ConstVertexRangeType vertices(input);
      for (ConstVertexIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit)
        output.vertex_points_3d[ vit->id().get() ] = viennagrid::point(*vit);

      ConstCellRangeType cells(input);
      output.cells.resize(cells.size());

      int index = 0;
      for (ConstCellIteratorType cit = cells.begin(); cit != cells.end(); ++cit, ++index)
        generic_convert_3d_plc(*cit, output.cells[index]);
    }

    template<typename WrappedMeshConfigT, typename WrappedSegmentationConfigT>
    void generic_convert_3d( viennagrid::segmented_mesh<
                            viennagrid::mesh<WrappedMeshConfigT>,
                            viennagrid::segmentation<WrappedSegmentationConfigT>
                          > const & input, triangle::input_mesh_3d & output )
    {
      output.is_segmented = true;

      typedef viennagrid::mesh<WrappedMeshConfigT> MeshType;
      typedef viennagrid::segmentation<WrappedSegmentationConfigT> SegmentationType;
      typedef typename viennagrid::result_of::cell<MeshType>::type CellType;

      typedef typename viennagrid::result_of::const_vertex_range<MeshType>::type ConstVertexRangeType;
      typedef typename viennagrid::result_of::iterator<ConstVertexRangeType>::type ConstVertexIteratorType;

      typedef typename viennagrid::result_of::const_cell_range<MeshType>::type ConstCellRangeType;
      typedef typename viennagrid::result_of::iterator<ConstCellRangeType>::type ConstCellIteratorType;

      typedef typename viennagrid::result_of::segment_id_range<SegmentationType, CellType>::type SegmentIDRangeType;

      ConstVertexRangeType vertices(input.mesh);
      for (ConstVertexIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit)
        output.vertex_points_3d[ vit->id().get() ] = viennagrid::point(*vit);

      ConstCellRangeType cells(input.mesh);
      output.cells.resize(cells.size());

      int index = 0;
      for (ConstCellIteratorType cit = cells.begin(); cit != cells.end(); ++cit, ++index)
      {
        generic_convert_3d_plc(*cit, output.cells[index]);
        SegmentIDRangeType segment_ids = viennagrid::segment_ids(input.segmentation, *cit);
        output.cells[index].segment_ids.resize( segment_ids.size() );
        std::copy( segment_ids.begin(), segment_ids.end(), output.cells[index].segment_ids.begin() );
      }
    }




    inline bool convert_3d( viennagrid::brep_3d_mesh const & input,
                         input_mesh_3d & output )
    {
      generic_convert_3d(input, output);
      return true;
    }

    inline bool convert_3d( viennagrid::segmented_mesh<viennagrid::brep_3d_mesh, viennagrid::brep_3d_segmentation> const & input,
                         input_mesh_3d & output )
    {
      generic_convert_3d(input, output);
      return true;
    }



    template<typename MeshT>
    void generic_convert_3d(triangle::output_mesh_3d const & input, MeshT & output)
    {
      typedef typename viennagrid::result_of::vertex_handle<MeshT>::type VertexHandleType;

      std::map<int, VertexHandleType> vertex_handles;

      for (std::map<int, point_3d>::const_iterator vpit = input.vertex_points_3d.begin(); vpit != input.vertex_points_3d.end(); ++vpit)
        vertex_handles[ vpit->first ] = viennagrid::make_vertex( output, vpit->second );

      for (unsigned int i = 0; i < input.cells.size(); ++i)
      {
        for (int j = 0; j < input.cells[i].plc.triangle_mesh.numberoftriangles; ++j)
        {
          viennagrid::make_triangle(
            output,
            vertex_handles[ input.cells[i].vertex_ids[ input.cells[i].plc.triangle_mesh.trianglelist[3*j+0] ] ],
            vertex_handles[ input.cells[i].vertex_ids[ input.cells[i].plc.triangle_mesh.trianglelist[3*j+1] ] ],
            vertex_handles[ input.cells[i].vertex_ids[ input.cells[i].plc.triangle_mesh.trianglelist[3*j+2] ] ]
          );
        }
      }
    }

    inline bool convert_3d(triangle::output_mesh_3d const & input, viennagrid::triangular_3d_mesh & output)
    {
      generic_convert_3d(input, output);
      return true;
    }

    inline bool convert_3d(triangle::output_mesh_3d const & input, viennagrid::thin_triangular_3d_mesh & output)
    {
      generic_convert_3d(input, output);
      return true;
    }


    template<typename MeshT, typename SegmentationT>
    void generic_convert_3d(triangle::output_mesh_3d const & input, viennagrid::segmented_mesh<MeshT, SegmentationT> & output)
    {
      if (input.is_segmented == false)
      {
        generic_convert_3d(input, output.mesh);
        return;
      }

      typedef typename viennagrid::result_of::vertex_handle<MeshT>::type VertexHandleType;
      typedef typename viennagrid::result_of::cell_handle<MeshT>::type CellHandleType;

      std::map<int, VertexHandleType> vertex_handles;

      for (std::map<int, point_3d>::const_iterator vpit = input.vertex_points_3d.begin(); vpit != input.vertex_points_3d.end(); ++vpit)
        vertex_handles[ vpit->first ] = viennagrid::make_vertex( output.mesh, vpit->second );

      for (unsigned int i = 0; i < input.cells.size(); ++i)
      {
        for (int j = 0; j < input.cells[i].plc.triangle_mesh.numberoftriangles; ++j)
        {
          CellHandleType cell_handle = viennagrid::make_triangle(
            output.mesh,
            vertex_handles[ input.cells[i].vertex_ids[ input.cells[i].plc.triangle_mesh.trianglelist[3*j+0] ] ],
            vertex_handles[ input.cells[i].vertex_ids[ input.cells[i].plc.triangle_mesh.trianglelist[3*j+1] ] ],
            vertex_handles[ input.cells[i].vertex_ids[ input.cells[i].plc.triangle_mesh.trianglelist[3*j+2] ] ]
          );

          for (unsigned int k = 0; k < input.cells[j].segment_ids.size(); ++k)
          {
            viennagrid::add(output.segmentation.get_make_segment(input.cells[j].segment_ids[k]), cell_handle);
          }
        }
      }
    }

    inline bool convert_3d(triangle::output_mesh_3d const & input, viennagrid::segmented_mesh<viennagrid::triangular_3d_mesh, viennagrid::triangular_3d_segmentation> & output)
    {
      generic_convert_3d(input, output);
      return true;
    }

    inline bool convert_3d(triangle::output_mesh_3d const & input, viennagrid::segmented_mesh<viennagrid::thin_triangular_3d_mesh, viennagrid::thin_triangular_3d_segmentation> & output)
    {
      generic_convert_3d(input, output);
      return true;
    }

    inline bool convert_3d(triangle::output_mesh_3d const & input, viennagrid::segmented_mesh<viennagrid::thin_triangular_3d_mesh, viennagrid::thin_cell_only_triangular_3d_segmentation> & output)
    {
      generic_convert_3d(input, output);
      return true;
    }
  }




  template<>
  struct type_information< triangle::input_mesh_3d >
  {
    typedef triangle::input_mesh_3d SelfType;

    static void init()
    {
      typedef viennagrid::brep_3d_mesh BRep3DViennaGridMeshType;
      typedef viennagrid::segmented_mesh<viennagrid::brep_3d_mesh, viennagrid::brep_3d_segmentation> SegmentedBRep3DViennaGridMeshType;

      converter::get().register_conversion<BRep3DViennaGridMeshType, SelfType>( &triangle::convert_3d );
      converter::get().register_conversion<SegmentedBRep3DViennaGridMeshType, SelfType>( &triangle::convert_3d );

      type_properties::get().set_property<SelfType>( "is_mesh", "true" );
      type_properties::get().set_property<SelfType>( "geometric_dimension", "3" );
      type_properties::get().set_property<SelfType>( "cell_type", "plc" );
      type_properties::get().set_property<SelfType>( "is_segmented", "true" );
    }

    static std::string name()
    { return "triangle::input_mesh_3d"; }

  };


  template<>
  struct type_information< triangle::output_mesh_3d >
  {
    typedef triangle::output_mesh_3d SelfType;

    static void init()
    {
      typedef viennagrid::triangular_3d_mesh Triangular3DViennaGridMeshType;
      typedef viennagrid::segmented_mesh<viennagrid::triangular_3d_mesh, viennagrid::triangular_3d_segmentation> SegmentedTriangular3DViennaGridMeshType;

      typedef viennagrid::thin_triangular_3d_mesh ThinTriangular3DViennaGridMeshType;
      typedef viennagrid::segmented_mesh<viennagrid::thin_triangular_3d_mesh, viennagrid::thin_triangular_3d_segmentation> ThinSegmentedTriangular3DViennaGridMeshType;

      typedef viennagrid::segmented_mesh<viennagrid::thin_triangular_3d_mesh, viennagrid::thin_cell_only_triangular_3d_segmentation> ThinCellOnlySegmentedTriangular3DViennaGridMeshType;


      converter::get().register_conversion<SelfType, Triangular3DViennaGridMeshType>( &triangle::convert_3d );
      converter::get().register_conversion<SelfType, SegmentedTriangular3DViennaGridMeshType>( &triangle::convert_3d );

      converter::get().register_conversion<SelfType, ThinTriangular3DViennaGridMeshType>( &triangle::convert_3d );
      converter::get().register_conversion<SelfType, ThinSegmentedTriangular3DViennaGridMeshType>( &triangle::convert_3d );
      converter::get().register_conversion<SelfType, ThinCellOnlySegmentedTriangular3DViennaGridMeshType>( &triangle::convert_3d );

      type_properties::get().set_property<SelfType>( "is_mesh", "true" );
      type_properties::get().set_property<SelfType>( "geometric_dimension", "3" );
      type_properties::get().set_property<SelfType>( "cell_type", "2-simplex" );
      type_properties::get().set_property<SelfType>( "is_segmented", "true" );
    }

    static std::string name()
    { return "triangle::output_mesh_3d"; }

  };

}

#endif
