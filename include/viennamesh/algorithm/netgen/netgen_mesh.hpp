#ifndef VIENNAMESH_ALGORITHM_NETGEN_MESH_HPP
#define VIENNAMESH_ALGORITHM_NETGEN_MESH_HPP

#include "netgen/libsrc/csg/csg.hpp"
namespace netgen
{
  extern CSGeometry * ParseCSG (istream & istr);
}

namespace viennamesh
{
  namespace netgen
  {

//     struct geometry
//     {
//       geometry() : geo(NULL) {}
//       ~geometry() { delete geo; }
//
//       netgen::CSGeometry * geo;
//     };


//     struct netgen_lib
//     {
//         netgen_lib()
//         {
// //             std::cout << "Init Netgen" << std::endl;
//             nglib::Ng_Init();
// //             netgen::printmessage_importance = -1;
//         }
//
//         ~netgen_lib()
//         {
// //             std::cout << "Exit Netgen" << std::endl;
//             nglib::Ng_Exit();
//         }
//     };


    struct segmented_mesh
    {
    public:
      segmented_mesh() {}
      ~segmented_mesh() { deinit(); }

      typedef ::netgen::Mesh InternalMeshType;

      InternalMeshType & operator()( std::size_t segment_id ) { return *segments[segment_id]; }
      InternalMeshType const & operator()( std::size_t segment_id ) const { return *segments[segment_id]; }

      template<typename SegmentationT>
      void init( SegmentationT const & segmentation )
      {
        deinit();
        for (std::size_t i = 0; i < segments.size(); ++i)
          segments.push_back( new InternalMeshType() );
      }

      void deinit()
      {
        for (std::size_t i = 0; i < segments.size(); ++i)
          delete segments[i];
        segments.clear();
      }

      std::size_t segment_count() const { return segments.size(); }

      segmented_mesh & operator=(segmented_mesh const & other)
      {
        deinit();
        for (std::size_t i = 0; i < other.segment_count(); ++i)
        {
          InternalMeshType * tmp = new InternalMeshType();
          *tmp = *other.segments[i];
          segments.push_back(tmp);
        }
        return *this;
      }

    private:
      std::vector<InternalMeshType *> segments;
    };

    struct output_mesh
    {
    public:
      output_mesh() : mesh( new ::netgen::Mesh() ) {}
      ~output_mesh() { delete mesh; }

      typedef ::netgen::Mesh InternalMeshType;

      InternalMeshType & operator()() { return *mesh; }
      InternalMeshType const & operator()() const { return *mesh; }

      InternalMeshType * & mesh_ptr() { return mesh; }

    private:
      InternalMeshType * mesh;
    };





    template<typename MeshT, typename SegmentationT>
    bool convert(viennagrid::segmented_mesh<MeshT, SegmentationT> const & input, netgen::segmented_mesh & output)
    {
      typedef typename viennagrid::result_of::point<MeshT>::type PointType;
      typedef typename SegmentationT::segment_handle_type SegmentType;

      typedef typename viennagrid::result_of::vertex_id<MeshT>::type VertexIDType;
      typedef typename viennagrid::result_of::vertex_handle<MeshT>::type VertexHandleType;
      typedef typename viennagrid::result_of::triangle<MeshT>::type TriangleType;

      typedef typename viennagrid::result_of::const_triangle_range<SegmentationT>::type TriangleRangeType;
      typedef typename viennagrid::result_of::iterator<TriangleRangeType>::type TriangleIteratorType;

      output.init( input.segmentation );

      for (std::size_t seg_id = 0; seg_id != output.segment_count(); ++seg_id)
      {
        SegmentType const & vgrid_segment = input.segmentation(seg_id);

        std::map<VertexIDType, int> vertex_index_map;
        int index = 0;

        segmented_mesh::InternalMeshType & current_mesh = output(seg_id);

        TriangleRangeType triangles( vgrid_segment );

        unsigned int triangle_count = 0;
        for (TriangleIteratorType it = triangles.begin(); it != triangles.end(); ++it)
        {
          TriangleType const & triangle = *it;

          ++triangle_count;

          int indices[3];
          for (int i = 0; i < 3; ++i)
          {
            VertexIDType vertex_id = viennagrid::vertices(triangle)[i].id();

            typename std::map<VertexIDType, int>::iterator tmp = vertex_index_map.find(vertex_id);
            if (tmp == vertex_index_map.end())
            {
              PointType const & p = viennagrid::point( viennagrid::vertices(triangle)[0] );

              tmp = vertex_index_map.insert( std::make_pair(vertex_id, index+1) ).first;     // increase by one because netgen start counting at 1
              ++index;
              current_mesh.AddPoint( Point3d( p[0], p[1], p[2] ) );
            }

            indices[i] = tmp->second;
          }

          bool faces_outward = true;
          bool const * faces_outward_on_segment_pointer = viennagrid::segment_element_info( vgrid_segment, triangle );
          if (faces_outward_on_segment_pointer)
            faces_outward = *faces_outward_on_segment_pointer;

          if (!faces_outward)
            std::swap( indices[1], indices[2] );

          ::netgen::Element2d el(3);
          el.SetIndex (1);
          el.PNum(1) = indices[0];
          el.PNum(2) = indices[1];
          el.PNum(3) = indices[2];
          current_mesh.AddSurfaceElement (el);
        }
      }

      return true;
    }








    template<typename MeshT>
    bool convert(netgen::output_mesh const & input, MeshT & output)
    {
      typedef typename viennagrid::result_of::point<MeshT>::type PointType;
      typedef typename viennagrid::result_of::vertex_handle<MeshT>::type VertexHandleType;


      int num_points = input().GetNP();
      int num_tets = input().GetNE();

      std::vector<VertexHandleType> vertex_handles( num_points );

      for (int i = 1; i <= num_points; ++i)
      {
        PointType vgrid_point(input().Point(i)[0], input().Point(i)[1], input().Point(i)[2]);
        vertex_handles[i-1] = viennagrid::make_vertex( output, vgrid_point );
      }


      for (int i = 0; i < num_tets; ++i)
      {
          ::netgen::ElementIndex ei = i;
          viennagrid::make_tetrahedron( output,
                                        vertex_handles[input()[ei][0]-1],
                                        vertex_handles[input()[ei][1]-1],
                                        vertex_handles[input()[ei][2]-1],
                                        vertex_handles[input()[ei][3]-1]);
      }

      return true;
    }




    template<typename MeshT, typename SegmentationT>
    bool convert(netgen::output_mesh const & input, viennagrid::segmented_mesh<MeshT, SegmentationT> & output)
    {
      typedef typename viennagrid::result_of::point<MeshT>::type PointType;
      typedef typename viennagrid::result_of::vertex_handle<MeshT>::type VertexHandleType;
      typedef typename viennagrid::result_of::segment_handle<SegmentationT>::type ViennaGridSegmentType;


      int num_points = input().GetNP();
      int num_tets = input().GetNE();

      std::vector<VertexHandleType> vertex_handles( num_points );

      for (int i = 1; i <= num_points; ++i)
      {
        PointType vgrid_point(input().Point(i)[0], input().Point(i)[1], input().Point(i)[2]);        vertex_handles[i-1] = viennagrid::make_vertex( output.mesh, vgrid_point );
      }


      for (int i = 0; i < num_tets; ++i)
      {
          ::netgen::ElementIndex ei = i;
          ViennaGridSegmentType segment = output.segmentation.get_make_segment( input()[ei].GetIndex() );
          viennagrid::make_tetrahedron( segment,
                                        vertex_handles[input()[ei][0]-1],
                                        vertex_handles[input()[ei][1]-1],                                        vertex_handles[input()[ei][2]-1],
                                        vertex_handles[input()[ei][3]-1]);
      }

      return true;
    }


  }


  template<>
  struct type_information< netgen::output_mesh >
  {
    typedef netgen::output_mesh SelfType;

    static void init()
    {
      typedef viennagrid::tetrahedral_3d_mesh Tetrahedral3DViennaGridMeshType;
      typedef viennagrid::segmented_mesh<viennagrid::tetrahedral_3d_mesh, viennagrid::tetrahedral_3d_segmentation> SegmentedTetrahedral3DViennaGridMeshType;

      typedef viennagrid::thin_tetrahedral_3d_mesh ThinTetrahedral3DViennaGridMeshType;
      typedef viennagrid::segmented_mesh<viennagrid::thin_tetrahedral_3d_mesh, viennagrid::thin_tetrahedral_3d_segmentation> ThinSegmentedTetrahedral3DViennaGridMeshType;

      typedef viennagrid::segmented_mesh<viennagrid::thin_tetrahedral_3d_mesh, viennagrid::thin_cell_only_tetrahedral_3d_segmentation> ThinCellOnlySegmentedTetrahedral3DViennaGridMeshType;


      converter::get().register_conversion<SelfType, Tetrahedral3DViennaGridMeshType>( &netgen::convert );
      converter::get().register_conversion<SelfType, SegmentedTetrahedral3DViennaGridMeshType>( &netgen::convert );

      converter::get().register_conversion<SelfType, ThinTetrahedral3DViennaGridMeshType>( &netgen::convert );
      converter::get().register_conversion<SelfType, ThinSegmentedTetrahedral3DViennaGridMeshType>( &netgen::convert );
      converter::get().register_conversion<SelfType, ThinCellOnlySegmentedTetrahedral3DViennaGridMeshType>( &netgen::convert );

      type_properties::get().set_property<SelfType>( "is_mesh", "true" );
      type_properties::get().set_property<SelfType>( "is_segmented", "true" );
      type_properties::get().set_property<SelfType>( "geometric_dimension", "3" );
      type_properties::get().set_property<SelfType>( "cell_type", "3-simplex" );
    }

    static string name()
    {
      return "netgen::output_mesh";
    }
  };


}

#endif
