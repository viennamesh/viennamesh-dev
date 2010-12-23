/* ============================================================================
   Copyright (c) 2008-2009 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2003-2009 Rene Heinzl                             rene@gsse.at    

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/
   
#ifndef GML_UTIL_TRAITS
#define GML_UTIL_TRAITS
//
// ===================================================================================
//
// *** SYSTEM includes
//
#include <vector>  
#include <map>
//
// *** BOOST include    
//
#include <boost/type_traits/integral_constant.hpp>
#include <boost/type_traits/remove_pointer.hpp>
#include <boost/type_traits/is_arithmetic.hpp>
#include <boost/array.hpp>                   // boost::array
#include <boost/fusion/include/vector.hpp>
#include <boost/fusion/include/boost_tuple.hpp>
//
// *** GML include  
//
#include "tags.hpp"
#include "../datastructure/metric_object_rt.hpp"
//#include "../datastructure/container_wrapper.hpp"
//
// *** GSSE include    
//
#include <gsse/geometry/metric_object.hpp>   // gsse::metric_object
#include <gsse/util/common.hpp>              // gsse::array
//
// ===================================================================================
//
/*
namespace gsse
{
namespace traits    
{
template< typename T >
struct tag_of< gml::detail::container_wrapper_impl< T > >
{
   typedef typename gml::detail::container_wrapper_impl< T >::type type;
   typedef typename gml::detail::container_wrapper_impl< T >::evaluation_type evaluation_type;
   typedef typename gml::detail::container_wrapper_impl< T >::container_type container_type;
};  
} // end namespace: traits
} // end namespace: gsse
*/


namespace gml
{

namespace traits {

// ----------------------------------------------
template<typename T, typename Active = void>
struct tag_of;
// ----------------------------------------------
template <typename Sequence, typename Active>
struct tag_of
{
   typedef typename Sequence::tag_type             type;
   typedef typename Sequence::evaluation_type      evaluation_type;
   typedef typename Sequence::container_type       container_type;
   //typedef gml::tag_none                        type;
   //typedef gml::tag_none                        evaluation_type;
   //typedef gml::tag_none                        container_type; // gsse ..
};
// ----------------------------------------------
template<typename T, std::size_t N>
struct tag_of< boost::array<T,N> >
{
   typedef gml::tag_boost_array                 type;
   typedef gml::tag_compiletime                 evaluation_type;   
   typedef tag_compiletime_container            container_type; // gsse ..
}; 
template<typename T, std::size_t N>
struct tag_of< const boost::array<T,N> >
{
   typedef gml::tag_boost_array                 type;
   typedef gml::tag_compiletime                 evaluation_type;   
   typedef tag_compiletime_container            container_type; // gsse ..
}; 
template<typename T, long N>
struct tag_of< boost::array<T,N> >
{
   typedef gml::tag_boost_array                 type;
   typedef gml::tag_compiletime                 evaluation_type;      
   typedef tag_compiletime_container            container_type; // gsse ..
};
template<typename T, long N>
struct tag_of< const boost::array<T,N> >
{
   typedef gml::tag_boost_array                 type;
   typedef gml::tag_compiletime                 evaluation_type;      
   typedef tag_compiletime_container            container_type; // gsse ..
};
// ----------------------------------------------
template<typename T, std::size_t N>
struct tag_of< gsse::array<T,N> >
{
   typedef gml::tag_gsse_array                  type;
   typedef gml::tag_compiletime                 evaluation_type;      
   typedef tag_compiletime_container            container_type; // gsse ..   
};
template<typename T, std::size_t N>
struct tag_of< const gsse::array<T,N> >
{
   typedef gml::tag_gsse_array                  type;
   typedef gml::tag_compiletime                 evaluation_type;      
   typedef tag_compiletime_container            container_type; // gsse ..   
};
template<typename T, long N>
struct tag_of< gsse::array<T,N> >
{
   typedef gml::tag_gsse_array                  type;
   typedef gml::tag_compiletime                 evaluation_type;      
   typedef tag_compiletime_container            container_type; // gsse ..  
};
template<typename T, long N>
struct tag_of< const gsse::array<T,N> >
{
   typedef gml::tag_gsse_array                  type;
   typedef gml::tag_compiletime                 evaluation_type;      
   typedef tag_compiletime_container            container_type; // gsse ..  
};
// ----------------------------------------------
template<typename T, std::size_t N>
struct tag_of< gsse::metric_object<T,N> >
{
   typedef gml::tag_gsse_metric_object          type;
   typedef gml::tag_compiletime                 evaluation_type;      
   typedef tag_compiletime_container            container_type; // gsse .. 
};
template<typename T, std::size_t N>
struct tag_of< const gsse::metric_object<T,N> >
{
   typedef gml::tag_gsse_metric_object          type;
   typedef gml::tag_compiletime                 evaluation_type;      
   typedef tag_compiletime_container            container_type; // gsse .. 
};
template<typename T, long N>
struct tag_of< gsse::metric_object<T,N> >
{
   typedef gml::tag_gsse_metric_object          type;
   typedef gml::tag_compiletime                 evaluation_type;      
   typedef tag_compiletime_container            container_type; // gsse ..   
};
template<typename T, long N>
struct tag_of< const gsse::metric_object< T,N > >
{
   typedef gml::tag_gsse_metric_object          type;
   typedef gml::tag_compiletime                 evaluation_type;      
   typedef tag_compiletime_container            container_type; // gsse ..   
};
// ----------------------------------------------
template<typename TA, typename TB>
struct tag_of< gsse::map< TA, TB > >
{
   typedef gml::tag_gsse_map                    type;
   typedef gml::tag_runtime                     evaluation_type;      
   typedef tag_runtime_container                container_type; // gsse .. 
};
template<typename TA, typename TB>
struct tag_of< const gsse::map< TA, TB > >
{
   typedef gml::tag_gsse_map                    type;
   typedef gml::tag_runtime                     evaluation_type;      
   typedef tag_runtime_container                container_type; // gsse .. 
};
// ----------------------------------------------
template<typename T>
struct tag_of< gml::metric_object_rt< T > >
{
   typedef gml::tag_metric_object_rt            type;
   typedef gml::tag_runtime                     evaluation_type;      
   typedef tag_runtime_container                container_type; // gsse .. 
};
template<typename T>
struct tag_of< const gml::metric_object_rt< T > >
{
   typedef gml::tag_metric_object_rt            type;
   typedef gml::tag_runtime                     evaluation_type;      
   typedef tag_runtime_container                container_type; // gsse .. 
};
// ----------------------------------------------
template<typename T0>
struct tag_of< boost::tuples::tuple<T0> >
{
   typedef gml::tag_boost_tuples_tuple          type;
   typedef gml::tag_compiletime                 evaluation_type;      
   typedef tag_sequence_container               container_type; // gsse ..      
};
template<typename T0>
struct tag_of< const boost::tuples::tuple<T0> >
{
   typedef gml::tag_boost_tuples_tuple          type;
   typedef gml::tag_compiletime                 evaluation_type;      
   typedef tag_sequence_container               container_type; // gsse ..      
};
template<typename T0, typename T1>
struct tag_of< boost::tuples::tuple<T0, T1> >
{
   typedef gml::tag_boost_tuples_tuple          type;
   typedef gml::tag_compiletime                 evaluation_type;      
   typedef tag_sequence_container               container_type; // gsse ..      
};
template<typename T0, typename T1>
struct tag_of< const boost::tuples::tuple<T0, T1> >
{
   typedef gml::tag_boost_tuples_tuple          type;
   typedef gml::tag_compiletime                 evaluation_type;      
   typedef tag_sequence_container               container_type; // gsse ..      
};
template<typename T0, typename T1, typename T2>
struct tag_of< boost::tuples::tuple<T0, T1, T2> >
{
   typedef gml::tag_boost_tuples_tuple          type;
   typedef gml::tag_compiletime                 evaluation_type;      
   typedef tag_sequence_container               container_type; // gsse ..      
};
template<typename T0, typename T1, typename T2>
struct tag_of< const boost::tuples::tuple<T0, T1, T2> >
{
   typedef gml::tag_boost_tuples_tuple          type;
   typedef gml::tag_compiletime                 evaluation_type;      
   typedef tag_sequence_container               container_type; // gsse ..      
};
// ----------------------------------------------
template<typename T0>
struct tag_of< boost::fusion::vector<T0> >
{
   typedef gml::tag_fusion_vector               type;
   typedef gml::tag_compiletime                 evaluation_type;      
   typedef tag_sequence_container               container_type; // gsse ..      
};
template<typename T0>
struct tag_of< const boost::fusion::vector<T0> >
{
   typedef gml::tag_fusion_vector               type;
   typedef gml::tag_compiletime                 evaluation_type;      
   typedef tag_sequence_container               container_type; // gsse ..      
};
template<typename T0, typename T1>
struct tag_of< boost::fusion::vector<T0, T1> >
{
   typedef gml::tag_fusion_vector               type;
   typedef gml::tag_compiletime                 evaluation_type;      
   typedef tag_sequence_container               container_type; // gsse ..      
};
template<typename T0, typename T1>
struct tag_of< const boost::fusion::vector<T0, T1> >
{
   typedef gml::tag_fusion_vector               type;
   typedef gml::tag_compiletime                 evaluation_type;      
   typedef tag_sequence_container               container_type; // gsse ..      
};
template<typename T0, typename T1, typename T2>
struct tag_of< boost::fusion::vector<T0, T1, T2> >
{
   typedef gml::tag_fusion_vector               type;
   typedef gml::tag_compiletime                 evaluation_type;      
   typedef tag_sequence_container               container_type; // gsse ..      
};
template<typename T0, typename T1, typename T2>
struct tag_of< const boost::fusion::vector<T0, T1, T2> >
{
   typedef gml::tag_fusion_vector               type;
   typedef gml::tag_compiletime                 evaluation_type;      
   typedef tag_sequence_container               container_type; // gsse ..      
};
template<typename T0, typename T1, typename T2, typename T3>
struct tag_of< boost::fusion::vector<T0, T1, T2, T3> >
{
   typedef gml::tag_fusion_vector               type;
   typedef gml::tag_compiletime                 evaluation_type;      
   typedef tag_sequence_container               container_type; // gsse ..      
};
template<typename T0, typename T1, typename T2, typename T3>
struct tag_of< const boost::fusion::vector<T0, T1, T2, T3> >
{
   typedef gml::tag_fusion_vector               type;
   typedef gml::tag_compiletime                 evaluation_type;      
   typedef tag_sequence_container               container_type; // gsse ..      
};
// ----------------------------------------------
template<typename T0>
struct tag_of< boost::fusion::list<T0> >
{
   typedef gml::tag_fusion_list                 type;
   typedef gml::tag_compiletime                 evaluation_type;      
   typedef tag_sequence_container               container_type; // gsse ..      
};
template<typename T0>
struct tag_of< const boost::fusion::list<T0> >
{
   typedef gml::tag_fusion_list                 type;
   typedef gml::tag_compiletime                 evaluation_type;      
   typedef tag_sequence_container               container_type; // gsse ..      
};
template<typename T0, typename T1>
struct tag_of< boost::fusion::list<T0, T1> >
{
   typedef gml::tag_fusion_list                 type;
   typedef gml::tag_compiletime                 evaluation_type;      
   typedef tag_sequence_container               container_type; // gsse ..      
};
template<typename T0, typename T1>
struct tag_of< const boost::fusion::list<T0, T1> >
{
   typedef gml::tag_fusion_list                 type;
   typedef gml::tag_compiletime                 evaluation_type;      
   typedef tag_sequence_container               container_type; // gsse ..      
};
template<typename T0, typename T1, typename T2>
struct tag_of< boost::fusion::list<T0, T1, T2> >
{
   typedef gml::tag_fusion_list                 type;
   typedef gml::tag_compiletime                 evaluation_type;      
   typedef tag_sequence_container               container_type; // gsse ..      
};
template<typename T0, typename T1, typename T2>
struct tag_of< const boost::fusion::list<T0, T1, T2> >
{
   typedef gml::tag_fusion_list                 type;
   typedef gml::tag_compiletime                 evaluation_type;      
   typedef tag_sequence_container               container_type; // gsse ..      
};
template<typename T0, typename T1, typename T2, typename T3>
struct tag_of< boost::fusion::list<T0, T1, T2, T3> >
{
   typedef gml::tag_fusion_list                 type;
   typedef gml::tag_compiletime                 evaluation_type;      
   typedef tag_sequence_container               container_type; // gsse ..      
};
template<typename T0, typename T1, typename T2, typename T3>
struct tag_of< const boost::fusion::list<T0, T1, T2, T3> >
{
   typedef gml::tag_fusion_list                 type;
   typedef gml::tag_compiletime                 evaluation_type;      
   typedef tag_sequence_container               container_type; // gsse ..      
};
// ----------------------------------------------
template<typename T>
struct tag_of< std::vector< T > >
{
   typedef gml::tag_std_vector                  type;
   typedef gml::tag_runtime                     evaluation_type;      
   typedef tag_runtime_container                container_type; // gsse .. 
};
template<typename T>
struct tag_of< const std::vector< T > >
{
   typedef gml::tag_std_vector                  type;
   typedef gml::tag_runtime                     evaluation_type;      
   typedef tag_runtime_container                container_type; // gsse .. 
};
template<typename T1, typename T2>
struct tag_of< std::vector< T1, T2 > >
{
   typedef gml::tag_std_vector                  type;
   typedef gml::tag_runtime                     evaluation_type;      
   typedef tag_runtime_container                container_type; // gsse .. 
};
template<typename T1, typename T2>
struct tag_of< const std::vector< T1, T2 > >
{
   typedef gml::tag_std_vector                  type;
   typedef gml::tag_runtime                     evaluation_type;      
   typedef tag_runtime_container                container_type; // gsse .. 
};
// ----------------------------------------------
template<typename TA, typename TB>
struct tag_of< std::map< TA, TB > >
{
   typedef gml::tag_std_map                     type;
   typedef gml::tag_runtime                     evaluation_type;
   typedef tag_runtime_container                container_type;
};
template<typename TA, typename TB>
struct tag_of< const std::map< TA, TB > >
{
   typedef gml::tag_std_map                     type;
   typedef gml::tag_runtime                     evaluation_type;
   typedef tag_runtime_container                container_type;
};
// ----------------------------------------------
template<typename TA, typename TB>
struct tag_of< std::set< TA, TB > >
{
   typedef gml::tag_std_set                     type;
   typedef gml::tag_runtime                     evaluation_type;
   typedef tag_runtime_container                container_type;
};
template<typename TA, typename TB>
struct tag_of< const std::set< TA, TB > >
{
   typedef gml::tag_std_set                     type;
   typedef gml::tag_runtime                     evaluation_type;
   typedef tag_runtime_container                container_type;
};
// ----------------------------------------------
template<typename TA, typename TB>
struct tag_of< std::pair< TA, TB > >
{
   typedef gml::tag_std_pair                    type;
   typedef gml::tag_runtime                     evaluation_type;
   typedef tag_runtime_container                container_type;
};
template<typename TA, typename TB>
struct tag_of< const std::pair< TA, TB > >
{
   typedef gml::tag_std_pair                    type;
   typedef gml::tag_runtime                     evaluation_type;
   typedef tag_runtime_container                container_type;
};
// ----------------------------------------------
template<typename TA, typename TB>
struct tag_of< std::tr1::unordered_map< TA, TB > >  
{
   typedef gml::tag_std_tr1_unordered_map       type;
   typedef gml::tag_runtime                     evaluation_type;
   typedef tag_runtime_container                container_type;
};
template<typename TA, typename TB>
struct tag_of< const std::tr1::unordered_map< TA, TB > >  
{
   typedef gml::tag_std_tr1_unordered_map       type;
   typedef gml::tag_runtime                     evaluation_type;
   typedef tag_runtime_container                container_type;
};
// ----------------------------------------------
}// end namespace: traits

} // end namespace: gml


#endif
