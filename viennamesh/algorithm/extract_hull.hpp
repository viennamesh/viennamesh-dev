#ifndef VIENNAMESH_ALGORITHM_EXTRACT_HULL_HPP
#define VIENNAMESH_ALGORITHM_EXTRACT_HULL_HPP

#include "viennagrid/domain/segmentation.hpp"

#include "viennamesh/utils/utils.hpp"



namespace viennamesh
{
    template<typename HullTypeOrTagT, typename VolumeDomainT, typename VolumeSegmentationT, typename HullDomainT, typename HullSegmentationT>
    void extract_hull( VolumeDomainT const & volume_domain,
                       VolumeSegmentationT const & volume_segmentation,
                       HullDomainT & hull_domain,
                       HullSegmentationT & hull_segmentation )
    {
        typedef typename viennagrid::result_of::segment<VolumeSegmentationT>::type    VolumeSegmentType;
        typedef typename viennagrid::result_of::point<VolumeDomainT>::type            VolumePointType;
        
        typedef typename viennagrid::result_of::segment<HullSegmentationT>::type      HullSegmentType;

        for (typename VolumeSegmentationT::const_iterator sit = volume_segmentation.begin(); sit != volume_segmentation.end(); ++sit)
        {
            VolumeSegmentType const & volume_segment = *sit;
            HullSegmentType & hull_segment = hull_segmentation( volume_segment.id() );
            
            typedef typename viennagrid::result_of::const_element_range<VolumeSegmentType, HullTypeOrTagT>::type    HullRangeType;
            typedef typename viennagrid::result_of::iterator<HullRangeType>::type                                   HullRangeIterator;
            
            typedef typename viennagrid::result_of::element<VolumeSegmentType, HullTypeOrTagT>::type    VolumeHullElement;
            typedef typename viennagrid::result_of::element<HullSegmentType, HullTypeOrTagT>::type      HullHullElement;
            
            HullRangeType hull_elements = viennagrid::elements( volume_segment );
            for (HullRangeIterator hit = hull_elements.begin(); hit != hull_elements.end(); ++hit)
            {
                VolumeHullElement const & hull_element = *hit;
                
                if ( viennagrid::is_boundary( volume_segment, hull_element ) )
                {
                  typedef typename viennagrid::result_of::vertex_handle<HullSegmentType>::type HullVertexHandleType;

                  std::vector<HullVertexHandleType> vertices;
                  vertices.resize( viennagrid::vertices(hull_element).size() );

                  for (int i = 0; i < viennagrid::vertices(hull_element).size(); ++i)
                    vertices[i] = viennagrid::make_unique_vertex( hull_domain, viennagrid::point(volume_segment, viennagrid::vertices(hull_element)[i]) );

                  viennagrid::make_element<HullHullElement>( hull_segment, vertices.begin(), vertices.end() );
                }
            }
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