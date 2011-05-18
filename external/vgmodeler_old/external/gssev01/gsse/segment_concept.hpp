/* ************* :: Generic Scientific Simulation Environment :: *************
 **  http://www.gsse.at                                                     **

     Copyright (c) 2003-2008 Rene Heinzl                 rene@gsse.at
     Copyright (c) 2004-2008 Philipp Schwaha          philipp@gsse.at

     Use, modification and distribution is subject to the Boost Software
     License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 **  http://www.boost.org/LICENSE_1_0.txt)                                  **
 *************************************************************************** */

#ifndef GSSE_SEGMENT_CONCEPTS_HH_ID
#define GSSE_SEGMENT_CONCEPTS_HH_ID

#include "gsse/quantity_traits.hpp"
#include "gsse/quantity_concept.hpp"
#include "gsse/topology/topology_concepts.hpp"


namespace gsse
{
namespace concepts
{

template <class ST>
struct GSSESegmentConcept
{
  void constraints() 
  {
    ST        seg;
    ST const  seg_c;

    typedef typename segment_traits<ST>::topology_type                       topology_type;

    typedef typename segment_traits<ST>::segment_key_iterator                segment_key_iterator;
    typedef typename segment_traits<ST>::segment_skey_iterator               segment_skey_iterator;
    typedef typename segment_traits<ST>::segment_vertex_quantity_iterator    vertex_key_iterator;
    typedef typename segment_traits<ST>::segment_edge_quantity_iterator      edge_key_iterator;
    typedef typename segment_traits<ST>::segment_facet_quantity_iterator     facet_key_iterator;
    typedef typename segment_traits<ST>::segment_cell_quantity_iterator      cell_key_iterator;


    typedef typename segment_traits<ST>::vertex_type    vertex_type;
    typedef typename segment_traits<ST>::edge_type      edge_type;
    typedef typename segment_traits<ST>::facet_type     facet_type;
    typedef typename segment_traits<ST>::cell_type      cell_type;
    
    typedef typename segment_traits<ST>::vertex_handle  vertex_handle;
    typedef typename segment_traits<ST>::edge_handle    edge_handle;
    typedef typename segment_traits<ST>::facet_handle   facet_handle;
    typedef typename segment_traits<ST>::cell_handle    cell_handle;

    typedef typename segment_traits<ST>::vertex_iterator vertex_iterator;
    typedef typename segment_traits<ST>::edge_iterator   edge_iterator;
    typedef typename segment_traits<ST>::facet_iterator  facet_iterator;
    typedef typename segment_traits<ST>::cell_iterator   cell_iterator;

    boost::function_requires< GSSETopologyConcept<topology_type> > ();

    // gsse segment concept checks

    // --------------------------
    // naming
    //
    std::string s;
    seg.set_name(s);
    s = seg_c.get_segment_name();


    // --------------------------
    // segment quantity key iterations
    //
    segment_key_iterator sk_it;
    sk_it = seg_c.segment_key_begin();
    sk_it = seg_c.segment_key_end();

    segment_skey_iterator ssk_it;
    ssk_it = seg_c.segment_skey_begin();
    ssk_it = seg_c.segment_skey_end();

    vertex_key_iterator vk_it;
    vk_it = seg_c.vertex_key_begin();
    vk_it = seg_c.vertex_key_end();

    edge_key_iterator ek_it;
    ek_it = seg_c.edge_key_begin();
    ek_it = seg_c.edge_key_end();

    facet_key_iterator fk_it;
    fk_it = seg_c.facet_key_begin();
    fk_it = seg_c.facet_key_end();

    cell_key_iterator ck_it;
    ck_it = seg_c.cell_key_begin();
    ck_it = seg_c.cell_key_end();


    // --------------------------
    // quantity key sizes
    //
    unsigned int size;

    size = seg_c.vertex_key_size();
    size = seg_c.edge_key_size();
    size = seg_c.facet_key_size();
    size = seg_c.cell_key_size();
    size = seg_c.segment_key_size();

    size = seg_c.vertex_size();
    size = seg_c.edge_size();
    size = seg_c.facet_size();
    size = seg_c.cell_size();


    // --------------------------
    // topological traversal
    //
    vertex_iterator vertex_it = seg_c.vertex_begin();
    vertex_it                 = seg_c.vertex_begin();

    edge_iterator edge_it     = seg.edge_begin();    // [RH][TODO] .. maybe we can a const segment as well 
    edge_it                   = seg.edge_begin();

    facet_iterator facet_it   = seg.facet_begin();
    facet_it                  = seg.facet_begin();

    cell_iterator cell_it     = seg_c.cell_begin();
    cell_it                   = seg_c.cell_begin();


//     boost::function_requires<GSSEIteratorConcept<edge_iterator> > ();
//     boost::function_requires<GSSEIteratorConcept<facet_iterator> > ();

    
    // --------------------------
    // topology retrieval
    //
    topology_type&       topo   = seg.retrieve_topology();
    topology_type const& topo_c = seg_c.retrieve_topology();

    ignore_unused_variable_warning(topo);
    ignore_unused_variable_warning(topo_c);
  }
};

}
}

#endif
