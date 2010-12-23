/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha          philipp@gsse.at
     Copyright (c) 2005-2006 Michael Spevak

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#ifndef TOPOLOGY_CONCEPTS_HH_ID
#define TOPOLOGY_CONCEPTS_HH_ID

#define BOOST_NO_STD_ITERATOR_TRAITOBS
#include <boost/concept_check.hpp>
#undef BOOST_NO_STD_ITERATOR_TRAITS


namespace gsse
{
namespace concepts
{

template <class TT>
struct GSSEIteratorConcept
{
  void constraints() 
  {
     // [RH][TODO] .. this style is deprectated .. but as I can see, boost::concept check 
     //  does not provide it as before.. only on newer compilers??
     //
     //boost::function_requires< boost::BidirectionalIteratorConcept<TT> >();

    TT t;
    t.valid();
    t.reset();
  }
};


template <class TT>
struct GSSERAIteratorConcept
{
  void constraints() 
  {
     // [RH][TODO] .. this style is deprectated .. but as I can see, boost::concept check 
     //  does not provide it as before.. only on newer compilers??
     //
     // boost::function_requires< boost::RandomAccessIteratorConcept<TT> >();

    TT t;
    t.valid();
    t.reset();
  }
};

template <class TT>
struct GSSELocalRAIteratorConcept
{
  void constraints() 
  {
    boost::function_requires< GSSEIteratorConcept<TT> >();    
     // [RH][TODO] .. this style is deprectated .. but as I can see, boost::concept check 
     //  does not provide it as before.. only on newer compilers??
     //
    // boost::function_requires< boost::RandomAccessIteratorConcept<TT> >();

    typename TT::base_type b;
    TT t(b);
    b = t.base();
  }
};


template <class TT>
struct GSSELocalIteratorConcept
{
  void constraints() 
  {
    boost::function_requires< GSSEIteratorConcept<TT> >();    

    typename TT::base_type b;
    TT t(b);
    b = t.base();
  }
};

template <class TC>
struct GSSETopologyContainerConcept
{
  void constraints()
  {
    typename topology_traits<TC>::vertex_iterator vit;

    TC tc;
    vit = tc.vertex_begin();
    vit = tc.vertex_end();

    typename topology_traits<TC>::cell_iterator cit;

    cit = tc.cell_begin();
    cit = tc.cell_end();
  }
};



template <class TE>
struct GSSETopologyElementConcept
{
  void constraints()
  {
     // [RH][TODO] .. this style is deprectated .. but as I can see, boost::concept check 
     //  does not provide it as before.. only on newer compilers??
     //
//     boost::function_requires< boost::AssignableConcept<TE> > ();
//     boost::function_requires< boost::EqualityComparableConcept<TE> > ();
//     boost::function_requires< boost::LessThanComparableConcept<TE> > ();

    TE const elem;
    
    typedef typename TE::handle_type handle_type;
    handle_type handle = elem.handle();

    typedef typename TE::topology_type topo_type;
    topo_type const& tt(elem.topo_anchor());

    TE elem2(tt);
    TE elem3(tt, handle);
  }
};

template <class TH>
struct GSSETopologyHandleConcept
{
  void constraints()
  {
     // [RH][TODO] .. this style is deprectated .. but as I can see, boost::concept check 
     //  does not provide it as before.. only on newer compilers??
     //
//     boost::function_requires< boost::AssignableConcept<TH> >();
//     boost::function_requires< boost::EqualityComparableConcept<TH> >();
//     boost::function_requires< boost::LessThanComparableConcept<TH> >();
  }
};

template <typename TopologyT>
struct GSSETopologyConcept
{
  void constraints()
  {
    typedef typename topology_traits<TopologyT>::vertex_type               vertex_type;
    typedef typename topology_traits<TopologyT>::cell_type                 cell_type;
    typedef typename topology_traits<TopologyT>::edge_type                 edge_type;
    typedef typename topology_traits<TopologyT>::facet_type                facet_type;
    
    typedef typename topology_traits<TopologyT>::cell_handle               cell_handle;
    typedef typename topology_traits<TopologyT>::vertex_handle             vertex_handle;
    typedef typename topology_traits<TopologyT>::facet_handle              facet_handle;
    typedef typename topology_traits<TopologyT>::edge_handle               edge_handle;

    typedef typename topology_traits<TopologyT>::vertex_iterator           vertex_iterator;
    typedef typename topology_traits<TopologyT>::cell_iterator             cell_iterator;
    
    typedef typename topology_traits<TopologyT>::vertex_on_edge_iterator   edge_vertex;
    typedef typename topology_traits<TopologyT>::vertex_on_facet_iterator  facet_vertex;
    typedef typename topology_traits<TopologyT>::vertex_on_cell_iterator   cell_vertex;
    
    typedef typename topology_traits<TopologyT>::edge_on_vertex_iterator   vertex_edge;
    typedef typename topology_traits<TopologyT>::edge_on_facet_iterator    facet_edge;
    typedef typename topology_traits<TopologyT>::edge_on_cell_iterator     cell_edge;
    
    typedef typename topology_traits<TopologyT>::facet_on_edge_iterator    edge_facet;
    typedef typename topology_traits<TopologyT>::facet_on_vertex_iterator  vertex_facet;
    typedef typename topology_traits<TopologyT>::facet_on_cell_iterator    cell_facet;
    
    typedef typename topology_traits<TopologyT>::cell_on_edge_iterator     edge_cell;
    typedef typename topology_traits<TopologyT>::cell_on_facet_iterator    facet_cell;
    typedef typename topology_traits<TopologyT>::cell_on_vertex_iterator   vertex_cell;  
    typedef typename topology_traits<TopologyT>::vertex_on_vertex_iterator vertex_vertex;  

    typedef typename topology_traits<TopologyT>::template 
      vertex_on_element_iterator<vertex_type>::type vertex_vertex_2;

    typedef typename topology_traits<TopologyT>::template 
      vertex_on_element_iterator<edge_type>::type edge_vertex_2;

    typedef typename topology_traits<TopologyT>::template 
      vertex_on_element_iterator<facet_type>::type facet_vertex_2;

    typedef typename topology_traits<TopologyT>::template 
      vertex_on_element_iterator<cell_type>::type cell_vertex_2;


    BOOST_STATIC_ASSERT((boost::is_same<vertex_vertex, vertex_vertex_2>::value));
    BOOST_STATIC_ASSERT((boost::is_same<edge_vertex,   edge_vertex_2>::value));
    BOOST_STATIC_ASSERT((boost::is_same<facet_vertex,  facet_vertex_2>::value));
    BOOST_STATIC_ASSERT((boost::is_same<cell_vertex,   cell_vertex_2>::value));
    
 
    boost::function_requires< GSSETopologyContainerConcept<TopologyT> >();
    
    boost::function_requires< GSSEIteratorConcept<vertex_iterator> >();
    boost::function_requires< GSSEIteratorConcept<cell_iterator> >();
    
    boost::function_requires< GSSELocalRAIteratorConcept<cell_vertex> >();
    boost::function_requires< GSSELocalIteratorConcept<cell_edge> >();
    boost::function_requires< GSSELocalIteratorConcept<cell_facet> >();
    boost::function_requires< GSSELocalRAIteratorConcept<edge_vertex> >();
    boost::function_requires< GSSELocalIteratorConcept<edge_cell> >();
    boost::function_requires< GSSELocalIteratorConcept<edge_facet> >();
    boost::function_requires< GSSELocalRAIteratorConcept<facet_vertex> >();
    boost::function_requires< GSSELocalIteratorConcept<facet_edge> >();
    boost::function_requires< GSSELocalIteratorConcept<facet_cell> >();
    boost::function_requires< GSSELocalIteratorConcept<vertex_cell> >();
    boost::function_requires< GSSELocalIteratorConcept<vertex_edge> >();
    boost::function_requires< GSSELocalIteratorConcept<vertex_facet> >();
    boost::function_requires< GSSELocalRAIteratorConcept<vertex_vertex> >();
    
    boost::function_requires< GSSETopologyHandleConcept<vertex_handle> >();
    boost::function_requires< GSSETopologyHandleConcept<edge_handle> >();
    boost::function_requires< GSSETopologyHandleConcept<facet_handle> >();
    boost::function_requires< GSSETopologyHandleConcept<cell_handle> >();
    
    boost::function_requires< GSSETopologyElementConcept<vertex_type> >();
    boost::function_requires< GSSETopologyElementConcept<edge_type> >();
    boost::function_requires< GSSETopologyElementConcept<facet_type> >();
    boost::function_requires< GSSETopologyElementConcept<cell_type> >();
  }
};

}
}


#endif
