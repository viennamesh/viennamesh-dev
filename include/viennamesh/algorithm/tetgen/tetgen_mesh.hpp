#ifndef VIENNAMESH_ALGORITHM_TETGEN_MESH_HPP
#define VIENNAMESH_ALGORITHM_TETGEN_MESH_HPP

#include <list>

#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/mesh/segmented_mesh.hpp"
#include "viennamesh/core/parameter.hpp"

#ifndef TETLIBRARY
  #define TETLIBRARY
#endif
#include "tetgen/tetgen.h"


namespace viennamesh
{
  namespace tetgen
  {

    class input_mesh : public tetgenio
    {};

    class output_mesh : public tetgenio
    {};

    class input_segmentation
    {
    public:
      std::list<input_mesh> segments;
    };





    template<typename PointContainerT>
    void set_hole_points( tetgenio::facet & facet, PointContainerT const & hole_points )
    {
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
    }

    inline void set_hole_points( tetgenio::facet & facet, viennagrid::null_type const & )
    {
      facet.holelist = 0;
      facet.numberofholes = 0;
    }


    template<typename MeshSegmentT>
    void generic_convert(MeshSegmentT const & input, tetgen::input_mesh & output)
    {
      typedef MeshSegmentT ViennaGridMeshType;

      typedef typename viennagrid::result_of::const_vertex_handle<ViennaGridMeshType>::type ConstVertexHandleType;
      typedef typename viennagrid::result_of::cell<ViennaGridMeshType>::type CellType;

      typedef typename viennagrid::result_of::const_cell_range<ViennaGridMeshType>::type ConstCellRangeType;
      typedef typename viennagrid::result_of::iterator<ConstCellRangeType>::type ConstCellIteratorType;

      std::map<ConstVertexHandleType, int> vertex_handle_to_tetgen_index_map;

      output.firstnumber = 0;
      output.numberofpoints = 0;
      output.pointlist = new REAL[ viennagrid::vertices(input).size() * 3 ];

      int index = 0;
//       for (ConstVertexIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit, ++index)
//       {
//
//         output.pointlist[index*3+0] = viennagrid::point(input, *vit)[0];
//         output.pointlist[index*3+1] = viennagrid::point(input, *vit)[1];
//         output.pointlist[index*3+2] = viennagrid::point(input, *vit)[2];
//
//         vertex_handle_to_tetgen_index_map[ vit.handle() ] = index;
//       }


      ConstCellRangeType cells(input);

      output.numberoffacets = cells.size();
      output.facetlist = new tetgenio::facet[output.numberoffacets];
//       output.facetmarkerlist = new int[output.numberoffacets];

//       int vertex_index = 0;
      index = 0;
      for (ConstCellIteratorType cit = cells.begin(); cit != cells.end(); ++cit, ++index)
      {
        typedef typename viennagrid::result_of::const_line_range<CellType>::type ConstLineOnCellRange;
        typedef typename viennagrid::result_of::iterator<ConstLineOnCellRange>::type ConstLineOnCellIterator;

        tetgenio::facet & facet = output.facetlist[index];

        set_hole_points( facet, viennagrid::hole_points(*cit) );

//         std::cout << "Facet #" << index << std::endl;

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
            typename std::map<ConstVertexHandleType, int>::iterator vit = vertex_handle_to_tetgen_index_map.find( viennagrid::vertices(*lcit).handle_at(i) );
            if (vit != vertex_handle_to_tetgen_index_map.end())
            {
              polygon.vertexlist[i] = vit->second;
            }
            else
            {
              output.pointlist[output.numberofpoints*3+0] = viennagrid::point(viennagrid::vertices(*lcit)[i])[0];
              output.pointlist[output.numberofpoints*3+1] = viennagrid::point(viennagrid::vertices(*lcit)[i])[1];
              output.pointlist[output.numberofpoints*3+2] = viennagrid::point(viennagrid::vertices(*lcit)[i])[2];

              polygon.vertexlist[i] = output.numberofpoints;
              vertex_handle_to_tetgen_index_map[viennagrid::vertices(*lcit).handle_at(i)] = output.numberofpoints;

              ++output.numberofpoints;
            }
          }

//           std::cout << "  Line " << polygon_index << ": " << polygon.vertexlist[0] << " " << polygon.vertexlist[1] << std::endl;

//           polygon.vertexlist[0] = vertex_handle_to_tetgen_index_map[ viennagrid::vertices(*lcit).handle_at(0) ];
//           polygon.vertexlist[1] = vertex_handle_to_tetgen_index_map[ viennagrid::vertices(*lcit).handle_at(1) ];
        }

      }

//       std::cout << "Tetgen vertices " << output.numberofpoints << std::endl;
    }





    inline bool convert(viennagrid::brep_3d_mesh const & input,
                        viennagrid::segmented_mesh<tetgen::input_mesh, tetgen::input_segmentation> & output)
    {
      generic_convert(input, output.mesh);
      return true;
    }

    inline bool convert(viennagrid::triangular_3d_mesh const & input,
                        viennagrid::segmented_mesh<tetgen::input_mesh, tetgen::input_segmentation> & output)
    {
      generic_convert(input, output.mesh);
      return true;
    }


    template<typename WrappedMeshConfigT, typename WrappedSegmentationConfigT>
    void generic_convert(viennagrid::segmented_mesh<
                            viennagrid::mesh<WrappedMeshConfigT>,
                            viennagrid::segmentation<WrappedSegmentationConfigT>
                          > const & input,
                          viennagrid::segmented_mesh<tetgen::input_mesh, tetgen::input_segmentation> & output)
    {
//       std::cout << "Num of Lines in total " << viennagrid::lines(input.mesh).size() << std::endl;
      generic_convert( input.mesh, output.mesh );
      if (input.segmentation.size() <= 1)
        return;

      typedef viennagrid::segmentation<WrappedSegmentationConfigT> ViennaGridSegmentationType;
      for (typename ViennaGridSegmentationType::const_iterator sit = input.segmentation.begin(); sit != input.segmentation.end(); ++sit)
      {
        output.segmentation.segments.push_back( tetgen::input_mesh() );
        generic_convert( *sit, output.segmentation.segments.back() );
      }
    }


    inline bool convert(viennagrid::segmented_mesh<viennagrid::brep_3d_mesh, viennagrid::brep_3d_segmentation> const & input,
                        viennagrid::segmented_mesh<tetgen::input_mesh, tetgen::input_segmentation> & output)
    {
      generic_convert(input, output );
      return true;
    }

    inline bool convert(viennagrid::segmented_mesh<viennagrid::triangular_3d_mesh, viennagrid::triangular_3d_segmentation> const & input,
                        viennagrid::segmented_mesh<tetgen::input_mesh, tetgen::input_segmentation> & output)
    {
      generic_convert(input, output );
      return true;
    }





    template<typename MeshT>
    void generic_convert(tetgen::output_mesh const & input, MeshT & output)
    {
      typedef typename viennagrid::result_of::point<MeshT>::type PointType;
      typedef typename viennagrid::result_of::vertex_handle<MeshT>::type VertexHandleType;

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
    }

    inline bool convert(tetgen::output_mesh const & input, viennagrid::tetrahedral_3d_mesh & output)
    {
      generic_convert(input, output);
      return true;
    }

    inline bool convert(tetgen::output_mesh const & input, viennagrid::thin_tetrahedral_3d_mesh & output)
    {
      generic_convert(input, output);
      return true;
    }




    template<typename MeshT, typename SegmentationT>
    void generic_convert(tetgen::output_mesh const & input, viennagrid::segmented_mesh<MeshT, SegmentationT> & output)
    {
      if (input.numberoftetrahedronattributes == 0)
      {
        generic_convert(input, output.mesh);
        return;
      }

      typedef typename viennagrid::result_of::point<MeshT>::type PointType;
      typedef typename viennagrid::result_of::vertex_handle<MeshT>::type VertexHandleType;
      typedef typename viennagrid::result_of::cell_handle<MeshT>::type CellHandleType;
      typedef typename viennagrid::result_of::cell<MeshT>::type CellType;

      std::vector<VertexHandleType> vertex_handles(input.numberofpoints);

      for (int i = 0; i < input.numberofpoints; ++i)
      {
        vertex_handles[i] = viennagrid::make_vertex( output.mesh, PointType(input.pointlist[3*i+0], input.pointlist[3*i+1], input.pointlist[3*i+2]) );
      }

      // performance
      viennagrid::get<CellType>(viennagrid::detail::element_segment_mapping_collection(output.segmentation)).resize( input.numberoftetrahedra );


      for (int i = 0; i < input.numberoftetrahedra; ++i)
      {
        CellHandleType cell_handle = viennagrid::make_tetrahedron(
          output.mesh,
          vertex_handles[ input.tetrahedronlist[4*i+0] ],
          vertex_handles[ input.tetrahedronlist[4*i+1] ],
          vertex_handles[ input.tetrahedronlist[4*i+2] ],
          vertex_handles[ input.tetrahedronlist[4*i+3] ]
        );

        viennagrid::tetrahedral_3d_segmentation::segment_id_type segment_id = input.tetrahedronattributelist[i];
        viennagrid::unchecked_add(output.segmentation.get_make_segment(segment_id), cell_handle);
      }
    }

    inline bool convert(tetgen::output_mesh const & input, viennagrid::segmented_mesh<viennagrid::tetrahedral_3d_mesh, viennagrid::tetrahedral_3d_segmentation> & output)
    {
      generic_convert(input, output);
      return true;
    }

    inline bool convert(tetgen::output_mesh const & input, viennagrid::segmented_mesh<viennagrid::thin_tetrahedral_3d_mesh, viennagrid::thin_tetrahedral_3d_segmentation> & output)
    {
      generic_convert(input, output);
      return true;
    }

    inline bool convert(tetgen::output_mesh const & input, viennagrid::segmented_mesh<viennagrid::thin_tetrahedral_3d_mesh, viennagrid::thin_cell_only_tetrahedral_3d_segmentation> & output)
    {
      generic_convert(input, output);
      return true;
    }

  }



  template<>
  struct type_information< viennagrid::segmented_mesh<tetgen::input_mesh, tetgen::input_segmentation> >
  {
    typedef viennagrid::segmented_mesh<tetgen::input_mesh, tetgen::input_segmentation> SelfType;

    static void init()
    {
      typedef viennagrid::brep_3d_mesh BRep3DViennaGridMeshType;
      typedef viennagrid::segmented_mesh<viennagrid::brep_3d_mesh, viennagrid::brep_3d_segmentation> SegmentedBRep3DViennaGridMeshType;

      typedef viennagrid::triangular_3d_mesh Triangle3DViennaGridMeshType;
      typedef viennagrid::segmented_mesh<viennagrid::triangular_3d_mesh, viennagrid::triangular_3d_segmentation> SegmentedTriangle3DViennaGridMeshType;

      converter::get().register_conversion<BRep3DViennaGridMeshType, SelfType>( &tetgen::convert );
      converter::get().register_conversion<SegmentedBRep3DViennaGridMeshType, SelfType>( &tetgen::convert );
      converter::get().register_conversion<Triangle3DViennaGridMeshType, SelfType>( &tetgen::convert );
      converter::get().register_conversion<SegmentedTriangle3DViennaGridMeshType, SelfType>( &tetgen::convert );

      type_properties::get().set_property<SelfType>( "is_mesh", "true" );
      type_properties::get().set_property<SelfType>( "geometric_dimension", "3" );
      type_properties::get().set_property<SelfType>( "cell_type", "plc" );
      type_properties::get().set_property<SelfType>( "is_segmented", "true" );
    }

    static string name()
    { return "viennagrid::segmented_mesh<tetgen::input_mesh, tetgen::input_segmentation>"; }
  };


  template<>
  struct type_information< tetgen::output_mesh >
  {
    typedef tetgen::output_mesh SelfType;

    static void init()
    {
      typedef viennagrid::tetrahedral_3d_mesh Tetrahedral3DViennaGridMeshType;
      typedef viennagrid::segmented_mesh<viennagrid::tetrahedral_3d_mesh, viennagrid::tetrahedral_3d_segmentation> SegmentedTetrahedral3DViennaGridMeshType;

      typedef viennagrid::thin_tetrahedral_3d_mesh ThinTetrahedral3DViennaGridMeshType;
      typedef viennagrid::segmented_mesh<viennagrid::thin_tetrahedral_3d_mesh, viennagrid::thin_tetrahedral_3d_segmentation> ThinSegmentedTetrahedral3DViennaGridMeshType;

      typedef viennagrid::segmented_mesh<viennagrid::thin_tetrahedral_3d_mesh, viennagrid::thin_cell_only_tetrahedral_3d_segmentation> ThinCellOnlySegmentedTetrahedral3DViennaGridMeshType;


      converter::get().register_conversion<SelfType, Tetrahedral3DViennaGridMeshType>( &tetgen::convert );
      converter::get().register_conversion<SelfType, SegmentedTetrahedral3DViennaGridMeshType>( &tetgen::convert );

      converter::get().register_conversion<SelfType, ThinTetrahedral3DViennaGridMeshType>( &tetgen::convert );
      converter::get().register_conversion<SelfType, ThinSegmentedTetrahedral3DViennaGridMeshType>( &tetgen::convert );
      converter::get().register_conversion<SelfType, ThinCellOnlySegmentedTetrahedral3DViennaGridMeshType>( &tetgen::convert );

      type_properties::get().set_property<SelfType>( "is_mesh", "true" );
      type_properties::get().set_property<SelfType>( "geometric_dimension", "3" );
      type_properties::get().set_property<SelfType>( "cell_type", "3-simplex" );
      type_properties::get().set_property<SelfType>( "is_segmented", "true" );
    }

    static string name()
    { return "tetgen::output_mesh"; }
  };


}

#endif
