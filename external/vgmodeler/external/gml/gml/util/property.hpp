/* ============================================================================
   Copyright (c) 2008-2009 Josef Weinbub                          josef@gsse.at        
   Copyright (c) 2003-2009 Rene Heinzl                             rene@gsse.at       

   Distributed under the Boost Software License, Version 1.0.
   See accompanying LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt
   ============================================================================*/

#ifndef GML_UTIL_PROPERTY
#define GML_UTIL_PROPERTY
//
// ===================================================================================
//
// *** GML include  
//
#include "tags.hpp"
#include "common.hpp"
//
// ===================================================================================
//
namespace gml {

namespace detail {

template <  typename TopologyOperation1,
            typename TopologyContainer1,
            typename TopologyOperation2,
            typename TopologyContainer2,
            typename CellTopology  >
struct property_data_impl
{
   typedef CellTopology cell_topology;

   typedef TopologyContainer1                                                       topology_container_1_type;
   typedef typename gml::result_of::val< TopologyContainer1 >::type                 topology_element_1_type; 

   typedef TopologyContainer2                                                       topology_container_2_type;
   typedef typename gml::result_of::val< TopologyContainer2 >::type                 topology_element_2_type; 

   typedef TopologyOperation1                                                        topology_operation_1_type;
   typedef TopologyOperation2                                                        topology_operation_2_type;
   
   static const long DIMResult1 = gml::result_of::dim_result< TopologyOperation1 >::value;
   static const long DIMResult2 = gml::result_of::dim_result< TopologyOperation2 >::value;
};

template < typename TopologyOperation1,
           typename TopologyContainer1,
           typename CellTopology  >
struct property_data_impl <  TopologyOperation1,
                             TopologyContainer1,
                             boost::fusion::void_,
                             boost::fusion::void_,
                             CellTopology >
{
   typedef CellTopology cell_topology;

   typedef TopologyContainer1                                                       topology_container_type;
   typedef typename gml::result_of::val< TopologyContainer1 >::type                 topology_element_type; 

   typedef TopologyOperation1                                                       topology_operation_type;
   
   static const long DIMResult = gml::result_of::dim_result< TopologyOperation1 >::value;
};

} // end namespace: detail

template <  typename TopologyOperation1,
            typename TopologyContainer1,
            typename TopologyOperation2 = boost::fusion::void_, 
            typename TopologyContainer2 = boost::fusion::void_,
            typename CellTopology       = gml::cell_simplex >
struct property_data
{
   typedef gml::detail::property_data_impl< 
      TopologyOperation1,
      TopologyContainer1,
      TopologyOperation2,
      TopologyContainer2,
      CellTopology
   >     type; 
};

} // end namespace: gml
/*
namespace gml {

namespace detail {

template <  typename TopologyContainer1,
            typename TopologyContainer2  >
struct property_impl
{
   typedef TopologyContainer1                                                       topology_container_1_type;
   typedef typename gml::result_of::val< TopologyContainer1 >::type                 topology_element_1_type; 
   typedef typename gml::result_of::val< topology_element_1_type >::type            index_1_type;
   typedef typename gml::traits::tag_of< topology_element_1_type >::evaluation_type topology_element_1_evaluation_type;    

   typedef TopologyContainer2                                                       topology_container_2_type;
   typedef typename gml::result_of::val< TopologyContainer2 >::type                 topology_element_2_type; 
   typedef typename gml::result_of::val< topology_element_2_type >::type            index_2_type;
   typedef typename gml::traits::tag_of< topology_element_2_type >::evaluation_type topology_element_2_evaluation_type;    
};

template < typename TopologyContainer1  >
struct property_impl <  TopologyContainer1,
                        boost::fusion::void_ >
{
   typedef TopologyContainer1                                                       topology_container_type;
   typedef typename gml::result_of::val< TopologyContainer1 >::type                 topology_element_type; 
   typedef typename gml::result_of::val< topology_element_type >::type              index_type;
   typedef typename gml::traits::tag_of< topology_element_type >::evaluation_type   topology_element_evaluation_type;    
};

} // end namespace: detail

template <  typename TopologyContainer1,
            typename TopologyContainer2 = boost::fusion::void_ >
struct property
{
   typedef gml::detail::property_impl< 
      TopologyContainer1,
      TopologyContainer2
   >     type; 
};

} // end namespace: gml

*/
#endif



















