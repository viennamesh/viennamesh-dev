#ifndef VIENNAMESH_STATISTICS_ELEMENT_METRICS_HPP
#define VIENNAMESH_STATISTICS_ELEMENT_METRICS_HPP


#include "viennamesh/statistics/metrics/radius_edge_ratio.hpp"
#include "viennamesh/statistics/metrics/aspect_ratio.hpp"
#include "viennamesh/statistics/metrics/condition_number.hpp"
#include "viennamesh/statistics/metrics/min_angle.hpp"
#include "viennamesh/statistics/metrics/min_dihedral_angle.hpp"


namespace viennamesh
{
  struct lower_is_better_tag;
  struct higher_is_better_tag;

  namespace result_of
  {
    template<typename MetricTagT>
    struct metric_ordering_tag;
  }


  template<typename MetricTagT, typename PointAccessorT, typename ElementT, typename NumericLimitsT>
  typename viennagrid::result_of::coord<typename PointAccessorT::value_type>::type metric( PointAccessorT const point_accessor, ElementT const & element, NumericLimitsT numeric_limits)
  {
    return detail::metric( point_accessor, element, numeric_limits, MetricTagT() );
  }


  template<typename MetricTagT, typename PointAccessorT, typename ElementT>
  typename viennagrid::result_of::coord<typename PointAccessorT::value_type>::type metric( PointAccessorT const point_accessor, ElementT const & element )
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
  { return metric<MetricTagT>(element0) < metric<MetricTagT>(element1); }


  template<typename OrderingTagT>
  struct worst_element_impl;

  template<>
  struct worst_element_impl<lower_is_better_tag>
  {
    template<typename IteratorT, typename Compare>
    static IteratorT find(IteratorT const & begin, IteratorT const & end, Compare comp)
    { return std::max_element( begin, end, comp ); }
  };

  template<>
  struct worst_element_impl<higher_is_better_tag>
  {
    template<typename IteratorT, typename Compare>
    static IteratorT find(IteratorT const & begin, IteratorT const & end, Compare comp)
    { return std::min_element( begin, end, comp ); }
  };


  template<typename MetricTagT, typename MeshT>
  typename viennagrid::result_of::iterator<
    typename viennagrid::result_of::const_cell_range<MeshT>::type
  >::type worst_element( MeshT const & mesh )
  {
    typedef typename viennagrid::result_of::cell<MeshT>::type CellType;
    typedef typename viennamesh::result_of::metric_ordering_tag<MetricTagT>::type MetricOrderingTag;
    typedef typename viennagrid::result_of::const_cell_range<MeshT>::type CellRangeType;

    CellRangeType cells = viennagrid::elements(mesh);
    return worst_element_impl<MetricOrderingTag>::find( cells.begin(), cells.end(), viennamesh::metric_compare<MetricTagT, CellType> );
  }
}

#endif
