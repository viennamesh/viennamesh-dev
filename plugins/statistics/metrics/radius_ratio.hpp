#ifndef VIENNAMESH_STATISTICS_METRICS_RADIUS_RATIO_HPP
#define VIENNAMESH_STATISTICS_METRICS_RADIUS_RATIO_HPP

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

#include "viennagrid/algorithm/volume.hpp"
#include "viennagrid/algorithm/surface.hpp"
#include "viennagrid/algorithm/circumcenter.hpp"
#include "viennagrid/algorithm/distance.hpp"
#include "forwards.hpp"

namespace viennamesh
{
    namespace metrics
    {

        /*
         * Exradius (circumcircle)
         * ----------------------   * normalization factor
         * Inradius (in-circle)
         *
        */

        template<typename ElementT, typename NumericLimitsT>
        typename viennagrid::result_of::coord<ElementT>::type radius_ratio_impl(ElementT const & element, NumericLimitsT numeric_limits, viennagrid::triangle_tag)
        {
            typedef typename viennagrid::result_of::point<ElementT>::type  PointType;
            typedef typename viennagrid::result_of::coord<PointType>::type NumericType;

            PointType const & p0 = viennagrid::get_point( viennagrid::vertices(element)[0] );
            PointType const & p1 = viennagrid::get_point( viennagrid::vertices(element)[1] );
            PointType const & p2 = viennagrid::get_point( viennagrid::vertices(element)[2] );

            NumericType area = viennagrid::volume(element);

            PointType circumcenter = viennagrid::circumcenter(element);

            //TODO check if viennagrid::surface(element)/2 yields the same as p
            NumericType a = viennagrid::distance(p0, p1);
            NumericType b = viennagrid::distance(p0, p2);
            NumericType c = viennagrid::distance(p1, p2);

            NumericType p = (a+b+c)/2; //half perimeter


            //ex-radius
            NumericType R = viennagrid::distance(circumcenter,p0);

            //in-radius
            NumericType r = area/p;


            if (std::abs(R/(2*r)) < numeric_limits.epsilon())
                return numeric_limits.max();

            return R/(2*r) ;
        }


        template<typename ElementT, typename NumericLimitsT>
        typename viennagrid::result_of::coord<ElementT>::type radius_ratio_impl(ElementT const & element, NumericLimitsT, viennagrid::tetrahedron_tag)
        {
            typedef typename viennagrid::result_of::point<ElementT>::type  PointType;
            typedef typename viennagrid::result_of::coord<PointType>::type NumericType;

            PointType const & p0 = viennagrid::get_point( viennagrid::vertices(element)[0] );
            PointType const & p1 = viennagrid::get_point( viennagrid::vertices(element)[1] );
            PointType const & p2 = viennagrid::get_point( viennagrid::vertices(element)[2] );
            PointType const & p3 = viennagrid::get_point( viennagrid::vertices(element)[3] );

            PointType l0 = p1-p0;
            PointType l2 = p0-p2;
            PointType l3 = p3-p0;

            NumericType volume = viennagrid::volume(element);
            NumericType area = viennagrid::surface(element);
            NumericType r = 3 * volume / area;

            PointType circumcenter = viennagrid::circumcenter(element);
            NumericType R = viennagrid::distance(circumcenter,p0);

            //TODO check if this ratio == 1 if tetrahedron  is regular
            return R / (3 * r);
        }
    }


    struct radius_ratio_tag {};

    namespace result_of
    {
        template<>
        struct metric_ordering_tag<radius_ratio_tag>
        {
            typedef lower_is_better_tag type;
        };
    }

    template<typename ElementT, typename NumericLimitsT>
    typename viennagrid::result_of::coord<ElementT>::type radius_ratio( ElementT const & element, NumericLimitsT numeric_limits )
    {

        if ( element.tag().is_triangle() )
            return metrics::radius_ratio_impl(element, numeric_limits, viennagrid::triangle_tag());
        else if ( element.tag().is_tetrahedron() )
            return metrics::radius_ratio_impl(element, numeric_limits, viennagrid::tetrahedron_tag());

        assert(false);
        return 0;
    }

    template<typename ElementT>
    typename viennagrid::result_of::coord<ElementT>::type radius_ratio( ElementT const & element )
    {
        return radius_ratio(element, std::numeric_limits< typename viennagrid::result_of::coord<ElementT>::type >() );
    }

    namespace detail
    {
        template<typename ElementT, typename NumericLimitsT>
        typename viennagrid::result_of::coord<ElementT>::type metric( ElementT const & element, NumericLimitsT numeric_limits, radius_ratio_tag)
        {
            return radius_ratio(element, numeric_limits);
        }

        template<typename ElementT>
        typename viennagrid::result_of::coord<ElementT>::type metric(ElementT const & element, radius_ratio_tag)
        {
            return radius_ratio(element);
        }
    }

}


#endif
