#ifndef VIENNAMESH_STATISTICS_METRICS_PERIMETER_INRADIUS_RATIO_HPP
#define VIENNAMESH_STATISTICS_METRICS_PERIMETER_INRADIUS_RATIO_HPP

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
#include "viennagrid/algorithm/distance.hpp"
#include "forwards.hpp"

namespace viennamesh
{
    namespace metrics
    {
        /*
           * Half perimeter
           * ----------------------   * normalization factor
           * In-radius (In-circle)
           *
          */

        template<typename ElementT, typename NumericLimitsT>
        typename viennagrid::result_of::coord<ElementT>::type perimeter_inradius_impl(ElementT const & element, NumericLimitsT numeric_limits)
        {
            typedef typename viennagrid::result_of::point<ElementT>::type  PointType;
            typedef typename viennagrid::result_of::coord<PointType>::type NumericType;

            PointType const & p0 = viennagrid::get_point( viennagrid::vertices(element)[0] );
            PointType const & p1 = viennagrid::get_point( viennagrid::vertices(element)[1] );
            PointType const & p2 = viennagrid::get_point( viennagrid::vertices(element)[2] );

            NumericType area = viennagrid::volume(element);

            NumericType a = viennagrid::distance(p0, p1);
            NumericType b = viennagrid::distance(p0, p2);
            NumericType c = viennagrid::distance(p1, p2);

            //half perimeter
            NumericType p = a+b+c/2;

            //in-radius
            NumericType r = area/p;



            if (r < numeric_limits.epsilon())
                return numeric_limits.max();

            //normalized, now == 1 if and only if triangle is equilateral
            return p/(r * 3* std::sqrt(3));
        }
    }


    struct perimeter_inradius_ratio_tag {};

    namespace result_of
    {
        template<>
        struct metric_ordering_tag<perimeter_inradius_ratio_tag>
        {
            typedef lower_is_better_tag type;
        };
    }


    template<typename ElementT, typename NumericLimitsT>
    typename viennagrid::result_of::coord<ElementT>::type perimeter_inradius_ratio(ElementT const & element, NumericLimitsT numeric_limits )
    {
        if(element.tag().is_triangle())
        {
            return metrics::radius_ratio_impl(element, numeric_limits);
        }

        assert(false); //perimeter_inradius_ratio only implemented for triangular element, TODO better error handling
        return 0;

    }

    template<typename ElementT>
    typename viennagrid::result_of::coord<ElementT>::type perimeter_inradius_ratio(ElementT const & element)
    {
        return perimeter_inradius_ratio( element, std::numeric_limits< typename viennagrid::result_of::coord<ElementT>::type >() );
    }

    namespace detail
    {
        template<typename ElementT, typename NumericLimitsT>
        typename viennagrid::result_of::coord<ElementT>::type metric(ElementT const & element, NumericLimitsT numeric_limits, perimeter_inradius_ratio_tag)
        {
            return perimeter_inradius_ratio(element, numeric_limits);
        }

        template<typename ElementT>
        typename viennagrid::result_of::coord< ElementT >::type metric( ElementT const & element, perimeter_inradius_ratio_tag)
        {
            return perimeter_inradius_ratio(element);
        }
    }


}


#endif
