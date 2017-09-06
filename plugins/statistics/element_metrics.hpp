#ifndef VIENNAMESH_STATISTICS_ELEMENT_METRICS_HPP
#define VIENNAMESH_STATISTICS_ELEMENT_METRICS_HPP

/* ============================================================================
   Copyright (c) 2011-2016, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                ViennaMesh - The Vienna Meshing Framework
                            -----------------

                    http://viennamesh.sourceforge.net/

   License:         MIT (X11), see file LICENSE in the base directory
=============================================================================== */

#include "metrics/radius_edge_ratio.hpp"
#include "metrics/aspect_ratio.hpp"
#include "metrics/min_angle.hpp"
#include "metrics/max_angle.hpp"
#include "metrics/min_dihedral_angle.hpp"
#include "metrics/radius_ratio.hpp"
#include "metrics/edge_ratio.hpp"
#include "metrics/circum_perimeter_ratio.hpp"
#include "metrics/stretch.hpp"
#include "metrics/skewness.hpp"
#include "metrics/perimeter_inradius_ratio.hpp"

namespace viennamesh
{
    template<typename MetricTagT, typename PointAccessorT, typename ElementT, typename NumericLimitsT>
    typename viennagrid::result_of::coord<typename PointAccessorT::point_type>::type metric( PointAccessorT const point_accessor, ElementT const & element, NumericLimitsT numeric_limits)
    {
        return detail::metric( point_accessor, element, numeric_limits, MetricTagT() );
    }


    template<typename MetricTagT, typename PointAccessorT, typename ElementT>
    typename viennagrid::result_of::coord<typename PointAccessorT::point_type>::type metric( PointAccessorT const point_accessor, ElementT const & element )
    {
        return detail::metric( point_accessor, element, MetricTagT() );
    }

    template<typename MetricTagT, typename ElementT>
    typename viennagrid::result_of::coord< ElementT >::type metric( ElementT const & element )
    {
        return detail::metric(element, MetricTagT());
    }





    template<typename MetricTagT, typename ElementT>
    bool metric_compare( ElementT const & element0, ElementT const & element1 )
    {
        return metric<MetricTagT>(element0) < metric<MetricTagT>(element1);
    }


    template<typename OrderingTagT>
    struct worst_element_impl;

    template<>
    struct worst_element_impl<lower_is_better_tag>
    {
        template<typename IteratorT, typename Compare>
        static IteratorT find(IteratorT const & begin, IteratorT const & end, Compare comp)
        {
            return std::max_element( begin, end, comp );
        }
    };

    template<>
    struct worst_element_impl<higher_is_better_tag>
    {
        template<typename IteratorT, typename Compare>
        static IteratorT find(IteratorT const & begin, IteratorT const & end, Compare comp)
        {
            return std::min_element( begin, end, comp );
        }
    };


    template<typename MetricTagT, typename MeshT>
    typename viennagrid::result_of::iterator<
    typename viennagrid::result_of::const_cell_range<MeshT>::type
    >::type worst_element( MeshT const & mesh )
    {
        typedef typename viennagrid::result_of::element<MeshT>::type CellType;
        typedef typename viennamesh::result_of::metric_ordering_tag<MetricTagT>::type MetricOrderingTag;
        typedef typename viennagrid::result_of::const_cell_range<MeshT>::type CellRangeType;

        CellRangeType cells(mesh);
        return worst_element_impl<MetricOrderingTag>::find( cells.begin(), cells.end(), viennamesh::metric_compare<MetricTagT, CellType> );
    }


    //Implementation of automatic deducation of comparison sign (< or >) based an the OrderingTagT
    template<typename OrderingTagT>
    struct is_good_element_impl;

    template<>
    struct is_good_element_impl<lower_is_better_tag>
    {
        template<typename NumericT>
        static bool check(NumericT metric_value, NumericT threshold)
        {
            return metric_value < threshold;
        }
    };

    template<>
    struct is_good_element_impl<higher_is_better_tag>
    {
        template<typename NumericT>
        static bool check(NumericT metric_value, NumericT threshold)
        {
            return metric_value > threshold;
        }
    };



    template<typename MetricTagT, typename NumericT>
    bool is_good_element(NumericT metric_value, NumericT threshold)
    {
        typedef typename viennamesh::result_of::metric_ordering_tag<MetricTagT>::type MetricOrderingTag;

        return is_good_element_impl<MetricOrderingTag>::check(metric_value, threshold);
    }
}

#endif
