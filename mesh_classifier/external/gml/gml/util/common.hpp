/* ============================================================================
   Copyright (c) 2008-2009 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2003-2009 Rene Heinzl                             rene@gsse.at    

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
   
   
#ifndef GML_UTIL_COMMON
#define GML_UTIL_COMMON
//
// ===================================================================================
//
// *** GML includes
//
#include "tags.hpp"
#include "size.hpp"
//
// *** GSSE includes
//
//#include <gsse/algorithm/io.hpp>
#include <gsse/util/common.hpp>
#include <gsse/topology/coboundary.hpp>
//
// *** BOOST include  
//
#include <boost/mpl/max_element.hpp>
#include <boost/mpl/transform_view.hpp>
#include <boost/mpl/sizeof.hpp>
#include <boost/mpl/lambda.hpp>
#include <boost/fusion/include/boost_tuple.hpp>
#include <boost/mpl/pair.hpp>
//
// ===================================================================================
// 
// GSSE::TRAITS - ADDONS
//
namespace gsse {
namespace traits {
// --------------------------------------------------
//
// [JW] for 32-bit hardware we have to additionally provide
// the following specializations for std::size_t
// for 64-bit hardware this is obsolete and must not be
// active code, otherwise compilation errors occure.
//
#ifdef __i386__
template<typename T1, std::size_t T2>
struct tag_container_type<gsse::array<T1, T2> >
{
   typedef gsse::tag_container_array type;
};
// --------------------------------------------------
template<typename T1, std::size_t T2>
struct tag_container_type<boost::array<T1, T2> >
{
   typedef gsse::tag_container_array type;
};
#endif
// --------------------------------------------------
template<typename T1>
struct tag_container_type< boost::fusion::vector< T1 > >
{
   typedef gml::tag_sequence_container type;  
};
template<typename T1, typename T2>
struct tag_container_type< boost::fusion::vector< T1, T2 > >
{
   typedef gml::tag_sequence_container type;  
};
template<typename T1, typename T2, typename T3>
struct tag_container_type< boost::fusion::vector< T1, T2, T3 > >
{
   typedef gml::tag_sequence_container type;  
};
template<typename T1, typename T2, typename T3, typename T4>
struct tag_container_type< boost::fusion::vector< T1, T2, T3, T4 > >
{
   typedef gml::tag_sequence_container type;  
};
// --------------------------------------------------
template<typename T1, typename T2>
struct tag_container_type< std::map< T1, T2 > >
{
   typedef gsse::tag_runtime_map type;   
};
// --------------------------------------------------
template<typename T1, typename T2>
struct tag_container_type< boost::tuples::tuple< T1, T2 > >
{
   typedef gml::tag_boost_tuples_tuple type;
};
template<typename T1, typename T2, typename T3>
struct tag_container_type< boost::tuples::tuple< T1, T2, T3 > >
{
   typedef gml::tag_boost_tuples_tuple type;
};

} // end namespace: traits
} // end namespace: gsse
//
// ===================================================================================
// 
// GSSE::RESULT_OF::VAL - ADDONS
//
namespace gsse {
namespace result_of {

template <typename Sequence>
struct val_impl< Sequence, gml::tag_sequence_container >
{
   typedef typename boost::mpl::deref<
      typename boost::mpl::max_element<
         boost::mpl::transform_view< 
            Sequence, 
            boost::mpl::sizeof_< boost::mpl::_1 > 
         >
       >::type::base
   >::type                                type;  
};

template <typename Sequence>
struct val_impl< Sequence, gml::tag_boost_tuples_tuple >
{
   // [JW] TODO .. the maximum type detection doesn't work here ..
   //
   typedef typename boost::fusion::result_of::value_at<Sequence, boost::mpl::int_<0> >::type type;   
};

} // end namespace: result_of 
} // end namespace: gsse

//
// ===================================================================================
//
namespace gml
{
namespace result_of
{

/*
   gsse::result_of::val< >::type uses the the gsse::traits::tag_container_type metafunction
   to distinguish the type access.
   
*/

template <typename Sequence, typename Enable=void>
struct val : gsse::result_of::val< Sequence, Enable > {};

} // end namespace: result_of
//
// ===================================================================================
//
// ===================================================================================
//

namespace result_of {

template < typename GeometryElement, typename EvaluationTag >
struct create_conform_metric_impl { };

template < typename GeometryElement >
struct create_conform_metric_impl < GeometryElement, gml::tag_compiletime > 
{ 
   typedef gsse::metric_object< 
      typename gml::result_of::val< GeometryElement >::type, 
      gml::result_of::size< GeometryElement >::value 
   >           type;
};

template < typename GeometryElement >
struct create_conform_metric_impl < GeometryElement, gml::tag_runtime > 
{ 
   typedef gml::metric_object_rt<
      typename gml::result_of::val< GeometryElement >::type  
   >           type;
};

template < typename GeometryElement >
struct create_conform_metric
{
   typedef typename gml::result_of::create_conform_metric_impl< 
      GeometryElement,
      typename gml::traits::tag_of< GeometryElement >::evaluation_type
   >::type     type;
};
//
// ===================================================================================
//
template < typename Cell, typename EvaluationTag >
struct create_conform_cell_impl { };

template < typename Cell >
struct create_conform_cell_impl < Cell, gml::tag_compiletime > 
{ 
   typedef gsse::array<
      typename gml::result_of::val< Cell >::type, 
      gml::result_of::size< Cell >::value
   >           type;
};

template < typename Cell >
struct create_conform_cell_impl < Cell, gml::tag_runtime > 
{ 
   typedef std::vector<
      typename gml::result_of::val< Cell >::type 
   >           type;
};

template < typename Cell >
struct create_conform_cell
{
   typedef typename gml::result_of::create_conform_cell_impl< 
      Cell,
      typename gml::traits::tag_of< Cell >::evaluation_type
   >::type     type;
};

//
// ===================================================================================
//
template < int DIMT, int DIMBnd >
struct cell_decay
{
   typedef boost::mpl::pair< boost::mpl::int_< DIMT >, boost::mpl::int_< DIMBnd > > type;
};
//
// ===================================================================================
//

template< typename T1, typename T2 = void >
struct dim_result { };

template< long DIM, long DIMRel, typename CellTopology >
struct dim_result < gsse::boundary< DIM, DIMRel, CellTopology > >
{ 
   static const long value      = DIM - DIMRel;
};

template< long DIM, long DIMRel, typename CellTopology >
struct dim_result < gsse::coboundary< DIM, DIMRel, CellTopology > >
{ 
   static const long value      = DIM + DIMRel;
};

} // end namespace: result_of
//
// ===================================================================================
//
} // end namespace: gml
#endif






















