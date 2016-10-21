#ifndef VIENNAMESH_STATISTICS_METRICS_STRETCH_HPP
#define VIENNAMESH_STATISTICS_METRICS_STRETCH_HPP

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
          * Circumradius
          * --------------------  * normalization factor
          * Longest edge length
          *
          * PROBLEMATIC QUALITY MEASURE: Equilateral: 1, Bad Triangles for > 1 AND < 1
         */
        template<typename ElementT, typename NumericLimitsT>
        typename viennagrid::result_of::coord<ElementT>::type stretch_impl(ElementT const & element, NumericLimitsT numeric_limits, viennagrid::triangle_tag)
        {
            typedef typename viennagrid::result_of::point<ElementT>::type  PointType;
            typedef typename viennagrid::result_of::coord<PointType>::type NumericType;

            PointType const & p0 = viennagrid::get_point( viennagrid::vertices(element)[0] );
            PointType const & p1 = viennagrid::get_point( viennagrid::vertices(element)[1] );
            PointType const & p2 = viennagrid::get_point( viennagrid::vertices(element)[2] );

            PointType circumcenter = viennagrid::circumcenter(element);


            NumericType a = viennagrid::distance(p0, p1);
            NumericType b = viennagrid::distance(p0, p2);
            NumericType c = viennagrid::distance(p1, p2);

            NumericType longest = std::max(a, std::max(a,c));
            NumericType R = viennagrid::distance(circumcenter,p0);





            if(std::abs(longest) < numeric_limits.epsilon())
                return numeric_limits.max();
            else
                return  R * std::sqrt(12) / (longest * 2);

        }

        // for tetrahedrons currently not implemented (maybe max facet triangle surface / min facet triangle surface
        // template<typename ElementT, typename NumericLimitsT>
        // typename viennagrid::result_of::coord<ElementT>::type stretch_impl(ElementT const & element, NumericLimitsT, viennagrid::tetrahedron_tag)
        // {}
    }


    struct stretch_tag {};

    namespace result_of
    {
        template<>
        struct metric_ordering_tag<stretch_tag>
        {
            typedef lower_is_better_tag type;
        };
    }

    template<typename ElementT, typename NumericLimitsT>
    typename viennagrid::result_of::coord<ElementT>::type stretch( ElementT const & element, NumericLimitsT numeric_limits )
    {

        if ( element.tag().is_triangle() )
            return metrics::stretch_impl(element, numeric_limits, viennagrid::triangle_tag());


        assert(false);
        return 0;
    }

    template<typename ElementT>
    typename viennagrid::result_of::coord<ElementT>::type stretch( ElementT const & element )
    {
        return stretch(element, std::numeric_limits< typename viennagrid::result_of::coord<ElementT>::type >() );
    }

    namespace detail
    {
        template<typename ElementT, typename NumericLimitsT>
        typename viennagrid::result_of::coord<ElementT>::type metric( ElementT const & element, NumericLimitsT numeric_limits, stretch_tag)
        {
            return stretch(element, numeric_limits);
        }

        template<typename ElementT>
        typename viennagrid::result_of::coord<ElementT>::type metric(ElementT const & element, stretch_tag)
        {
            return stretch(element);
        }
    }

}


#endif
