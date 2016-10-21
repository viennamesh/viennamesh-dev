#ifndef VIENNAMESH_STATISTICS_METRICS_EDGE_RATIO_HPP
#define VIENNAMESH_STATISTICS_METRICS_EDGE_RATIO_HPP

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

#include "viennagrid/algorithm/distance.hpp"
#include "forwards.hpp"

namespace viennamesh
{
    namespace metrics
    {
        /*
          * Longest edge length
          * --------------------
          * Shortest edge length
          *
          * This quality measure tends to assign flattened triangles (slivers, one angle close to PI) a low value < 2 (i.e., flattened triangles
          * are regarded as being 'good'.
         */
        template<typename ElementT, typename NumericLimitsT>
        typename viennagrid::result_of::coord<ElementT>::type edge_ratio_impl(ElementT const & element, NumericLimitsT numeric_limits, viennagrid::triangle_tag)
        {
            typedef typename viennagrid::result_of::point<ElementT>::type  PointType;
            typedef typename viennagrid::result_of::coord<PointType>::type NumericType;

            PointType const & p0 = viennagrid::get_point( viennagrid::vertices(element)[0] );
            PointType const & p1 = viennagrid::get_point( viennagrid::vertices(element)[1] );
            PointType const & p2 = viennagrid::get_point( viennagrid::vertices(element)[2] );

            //TODO check if viennagrid::surface(element)/2 yields the same as p
            NumericType a = viennagrid::distance(p0, p1);
            NumericType b = viennagrid::distance(p0, p2);
            NumericType c = viennagrid::distance(p1, p2);

            NumericType shortest = std::min(a, std::min(a,c));
            NumericType longest = std::max(a, std::max(a,c));



            if (std::abs(shortest) < numeric_limits.epsilon())
                return numeric_limits.max();
            else
                return longest/shortest;

        }

        // for tetrahedrons currently not implemented (maybe max facet triangle surface / min facet triangle surface
        // template<typename ElementT, typename NumericLimitsT>
        // typename viennagrid::result_of::coord<ElementT>::type edge_ratio_impl(ElementT const & element, NumericLimitsT, viennagrid::tetrahedron_tag)
        // {}
    }


    struct edge_ratio_tag {};

    namespace result_of
    {
        template<>
        struct metric_ordering_tag<edge_ratio_tag>
        {
            typedef lower_is_better_tag type;
        };
    }

    template<typename ElementT, typename NumericLimitsT>
    typename viennagrid::result_of::coord<ElementT>::type edge_ratio( ElementT const & element, NumericLimitsT numeric_limits )
    {

        if ( element.tag().is_triangle() )
            return metrics::edge_ratio_impl(element, numeric_limits, viennagrid::triangle_tag());


        assert(false);
        return 0;
    }

    template<typename ElementT>
    typename viennagrid::result_of::coord<ElementT>::type edge_ratio( ElementT const & element )
    {
        return edge_ratio(element, std::numeric_limits< typename viennagrid::result_of::coord<ElementT>::type >() );
    }

    namespace detail
    {
        template<typename ElementT, typename NumericLimitsT>
        typename viennagrid::result_of::coord<ElementT>::type metric( ElementT const & element, NumericLimitsT numeric_limits, edge_ratio_tag)
        {
            return edge_ratio(element, numeric_limits);
        }

        template<typename ElementT>
        typename viennagrid::result_of::coord<ElementT>::type metric(ElementT const & element, edge_ratio_tag)
        {
            return edge_ratio(element);
        }
    }

}


#endif
