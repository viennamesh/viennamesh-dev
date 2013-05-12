#ifndef VIENNAMESH_ALGORITHM_EXTRACT_HULL_HPP
#define VIENNAMESH_ALGORITHM_EXTRACT_HULL_HPP

#include "viennagrid/domain/segmentation.hpp"

#include "viennamesh/utils/utils.hpp"



namespace viennamesh
{
    template<typename volume_domain_type, typename volume_segment_container_type, typename hull_domain_type, typename hull_segment_container_type>
    void extract_hull( volume_domain_type const & volume_domain, volume_segment_container_type const & volume_segments,
                    hull_domain_type & hull_domain, hull_segment_container_type & hull_segments )
    {
        typedef typename volume_segment_container_type::value_type volume_segment_type;
        typedef typename viennagrid::result_of::domain_view<volume_domain_type>::type volume_view_type;
        typedef typename viennagrid::result_of::point_type<volume_domain_type>::type volume_point_type;
        typedef typename viennagrid::result_of::cell_type<volume_domain_type>::type volume_cell_type;
        
        typedef typename hull_segment_container_type::value_type hull_segment_type;

        hull_segments.resize( volume_segments.size() );
        
        for (int seg = 0; seg < volume_segments.size(); ++seg)
        {
            volume_view_type const & volume_segment = volume_segments[seg];

            hull_segment_type & hull_segment = hull_segments[seg];
            hull_segment = viennagrid::create_view<hull_segment_type>( hull_domain );
            
            typedef typename viennagrid::result_of::const_triangle_range<volume_view_type>::type triangle_range_type;
            typedef typename viennagrid::result_of::iterator<triangle_range_type>::type triangle_range_iterator;
            typedef typename viennagrid::result_of::triangle<volume_view_type>::type triangle_type;
            
            triangle_range_type triangles = viennagrid::elements( volume_segment );
            for (triangle_range_iterator it = triangles.begin(); it != triangles.end(); ++it)
            {
                triangle_type const & triangle = *it;
                
                if ( viennagrid::is_boundary<volume_cell_type>( triangle, volume_segment ) )
                {
                    viennagrid::create_triangle(
                        hull_segment,
                        viennagrid::create_unique_vertex( hull_domain, viennagrid::point(volume_segment, viennagrid::vertices(triangle)[0]) ),
                        viennagrid::create_unique_vertex( hull_domain, viennagrid::point(volume_segment, viennagrid::vertices(triangle)[1]) ),
                        viennagrid::create_unique_vertex( hull_domain, viennagrid::point(volume_segment, viennagrid::vertices(triangle)[2]) )
                    );
                }
            }
        }
    }
}

#endif