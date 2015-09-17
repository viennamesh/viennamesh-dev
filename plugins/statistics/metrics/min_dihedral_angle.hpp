#ifndef VIENNAMESH_STATISTICS_METRICS_MIN_DIHEDRAL_ANGLE_HPP
#define VIENNAMESH_STATISTICS_METRICS_MIN_DIHEDRAL_ANGLE_HPP

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

#include "viennagrid/algorithm/angle.hpp"
#include "forwards.hpp"

namespace viennamesh
{
  namespace metrics
  {
    template<typename PointAccessorT, typename ElementT, typename NumericLimitsT>
    typename viennagrid::result_of::coord<typename PointAccessorT::point_type>::type min_dihedral_angle_impl(PointAccessorT const point_accessor, ElementT const & element, NumericLimitsT, viennagrid::tetrahedron_tag)
    {
      typedef typename PointAccessorT::point_type            PointType;

      PointType const & p0 = point_accessor.get( viennagrid::vertices(element)[0] );
      PointType const & p1 = point_accessor.get( viennagrid::vertices(element)[1] );
      PointType const & p2 = point_accessor.get( viennagrid::vertices(element)[2] );
      PointType const & p3 = point_accessor.get( viennagrid::vertices(element)[3] );

      double da_01 = viennagrid::dihedral_angle( p0, p1, p2, p0, p1, p3 );
      double da_02 = viennagrid::dihedral_angle( p0, p2, p1, p0, p2, p3 );
      double da_03 = viennagrid::dihedral_angle( p0, p3, p1, p0, p3, p2 );
      double da_12 = viennagrid::dihedral_angle( p1, p2, p0, p1, p2, p3 );
      double da_13 = viennagrid::dihedral_angle( p1, p3, p0, p1, p3, p2 );
      double da_23 = viennagrid::dihedral_angle( p2, p3, p0, p2, p3, p1 );

      return std::min( std::min( std::min( da_01, da_02 ), std::min(da_03, da_12) ), std::min(da_13, da_23) );
    }

    template<typename PointAccessorT, typename ElementT, typename NumericLimitsT, typename TagT>
    typename viennagrid::result_of::coord<typename PointAccessorT::point_type>::type min_dihedral_angle_impl(PointAccessorT const, ElementT const &, NumericLimitsT, TagT)
    {
      throw metric_not_implemented_or_supported_exception( "min dihedral angle not implemented for " + TagT::name() );
    }
  }


  struct min_dihedral_angle_tag {};

  template<typename PointAccessorT, typename ElementT, typename NumericLimitsT>
  typename viennagrid::result_of::coord<typename PointAccessorT::point_type>::type min_dihedral_angle( PointAccessorT const point_accessor, ElementT const & element, NumericLimitsT numeric_limits )
  {
    if ( element.tag().is_tetrahedron() )
      return metrics::min_dihedral_angle_impl(point_accessor, element, numeric_limits, viennagrid::tetrahedron_tag());

    assert(false);
    return 0;
  }

  template<typename PointAccessorT, typename ElementT>
  typename viennagrid::result_of::coord<typename PointAccessorT::point_type>::type min_dihedral_angle( PointAccessorT const point_accessor, ElementT const & element )
  {
    return min_dihedral_angle(point_accessor, element, std::numeric_limits< typename viennagrid::result_of::coord<typename PointAccessorT::point_type>::type >() );
  }

  template<typename ElementT>
  typename viennagrid::result_of::coord< ElementT >::type min_dihedral_angle(ElementT const & element)
  {
    return min_dihedral_angle( viennagrid::root_mesh_point_accessor(), element);
  }


  namespace detail
  {
    template<typename PointAccessorT, typename ElementT, typename NumericLimitsT>
    typename viennagrid::result_of::coord<typename PointAccessorT::value_type>::type metric( PointAccessorT const point_accessor, ElementT const & element, NumericLimitsT numeric_limits, min_dihedral_angle_tag)
    {
      return min_dihedral_angle(point_accessor, element, numeric_limits);
    }

    template<typename PointAccessorT, typename ElementT>
    typename viennagrid::result_of::coord<typename PointAccessorT::value_type>::type metric( PointAccessorT const point_accessor, ElementT const & element, min_dihedral_angle_tag)
    {
      return min_dihedral_angle(point_accessor, element);
    }

    template<typename ElementT>
    typename viennagrid::result_of::coord< ElementT >::type metric( ElementT const & element, min_dihedral_angle_tag)
    {
      return min_dihedral_angle(element);
    }
  }

}


#endif
