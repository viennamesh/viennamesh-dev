#ifndef VIENNAMESH_ALGORITHM_NETGEN_MESH_HPP
#define VIENNAMESH_ALGORITHM_NETGEN_MESH_HPP

/* ============================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                ViennaMesh - The Vienna Meshing Framework
                            -----------------

                    http://viennamesh.sourceforge.net/

   License:         MIT (X11), see file LICENSE in the base directory
=============================================================================== */

#ifdef VIENNAMESH_WITH_NETGEN

#include "netgen/libsrc/csg/csg.hpp"
namespace netgen
{
  extern CSGeometry * ParseCSG (istream & istr);
}

namespace viennamesh
{
  namespace netgen
  {
    struct mesh
    {
    public:
      mesh() : m( new InternalMeshType() ) {}
      ~mesh() { delete m; }

      typedef ::netgen::Mesh InternalMeshType;

      InternalMeshType & operator()() { return *m; }
      InternalMeshType const & operator()() const { return *m; }

      InternalMeshType * & mesh_ptr() { return m; }
      InternalMeshType const * mesh_ptr() const { return m; }

      mesh & operator=( mesh const & other )
      {
        *m = *other.m;
        return *this;
      }

    private:
      InternalMeshType * m;
    };

    inline std::ostream & operator<<( std::ostream & os, mesh const & m )
    {
      os << "netgen-mesh";
      return os;
    }


    template<typename MeshT, typename SegmentationT>
    bool convert(viennagrid::segmented_mesh<MeshT, SegmentationT> const & input, netgen::mesh & output)
    {
      typedef typename viennagrid::result_of::point<MeshT>::type PointType;
      typedef typename SegmentationT::segment_handle_type SegmentType;

      typedef typename viennagrid::result_of::vertex_id<MeshT>::type VertexIDType;
      typedef typename viennagrid::result_of::vertex_handle<MeshT>::type VertexHandleType;
      typedef typename viennagrid::result_of::triangle<MeshT>::type TriangleType;

      typedef typename viennagrid::result_of::const_vertex_range<MeshT>::type VertexRangeType;
      typedef typename viennagrid::result_of::iterator<VertexRangeType>::type VertexIteratorType;


      typedef typename viennagrid::result_of::const_triangle_range<MeshT>::type TriangleRangeType;
      typedef typename viennagrid::result_of::iterator<TriangleRangeType>::type TriangleIteratorType;


      for (typename SegmentationT::const_iterator sit = input.segmentation.begin(); sit != input.segmentation.end(); ++sit)
      {
        int netgen_sid = (*sit).id() + 1;
        output().AddFaceDescriptor( (::netgen::FaceDescriptor (netgen_sid, netgen_sid, 0, netgen_sid)) );
      }

      std::map<VertexIDType, int> vertex_index_map;
      VertexRangeType vertices(input.mesh);
      for (VertexIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit)
      {
        PointType const & p = viennagrid::point(*vit);
        vertex_index_map[ (*vit).id() ] = output().AddPoint( ::netgen::Point3d( p[0], p[1], p[2] ) );
      }

      TriangleRangeType triangles(input.mesh);
      for (TriangleIteratorType tit = triangles.begin(); tit != triangles.end(); ++tit)
      {
        typedef typename viennagrid::result_of::segment_id_range<SegmentationT, TriangleType>::type SegmentIDType;

        int indices[3];
        for (int i = 0; i < 3; ++i)
          indices[i] = vertex_index_map[ viennagrid::vertices(*tit)[i].id() ];

        SegmentIDType segment_ids = viennagrid::segment_ids(input.segmentation, *tit);
        for (typename SegmentIDType::iterator sit = segment_ids.begin(); sit != segment_ids.end(); ++sit)
        {
          bool faces_outward = true;
          bool const * faces_outward_on_segment_pointer = viennagrid::segment_element_info(input.segmentation(*sit), *tit);
          if (faces_outward_on_segment_pointer)
            faces_outward = *faces_outward_on_segment_pointer;

          if (!faces_outward)
            std::swap( indices[1], indices[2] );

          ::netgen::Element2d el(3);
          el.SetIndex ( *sit+1 );
          el.PNum(1) = indices[0];
          el.PNum(2) = indices[1];
          el.PNum(3) = indices[2];
          output().AddSurfaceElement (el);
        }
      }

      return true;
    }








    template<typename MeshT>
    bool convert(netgen::mesh const & input, MeshT & output)
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
    bool convert(netgen::mesh const & input, viennagrid::segmented_mesh<MeshT, SegmentationT> & output)
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
  struct type_information< netgen::mesh >
  {
    typedef netgen::mesh SelfType;

    static void init()
    {
      typedef viennagrid::segmented_mesh<viennagrid::triangular_3d_mesh, viennagrid::triangular_hull_3d_segmentation> SegmentedTriangular3DViennaGridMeshType;

      converter::get().register_conversion<SegmentedTriangular3DViennaGridMeshType, SelfType>( &netgen::convert );



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

    static std::string name()
    {
      return "netgen::mesh";
    }
  };


}

#endif

#endif
