#ifndef VIENNAMESH_ALGORITHM_EXTRACT_HULL_HPP
#define VIENNAMESH_ALGORITHM_EXTRACT_HULL_HPP

#include "viennagrid/mesh/segmentation.hpp"
#include "viennagrid/algorithm/boundary.hpp"

#include "viennamesh/utils/utils.hpp"



namespace viennamesh
{
  template<typename HullTypeOrTagT, typename VolumeMeshT, typename VolumeSegmentationT, typename HullMeshT, typename HullSegmentationT>
  void extract_hull( VolumeMeshT const & volume_mesh,
                      VolumeSegmentationT const & volume_segmentation,
                      HullMeshT & hull_mesh,
                      HullSegmentationT & hull_segmentation )
  {
    typedef typename viennagrid::result_of::segment_handle<VolumeSegmentationT>::type    VolumeSegmentHandleType;
    typedef typename viennagrid::result_of::point<VolumeMeshT>::type            VolumePointType;

    typedef typename viennagrid::result_of::segment_handle<HullSegmentationT>::type      HullSegmentHandleType;

    for (typename VolumeSegmentationT::const_iterator sit = volume_segmentation.begin(); sit != volume_segmentation.end(); ++sit)
    {
      VolumeSegmentHandleType const & volume_segment = *sit;
      HullSegmentHandleType & hull_segment = hull_segmentation( volume_segment.id() );

      typedef typename viennagrid::result_of::const_element_range<VolumeSegmentHandleType, HullTypeOrTagT>::type    HullRangeType;
      typedef typename viennagrid::result_of::iterator<HullRangeType>::type                                   HullRangeIterator;

      typedef typename viennagrid::result_of::element<VolumeSegmentHandleType, HullTypeOrTagT>::type    VolumeHullElement;
      typedef typename viennagrid::result_of::element<HullSegmentHandleType, HullTypeOrTagT>::type      HullHullElement;

      HullRangeType hull_elements = viennagrid::elements( volume_segment );
      for (HullRangeIterator hit = hull_elements.begin(); hit != hull_elements.end(); ++hit)
      {
        VolumeHullElement const & hull_element = *hit;

        if ( viennagrid::is_boundary( volume_segment, hull_element ) )
        {
          typedef typename viennagrid::result_of::vertex_handle<HullSegmentHandleType>::type HullVertexHandleType;

          std::vector<HullVertexHandleType> vertices;
          vertices.resize( viennagrid::vertices(hull_element).size() );

          for (int i = 0; i < viennagrid::vertices(hull_element).size(); ++i)
            vertices[i] = viennagrid::make_unique_vertex( hull_mesh, viennagrid::point(volume_segment, viennagrid::vertices(hull_element)[i]) );

          viennagrid::make_element<HullHullElement>( hull_segment, vertices.begin(), vertices.end() );
        }
      }
    }
  }

  template<typename VolumeMeshT, typename MarkedAccessorT, typename HullMeshT>
  void extract_hull(VolumeMeshT const & volume_mesh, MarkedAccessorT const marked_cells, HullMeshT & hull_mesh)
  {
    typedef typename viennagrid::result_of::cell_tag<VolumeMeshT>::type CellTag;
    typedef typename viennagrid::result_of::cell<VolumeMeshT>::type CellType;

    typedef typename viennagrid::result_of::vertex_handle<HullMeshT>::type HullVertexHandleType;

    typedef typename viennagrid::result_of::const_triangle_range<VolumeMeshT>::type TriangleRangeType;
    typedef typename viennagrid::result_of::iterator<TriangleRangeType>::type TriangleRangeIterator;

    TriangleRangeType hull_triangles = viennagrid::elements(volume_mesh);
    for (TriangleRangeIterator tit = hull_triangles.begin(); tit != hull_triangles.end(); ++tit)
    {
      typedef typename viennagrid::result_of::const_coboundary_range<VolumeMeshT, viennagrid::triangle_tag, CellTag>::type CoboundaryCellRange;

      bool use = false;

      CoboundaryCellRange coboundary_cells = viennagrid::coboundary_elements<viennagrid::triangle_tag, CellTag>( volume_mesh, tit.handle() );
      if ( coboundary_cells.size() == 1)
      {
        use = marked_cells( coboundary_cells[0] );
      }
      else if ( coboundary_cells.size() == 2)
      {
        use = marked_cells( coboundary_cells[0] ) ^ marked_cells( coboundary_cells[1] );
      }
      else
        std::cout << "Something went torribly wrong..." << std::endl;


      if ( use )
        viennagrid::copy_element( *tit, hull_mesh );
    }
  }



  template<typename SegmentationT, typename SeedPointContainerT>
  void extract_seed_points( SegmentationT const & segmentation, SeedPointContainerT & seed_points )
  {
    typedef typename viennagrid::result_of::cell_tag<SegmentationT>::type CellTag;
    typedef typename viennagrid::result_of::point<SegmentationT>::type point_type;

    for (typename SegmentationT::const_iterator it = segmentation.begin(); it != segmentation.end(); ++it)
    {
      point_type centroid = viennagrid::centroid( viennagrid::elements<CellTag>(*it)[0] );
      seed_points.push_back( std::make_pair(it->id(), centroid) );
    }
  }

}

#endif
