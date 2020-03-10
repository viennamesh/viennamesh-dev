#ifndef VIENNAMESH_STATISTICS_METRICS_SKEWNESS_HPP
#define VIENNAMESH_STATISTICS_METRICS_SKEWNESS_HPP

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

#include "max_angle.hpp"
#include "min_angle.hpp"
#include "forwards.hpp"


namespace viennamesh
{
    namespace metrics
    {

        /*
        *Equiangle skewness
        *
        * range  [0, 1]
        * 0 = equilateral triangle
        */


        template<typename ElementT, typename NumericLimitsT>
        typename viennagrid::result_of::coord<ElementT>::type skewness_impl(ElementT const & element, NumericLimitsT, viennagrid::triangle_tag)
        {
            typedef typename viennagrid::result_of::point<ElementT>::type  PointType;
            typedef typename viennagrid::result_of::coord<PointType>::type NumericType;

            NumericType max_angle_ = max_angle(element);
            NumericType min_angle_ = min_angle(element);
            NumericType equi_angle_ = M_PI/3;

            return std::max((max_angle_ - equi_angle_)/(2 * M_PI/3), (equi_angle_ - min_angle_)/equi_angle_);

        }

        // for tetrahedrons currently not implemented (maybe max facet triangle surface / min facet triangle surface
        // template<typename ElementT, typename NumericLimitsT>
        // typename viennagrid::result_of::coord<ElementT>::type skewness_impl(ElementT const & element, NumericLimitsT, viennagrid::tetrahedron_tag)
        // {}
    }


    struct skewness_tag {};

    namespace result_of
    {
        template<>
        struct metric_ordering_tag<skewness_tag>
        {
            typedef lower_is_better_tag type;
        };
    }

    template<typename ElementT, typename NumericLimitsT>
    typename viennagrid::result_of::coord<ElementT>::type skewness( ElementT const & element, NumericLimitsT numeric_limits )
    {

        if ( element.tag().is_triangle() )
            return metrics::skewness_impl(element, numeric_limits, viennagrid::triangle_tag());


        assert(false);
        return 0;
    }

    template<typename ElementT>
    typename viennagrid::result_of::coord<ElementT>::type skewness( ElementT const & element )
    {
        return skewness(element, std::numeric_limits< typename viennagrid::result_of::coord<ElementT>::type >() );
    }

    namespace detail
    {
        template<typename ElementT, typename NumericLimitsT>
        typename viennagrid::result_of::coord<ElementT>::type metric( ElementT const & element, NumericLimitsT numeric_limits, skewness_tag)
        {
            return skewness(element, numeric_limits);
        }

        template<typename ElementT>
        typename viennagrid::result_of::coord<ElementT>::type metric(ElementT const & element, skewness_tag)
        {
            return skewness(element);
        }
    }

}


#endif
